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
#include "include/imagecollection.h"
#include "include/integrator.h"
#include "include/logger.h"
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

    log.initialise(Logger::debug, path + "log.txt");
    log.log(Logger::info, "CCMD - Coulomb crystal molecular dynamics");
    log.log(Logger::info, "Version 2.0.0");

    // Parameter file paths
    std::string info_file = path + "trap.info";
    log.log(Logger::info, "Loading input file " + info_file);

    try {
        // Get simulation parameters from files
        TrapParams trapParams(info_file);
        CloudParams cloud_params(info_file);
        IntegrationParams integrationParams(info_file);
        MicroscopeParams microscope_params(info_file);
        SimParams sim_params(info_file);

        // Construct trap based on parameters
        std::shared_ptr<IonTrap> trap;
        if (trapParams.wave == trapParams.cosine) {
            trap = std::make_shared<CosineTrap>(trapParams);
        } else if (trapParams.wave == trapParams.digital) {
            trap = std::make_shared<PulsedTrap>(trapParams);
        } else if (trapParams.wave == trapParams.waveform) {
            trap = std::make_shared<WaveformTrap>(trapParams);
        } else if (trapParams.wave == trapParams.cosine_decay) {
            // have to do some unit conversions here for the decay params
            trapParams.tau *= PI;
            trapParams.deltaT = (integrationParams.cool_steps + integrationParams.hist_steps)*
                integrationParams.time_step-trapParams.deltaT*PI;
            trap = std::make_shared<CosineDecayTrap>(trapParams);
        } else if (trapParams.wave == trapParams.twofreq) {
            trap = std::make_shared<TwoFreq_trap>(trapParams);
        } else {
            log.log(Logger::error, "Unrecognised trap type");
            throw std::runtime_error("Unrecognised trap type");
        }

        // Construct ion cloud
        IonCloud_ptr cloud = std::make_shared<IonCloud>
            (trap, cloud_params, sim_params);

        // Construct integrator
        RespaIntegrator integrator(trap, cloud, integrationParams, sim_params);

        // Construct integrator
        // CUDA_integrator integrator(trap, cloud, integrationParams);

        // 3D histogram for image creation
        ImageCollection ionImages((1.0)/(1e6 * microscope_params.pixels_to_distance *
                                         trap->get_length_scale()));

        // Cool down ion cloud
        log.log(Logger::info, "Running cool down.");
        int nt_cool = integrationParams.cool_steps;
        double dt = integrationParams.time_step;
        DataWriter writer(",");
        writer.writeComment(path + "totalEnergy.csv", "t\tE_tot");

        // int write_every = std::floor(6.2831/(15*dt));
        // Write frame once every 2 RF cycles
        int write_every = (integrationParams.steps_per_period);
        write_every = std::max(1, write_every);
        log.log(Logger::debug, "Writing one frame every " +
                std::to_string(write_every));
        int frameNumber = 0;

        Stats<double> mean_energy;
        std::string stats_file = path + "energy.csv";
        int energy_row = 0;

//------------------------------------------------------------------------------
// Cooling
//------------------------------------------------------------------------------
        for (int t = 0; t < nt_cool; ++t) {
            //            cloud->collide();
            //            if (cloud->number_of_ions() ==0) {
            //                log.log(Logger::warn, "No ions remaining, stopping.");
            //                break;
            //            }

            integrator.evolve(dt);
            mean_energy.append(cloud->kinetic_energy());

            if (t%write_every == 0) {
                std::list<double> rowdata;
                rowdata.push_back(energy_row++);
                rowdata.push_back(mean_energy.average());
                rowdata.push_back(mean_energy.variance());
                writer.writeRow(stats_file, rowdata);
                mean_energy.reset();

                // char buffer[50];
                // std::sprintf(buffer, "%.4i", frameNumber++);
                // std::string framepath = buffer;
                // cloud->savePos(framepath, trap->get_length_scale(), trap->get_time_scale());
            }
            // Track progress
            int percent = static_cast<int>((t*100)/nt_cool);
            if ( (t*100/5)%nt_cool == 0 ) {
                printProgBar(percent);
            }
        }
        printProgBar(100);
        std::cout << '\n';

        // Evolution
        int nt = integrationParams.hist_steps;

//------------------------------------------------------------------------------
// Histogram
//------------------------------------------------------------------------------

        log.log(Logger::info, "Acquiring histogram data");

        // estimate number of steps per RF cycle
        int cycle_steps = 2.0 * integrationParams.steps_per_period;
        log.log(Logger::info, "Will plot RF phase for final " + std::to_string(cycle_steps) + " steps.");
        // Open a file to store step number and RF factor
        writer.writeComment(path + "RFphase.csv", "time step, phase factor");

        IonHistogram_ptr ionHistogram = std::make_shared<IonHistogram>(0.5);

        // Start timer
        stopWatchTimer();
        KE = 0;
        double etot = 0;
        int swap_count = 0;
        // if (swap_params.do_swap)
            // swap_count = std::floor(0.5*nt/swap_params.from.number);
        for (int t = 0; t < nt; ++t) {
//            cloud->collide();
//            if (cloud->number_of_ions() ==0) {
//                log.log(Logger::warn, "No ions remaining, stopping.");
//                break;
//            }

            integrator.evolve(dt);
            if (microscope_params.make_image)
                cloud->update_position_histogram(ionImages);

            cloud->updateStats();
            cloud->update_energy_histogram(ionHistogram);

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
            if (t >= nt-cycle_steps) {
                std::list<double> line;
                line.push_back(static_cast<double>(t)/integrationParams.steps_per_period);
                line.push_back(trap->get_phase());
                writer.writeRow( path + "RFphase.csv", line);
            }
        // if (swap_params.do_swap){
            // if (t%swap_count==0)
                // cloud->swap_first(swap_params.from, swap_params.to);
        // }

            // if (t%write_every==0) {
                // char buffer[50];
                // std::sprintf(buffer, "%.4i", frameNumber++);
                // std::string framepath = buffer;
                // cloud->savePos(framepath,
                        // trap->get_length_scale(), trap->get_time_scale());
            // }
        }
        KE /= nt;
        printProgBar(100);
        std::cout << std::endl;

        log.log(Logger::info, "total kinetic energy = " + std::to_string(KE));
        log.log(Logger::info, "total energy = " + std::to_string(etot));

        ionHistogram->writeFiles("ionEnergy");

        if (microscope_params.make_image) {
            ionImages.writeFiles(path, microscope_params);
        }
        cloud->saveStats(path, trap->get_length_scale(), trap->get_time_scale());

        timer.stop();
        log.log(Logger::info, "Wall time = " + 
                std::to_string(timer.get_wall_time()) + " s");
      log.log(Logger::info, "CPU time  = " + 
              std::to_string(timer.get_cpu_time()) + " s");
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

