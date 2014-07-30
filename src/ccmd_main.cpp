/** @mainpage Coulomb Crystal Molecular Dynamics simulation
 *
 *  # Introduction
 *  
 *  This program integrates the equations of motion for a collection of ions
 *  contained in a quadrupole radio-frequency trap. The ions can have arbitrary
 *  mass and charge, and optionally be laser cooled or subject to randomised
 *  heating.
 *
 *  # Usage
 *
 *  The program is designed to be run from the command line, with the path to 
 *  the working directory as an argument:
 *
 *      <path to ccmd>/ccmd <path to working directory>
 *
 *  The input file must be named \c trap.info and the file format is documented in ccmd_sim.h. The file is divided
 *  into sections, one for each set of related parameters. These are documented
 *  in detail in Trap_params, Cloud_params and Integration_params.
 *  Output files are written
 *  to the same directory. The puropse of this is to keep generated data
 *  together with the input file that generated it, somewhat documenting the
 *  parameters used in a particular simulation.
 *
 *
 *  # Description
 *
 *  After loading the simulation parameters, the program integrates the
 *  equations of motion of the trapped ions. The first run for a total of
 *  Integration_params.cool_steps time steps allows the ions to equilibrate,
 *  then for the number of time steps in Integration_params.hist_steps while
 *  collecting statistics on each ion's position and kinetic energy. The
 *  positions are stored in a 3-dimensional histogram, which is used to
 *  generate a representation of a microscope image of the crystal. Here, each
 *  layer of the histogram represents a pixel brightness that is blurred
 *  proportional to its distance from a focal plane (taken as the centre of the
 *  crystal.)
 *
 *  # Base Classes
 *
 *  The primary classes used to drive the simulation are the Ion_cloud,
 *  Ion_trap, and Integrator. These are base classes that provide a uniform
 *  interface to more specific implementations. The simulation progresses by
 *  calling Integrator.evolve which calls Ion_trap.evolve to update the
 *  trapping voltages, and then calculates the forces on each ion and updates
 *  position and velocity.
 *
 */

#include "ccmd_image.h"
#include "ccmd_sim.h"
#include "DataWriter.h"
#include "ion_trap.h"
#include "ion_cloud.h"
#include "IonHistogram.h"
#include "ImageCollection.h"
#include "integrator.h"
#include "logger.h"
#include "stats.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <list>
#include <memory>
#include <fstream>
#include <sstream>

double stopWatchTimer();
double KE;
void printProgBar( int percent );

double stopWatchTimer() {
    static clock_t start = clock();
    double time_elapsed;
    time_elapsed = ( std::clock() - start )/static_cast<double>(CLOCKS_PER_SEC); 
    return time_elapsed;
}

void printProgBar( int percent ){
    std::string bar;
    for(int i = 0; i < 50; i++){
        if( i < (percent/2)){
            bar.replace(i,1,"=");
        }else if( i == (percent/2)){
            bar.replace(i,1,">");
        }else{
            bar.replace(i,1," ");
        }
    }
    std::cout << '\r' << "[" << bar << "] ";
    std::cout.width( 3 );
    std::cout<< percent << "%     " << std::flush;
}

using namespace std;


