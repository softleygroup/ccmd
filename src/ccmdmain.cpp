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
 *  The input file must be named \c trap.info and the file format is documented
 *  in ccmdsim.h. The file is divided into sections, one for each set of
 *  related parameters. These are documented in detail in TrapParams,
 *  CloudParams and IntegrationParams.  Output files are written to the same
 *  directory. The puropse of this is to keep generated data together with the
 *  input file that generated it, somewhat documenting the parameters used in a
 *  particular simulation.
 *
 *
 *  # Description
 *
 *  After loading the simulation parameters, the program integrates the
 *  equations of motion of the trapped ions. The first run for a total of
 *  IntegrationParams.cool_steps time steps allows the ions to equilibrate,
 *  then for the number of time steps in IntegrationParams.hist_steps while
 *  collecting statistics on each ion's position and kinetic energy. The
 *  positions are stored in a 3-dimensional histogram, which is used to
 *  generate a representation of a microscope image of the crystal. Here, each
 *  layer of the histogram represents a pixel brightness that is blurred
 *  proportional to its distance from a focal plane (taken as the centre of the
 *  crystal.)
 *
 *  # Base Classes
 *
 *  The primary classes used to drive the simulation are the IonCloud,
 *  IonTrap, and Integrator. These are base classes that provide a uniform
 *  interface to more specific implementations. The simulation progresses by
 *  calling Integrator.evolve which calls IonTrap.evolve to update the
 *  trapping voltages, and then calculates the forces on each ion and updates
 *  position and velocity.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#include <ctime>
#include <iomanip>
#include <iostream>
#include <list>
#include <algorithm>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>

#include "include/image.h"
#include "include/ccmdsim.h"
#include "include/datawriter.h"
#include "include/iontrap.h"
#include "include/ioncloud.h"
#include "include/ionhistogram.h"
#include "include/ionstatslistener.h"
#include "include/imagehistogramlistener.h"
#include "include/imagecollection.h"
#include "include/integrator.h"
#include "include/logger.h"
#include "include/meanlistener.h"
#include "include/stats.h"
#include "include/timer.h"

double stopWatchTimer();
double KE;
void printProgBar(int percent);

double stopWatchTimer() {
    static clock_t start = clock();
    double time_elapsed;
    time_elapsed = (std::clock() - start)/static_cast<double>(CLOCKS_PER_SEC);
    return time_elapsed;
}

void printProgBar(int percent) {
    std::string bar;
    for (int i = 0; i < 50; i++) {
        if ( i < (percent/2)) {
            bar.replace(i, 1, "=");
        } else if ( i == (percent/2)) {
            bar.replace(i, 1, ">");
        } else {
            bar.replace(i, 1, " ");
        }
    }
    std::cout << '\r' << "[" << bar << "] ";
    std::cout.width(3);
    std::cout<< percent << "%     " << std::flush;
}