int main (int argc, char * const argv[]) {
    Logger& log = Logger::getInstance();
    
    if (argc != 2) {
	std::cout << "usage: " << argv[0] << " [working directory]" << std::endl;
	std::exit(1);
    }

    std::string path = std::string(argv[1]);
    if (path[path.length()-1] != '/') 
	    path += "/";

    log.initialise(Logger::debug, path + "log.txt");
    log.log(Logger::info, "CCMD - Coulomb crystal molecular dynamics");
    log.log(Logger::info, "Version 1.0.0");
    
    // Parameter file paths
    string info_file = path + "trap.info";
    log.log(Logger::info, "Loading input file " + info_file);
    
    try {
        // Get simulation parameters from files
        Trap_params trap_params(info_file);
        Cloud_params cloud_params(info_file);
        Integration_params integrator_params(info_file);
        Microscope_params microscope_params(info_file);
        Sim_params sim_params(info_file);
        
        // Construct trap based on parameters
        std::shared_ptr<Ion_trap> trap;
        if (trap_params.wave == trap_params.cosine) {
            trap = std::make_shared<Cosine_trap>(trap_params);
        } else if (trap_params.wave == trap_params.digital) {
            trap = std::make_shared<Pulsed_trap>(trap_params);
        } else if (trap_params.wave == trap_params.waveform) {
            trap = std::make_shared<Waveform_trap>(trap_params);
        } else if (trap_params.wave == trap_params.cosine_decay) {
                // have to do some unit conversions here for the decay params
                trap_params.tau *= M_PI;
                trap_params.deltaT = (integrator_params.cool_steps + integrator_params.hist_steps)*integrator_params.time_step-trap_params.deltaT*M_PI;
            trap = std::make_shared<Cosine_decay_trap>(trap_params);
        } else if (trap_params.wave == trap_params.twofreq) {
            trap = std::make_shared<TwoFreq_trap>(trap_params);
        } else {
            log.log(Logger::error, "Unrecognised trap type");
            throw runtime_error("Unrecognised trap type");
        }
        
        // Construct ion cloud
        Ion_cloud cloud(trap, cloud_params, sim_params);
        
        // Construct integrator
        RESPA_integrator integrator(trap, cloud, integrator_params, sim_params);

        // Construct integrator
        //CUDA_integrator integrator(trap, cloud, integrator_params);
        
        // 3D histogram for image creation
        ImageCollection ionImages((1.0)/(1e6 * microscope_params.pixels_to_distance * trap->get_length_scale()));
        
        // Cool down ion cloud
        cout << flush << "Running cool down" << endl;
        log.log(Logger::info, "Running cool down.");
        int nt_cool = integrator_params.cool_steps;
        double dt = integrator_params.time_step;
        DataWriter writer("\t");
        writer.writeComment(path + "totalEnergy.csv", "t\tE_tot");

        //int write_every = std::floor(6.2831/(15*dt));
        // Write frame once every 2 RF cycles
        int write_every = (integrator_params.steps_per_period);
        write_every = std::max(1, write_every);
        log.log(Logger::debug, "Writing one frame every " + std::to_string(write_every));
        int frameNumber=0;

        Stats<double> mean_energy;
        std::string stats_file = path + "energy.csv";
        int energy_row = 0;

//------------------------------------------------------------------------------
// Cooling
//------------------------------------------------------------------------------
        for (int t=0; t<nt_cool; ++t) {
//            cloud->collide();
//            if (cloud->number_of_ions() ==0) {
//                log.log(Logger::warn, "No ions remaining, stopping.");
//                break;
//            }
            
            integrator.evolve(dt);
            mean_energy.append(cloud.kinetic_energy());
            
            if (t%write_every==0) {
                std::list<double> rowdata;
                rowdata.push_back(energy_row++);
                rowdata.push_back(mean_energy.average());
                rowdata.push_back(mean_energy.variance());
                writer.writeRow(stats_file, rowdata);
                mean_energy.reset();

                //char buffer[50];
                //std::sprintf(buffer, "%.4i", frameNumber++);
                //std::string framepath = buffer;
                //cloud->savePos(framepath, trap->get_length_scale(), trap->get_time_scale());
            }
            // Track progress
            int percent = static_cast<int>( (t*100)/nt_cool );
            if ( (t*100/5)%nt_cool == 0 ) {
                printProgBar(percent);
            } 
        }
        printProgBar(100);
        cout << '\n';
        
        // Evolution
        int nt = integrator_params.hist_steps;

//------------------------------------------------------------------------------
// Histogram 
//------------------------------------------------------------------------------
        
        cout << flush << "Acquiring histogram data" << endl;
        log.log(Logger::info, "Acquiring histogram data");

	    // Start timer
        stopWatchTimer();
        KE = 0;
        double etot = 0;
        int swap_count = 0;
        //if (swap_params.do_swap)
            //swap_count = std::floor(0.5*nt/swap_params.from.number);
        for (int t=0; t<nt; ++t) {
//            cloud->collide();
//            if (cloud->number_of_ions() ==0) {
//                log.log(Logger::warn, "No ions remaining, stopping.");
//                break;
//            }
            
            integrator.evolve(dt);
            if (microscope_params.makeimage)
                cloud.update_position_histogram(ionImages);

            cloud.updateStats();
            
            // Track progress
            int percent = static_cast<int>( (t*100)/nt );
            if ( (t*100/5)%nt == 0) {
                printProgBar(percent);
                cout << setw(4) << stopWatchTimer() << "s";
            }
            KE += cloud.kinetic_energy();
            etot += cloud.total_energy();

        //if (swap_params.do_swap){
            //if (t%swap_count==0) 
                //cloud->swap_first(swap_params.from, swap_params.to);
        //}

            //if (t%write_every==0) {
                //char buffer[50];
                //std::sprintf(buffer, "%.4i", frameNumber++);
                //std::string framepath = buffer;
                //cloud->savePos(framepath, 
                        //trap->get_length_scale(), trap->get_time_scale());
            //}
        }
        KE /= nt;
        printProgBar(100);
        std::cout << endl;
        
        cout << "total kinetic energy = " << KE << endl;
        cout << "Total energy = " << etot << endl;

        log.log(Logger::info, "total kinetic energy = " + std::to_string(KE));
        log.log(Logger::info, "total energy = " + std::to_string(etot));
        
        if (microscope_params.makeimage)
        {
            ionImages.writeFiles(path, microscope_params);
        }
        cloud.saveStats(path, trap->get_length_scale(), trap->get_time_scale());

    } catch (std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