int main(int argc, char * const argv[]) {
    double PI = 3.1415926535897932;
    Timer timer;
    Logger& log = Logger::getInstance();

    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " [working directory]" << std::endl;
        std::exit(1);
    }

    std::string path = std::string(argv[1]);
    if (path[path.length()-1] != '/')
        path += "/";

    log.initialise(Logger::DEBUG, path + "log.txt");
    log.info("CCMD - Coulomb crystal molecular dynamics");
    log.info("Version 2.1.0");

    // Parameter file paths
    std::string info_file = path + "trap.info";
    log.info("Loading input file " + info_file);

    try {
        // Get simulation parameters from files
        TrapParams trap_params(info_file);
        CloudParams cloud_params(info_file);
        IntegrationParams integration_params(info_file);
        MicroscopeParams microscope_params(info_file);
        SimParams sim_params(info_file);

        // Construct trap based on parameters
        IonTrap_ptr trap;
        if (trap_params.wave == trap_params.cosine) {
            trap = std::make_shared<CosineTrap>(trap_params);
        } else if (trap_params.wave == trap_params.digital) {
            trap = std::make_shared<PulsedTrap>(trap_params);
        } else if (trap_params.wave == trap_params.waveform) {
            trap = std::make_shared<WaveformTrap>(trap_params);
        } else if (trap_params.wave == trap_params.cosine_decay) {
            // have to do some unit conversions here for the decay params
            trap_params.tau *= PI;
            trap_params.deltaT = (integration_params.cool_steps + integration_params.hist_steps)*
                integration_params.time_step-trap_params.deltaT*PI;
            trap = std::make_shared<CosineDecayTrap>(trap_params);
        } else if (trap_params.wave == trap_params.twofreq) {
            trap = std::make_shared<TwoFreq_trap>(trap_params);
        } else {
            log.error("Unrecognised trap type");
            throw std::runtime_error("Unrecognised trap type");
        }

        // Construct ion cloud
        IonCloud_ptr cloud = std::make_shared<IonCloud>
            (trap, cloud_params, sim_params, trap_params);

        // Construct integrator
        // RespaIntegrator integrator(trap, cloud, integration_params, sim_params);
        VerletIntegrator integrator(trap, cloud, integration_params, sim_params);

        // Construct integrator
        // CUDA_integrator integrator(trap, cloud, integration_params);

        // 3D histogram for image creation
        //ImageCollection ionImages((1.0)/(1e6 * microscope_params.pixels_to_distance *
                                         //trap_params.length_scale));

        // Cool down ion cloud
        log.info("Running cool down.");
        int nt_cool = integration_params.cool_steps;
        double dt = integration_params.time_step;
        DataWriter writer(",");

        // Write frame once every 2 RF cycles
        int write_every = (integration_params.steps_per_period);
        write_every = std::max(1, write_every);
        log.debug("Writing one frame every " + std::to_string(write_every));

//------------------------------------------------------------------------------
// Cooling
//------------------------------------------------------------------------------

        std::string stats_file = path + "energy.csv";
        auto meanListener = std::make_shared<MeanListener>(
                                integration_params, trap_params, stats_file);
        integrator.registerListener(meanListener);

        for (int t = 0; t < nt_cool; ++t) {
            integrator.evolve(dt);

            // Track progress
            int percent = static_cast<int>((t*100)/nt_cool);
            if ( (t*100/5)%nt_cool == 0 ) {
                printProgBar(percent);
            }
        }
        printProgBar(100);
        std::cout << '\n';

        integrator.deregisterListener(meanListener);

        // Evolution
        int nt = integration_params.hist_steps;

//------------------------------------------------------------------------------
// Histogram
//------------------------------------------------------------------------------
        log.info("Acquiring histogram data");

        auto ionStatsListener = std::make_shared<IonStatsListener>(
            integration_params, trap_params, cloud_params, path);
        integrator.registerListener(ionStatsListener);

        // estimate number of steps per RF cycle
        //int cycle_steps = 2.0 * integration_params.steps_per_period;
        //log.info("Will plot RF phase for final " + std::to_string(cycle_steps) + " steps.");
        // Open a file to store step number and RF factor
        //writer.writeComment(path + "RFphase.csv", "time step, phase factor");

        //IonHistogram_ptr ionHistogram = std::make_shared<IonHistogram>(0.5 * trap_params.energy_scale);

        // Start timer
        stopWatchTimer();
        KE = 0;
        double etot = 0;
        
        if (microscope_params.make_image) {
            auto imagesListener = std::make_shared<ImageHistogramListener>(
                integration_params, trap_params, microscope_params, path);
            integrator.registerListener(imagesListener);
        }

        for (int t = 0; t < nt; ++t) {

            integrator.evolve(dt);
            //if (microscope_params.make_image)
                //cloud->update_position_histogram(ionImages);

            //cloud->update_energy_histogram(ionHistogram);

            // Track progress
            int percent = static_cast<int>((t*100)/nt);
            if ((t*100/5)%nt == 0) {
                printProgBar(percent);
                std::cout << std::setw(4) << stopWatchTimer() << "s";
            }
            KE += cloud->kinetic_energy();
            etot += cloud->total_energy();

            // Output the trapping voltage scale factor during the final RF
            // cycle.
            //if (t >= nt-cycle_steps) {
                //std::list<double> line;
                //line.push_back(static_cast<double>(t)/integration_params.steps_per_period);
                //line.push_back(trap->get_phase());
                //writer.writeRow( path + "RFphase.csv", line);
            //}
        }
        KE /= nt;
        printProgBar(100);
        std::cout << std::endl;

        char buffer[256];
        snprintf(buffer, 256, "Total kinetic energy = %.4e J", KE * trap_params.energy_scale);
        log.info(std::string(buffer));
        snprintf(buffer, 256, "Total energy = %.4e J", etot * trap_params.energy_scale);
        log.info(std::string(buffer));
        //ionHistogram->writeFiles("ionEnergy");

        //cloud->saveStats(path, trap_params.length_scale, trap_params.time_scale);

        //if (microscope_params.make_image) {
            //ionImages.writeFiles(path, microscope_params);
        //}

        timer.stop();
        log.info("Wall time = " + 
                std::to_string(timer.get_wall_time()) + " s");
      log.info("CPU time  = " + 
              std::to_string(timer.get_cpu_time()) + " s");
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

