/** 
 * @file ccmdsim.cpp
 * @brief Class definitions for all parameter classes.
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 *
 *  All the parameters are stored in a text input file following the Boost info
 *  file format (see documentation at the [boost website][info_doc]), which is
 *  parsed as a property tree. The base branches are \c trap containing ion trap
 *  parameters, \c integrator containing integrator parameters, \c ionnumber
 *  containing the number of each type of ion in the simulation and \c iontype
 *  which defines the physical properties of the ions.
 *
 *  Note: the names in the input file are not case sensitive.
 *
 *  ## Example input file
 *
 *     trap {
 *         vrf     150.75
 *         vend    1.0
 *         eta     0.244
 *         r0      3.5e-3
 *         z0      2.75e-3
 *         freq    3.85e6
 *         type    {
 *             name cosine
 *         }
 *     }
 *     integrator {
 *         stepsPerPeriod 20
 *         respasteps  50       ; Respa inner loop steps
 *         coolperiods 2000
 *         histperiods   200
 *     }
 *     image {
 *         makeimage   true
 *         scale       1; 2.5     ; Image scaling in pixels per micron
 *         blur        5.0     ; Blur radius in microns
 *         dof         50.0    ; Depth of field in microns
 *         nz          640     ; Number of pixels in z axis
 *         nx          480     ; Number of pixels in x axis
 *     }
 *     simulation {
 *         threads     0
 *         seed        -1
 *     }
 *     ionnumbers {
 *         Ca      50
 *          Xe      0
 *     }
 *     iontype {
 *         CaF {
 *             name        CalciumFluoride
 *             mass        59.0
 *             charge      1
 *         }
 *         Ca {
 *             name        Calcium
 *             mass        40.0
 *             charge      1
 *             lasercooled true
 *             beta        0.8
 *             heated      true
 *             recoil      0.00001
 *             direction   0.5
 *         }
 *         ND3 {
 *             name        Ammonia-d3
 *             mass    	20.0
 *             charge  	1
 *         }
 *         Xe {
 *             Name        Xenon
 *             mass        130.0
 *             charge  	1	
 *         }
 *     }
 *
 *  [info_doc]: http://www.boost.org/doc/libs/1_46_1/doc/html/boost_propertytree/parsers.html#boost_propertytree.parsers.info_parser
 *
 *
 */

#include "include/ccmdsim.h"

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <sstream>
#include <string>
#include <cmath>

#include "include/logger.h"

/**
 *  @class TrapParams
 *  @brief Ion trap physical parameters
 *
 *  All the parameters are stored in a text input file following the Boost info
 *  file format (see documentation at the [boost website][info_doc]), which is
 *  parsed as a property tree.
 *  [info_doc]: http://www.boost.org/doc/libs/1_46_1/doc/html/boost_propertytree/parsers.html#boost_propertytree.parsers.info_parser
 *
 *  Parameters such as voltage and frequency are always defined for any trap.
 *  the trap type is defined in the \c type sub-branch by the \c name parameter.
 *  Valid trap names are \c cosine for a regular cosine trap, which takes no
 *  additional parameters; or \c digital for a digital, pulsed waveform, trap
 *  which takes the additional parameter of \c tau specifying the duty cycle.
 *
 *  # Avaliable parameters #
 *
 *  Parameter   | Description
 *  ------------|------------------------------------------------------------
 *  \c vrf      | Peak-to-peak RF voltage (29/9/15)
 *  \c vend     | End-cap voltage
 *  \c eta      | Geometric non-ideality factor
 *  \c r0       | Distance between quadrupole rod centres
 *  \c z0       | Distance between endcaps
 *  \c freq     | RF frequency
 *  \c type     | (_specifies trap type sub-branch_)
 *  \c name     | Trap type name (cosine / digital)
 *  \c tau      | (digital only) waveform duty cycle
 *
 *  # Example input #
 *  see the description of ccmdsim.h for a full input file, possible parameter
 *  values for a cosine or digital trap are as follows:
 *
 *  ## Cosine trap ##
 *
 *      trap {
 *          vrf     300.0
 *          vend    2.0
 *          eta     0.244
 *          r0      3.5e-3
 *          z0      2.75e-3
 *          freq    3.85e6
 *          type    {
 *              name    cosine
 *          }
 *      }
 *
 *  ## Digital trap ##
 *
 *      trap {
 *          vrf     300.0
 *          vend    2.0
 *          eta     0.244
 *          r0      3.5e-3
 *          z0      2.75e-3
 *          freq    3.85e6
 *          type    {
 *              name    digital
 *              tau     0.1
 *          }
 *      }
 * 
 */

/**
 *  @brief Load trap parameters from given file.
 *
 *  @param file_name file containing trap parameters.
 */
TrapParams::TrapParams(const std::string& file_name) {
    Logger& log = Logger::getInstance();
    using boost::property_tree::iptree;
    iptree pt;
    read_info(file_name, pt);
    try {
        log.debug("Loading trap parameters.");
        v_rf    = pt.get<double>("trap.vrf");
        v_end   = pt.get<double>("trap.vend");
        eta     = pt.get<double>("trap.eta");
        r0      = pt.get<double>("trap.r0");
        z0      = pt.get<double>("trap.z0");
        freq    = pt.get<double>("trap.freq");

        log.info("Trap parameters:");
        log.info("\tVrf: " + std::to_string(v_rf));
        log.info("\tEnd cap: " + std::to_string(v_end));
        log.info("\teta: " + std::to_string(eta));
        log.info("\tr0: " + std::to_string(r0));
        log.info("\tz0: " + std::to_string(z0));
        log.info("\tfreq: " + std::to_string(freq));

        // Calculate conversion factors from simulation units to S.I. units.
        /** @brief Over-precise value of pi. */
        double pi = 3.141592653589793238462643383279502884;
        /** @brief Over-precise value of epsilon_0. */
        double epsilon_0 = 8.85418781762e-12;
        /** @brief Over-precise value of electron charge. */
        double electron_charge = 1.60217733e-19;
        /** @brief 1 AMU in kg. */
        double u_mass = 1.6605402e-27;
        double omega = 2*pi*freq;
        length_scale = electron_charge*electron_charge /(pi*epsilon_0*u_mass*omega*omega);
        length_scale = std::pow(length_scale, 1.0/3.0);
        time_scale = 1/(pi * freq);
        energy_scale = u_mass * (length_scale * length_scale) / (time_scale * time_scale);
        char buffer[256];
        snprintf(buffer, 256, "Energy scale factor %.4e J per unit", energy_scale);
        log.debug(std::string(buffer));

        // Read trap-specific information.
        std::string typeString = pt.get<std::string>("trap.type.name");
        if (typeString == "cosine") {
            log.info("Making a cosine trap.");
            wave = cosine;
        } else if (typeString == "digital") {
            log.info("Making a digital trap.");
            wave = digital;
            tau = pt.get<double>("trap.type.tau");
            if (tau < 0.0) {
                std::stringstream ss;
                ss << "Warning: Tau=" << tau;
                ss << " out of range. Limiting to 0.0";
                log.warn(ss.str());
                tau = 0.0;
            } else if (tau > 1.0) {
                std::stringstream ss;
                ss << "Warning: Tau=" << tau;
                ss << " out of range. Limiting to 1.0";
                log.warn(ss.str());
                tau = 1.0;
            }
        } else if (typeString == "waveform") {
            log.info("Making a waveform trap.");
            wave = waveform;
            // The waveform file is in the same directory as the trap parameters
            size_t found;
            found = file_name.find_last_of("/\\");
            waveformFile = file_name.substr(0, found) + "/waveform.dat";
        } else if (typeString == "cosine_decay") {
            log.info("Making a decaying cosine trap.");
            wave = cosine_decay;
            tau = pt.get<double>("trap.type.tau");
            deltaT = pt.get<double>("trap.type.deltaT");
        } else if (typeString == "twofreq") {
            log.info("Making a two-frequency trap.");
            wave = twofreq;
            freq_mult = pt.get<double>("trap.type.mult");
            log.info("\tFreq multiple:" + std::to_string(freq_mult));
        } else {
            std::stringstream ss;
            ss << "Unrecognised trap type " << typeString;
            log.error(ss.str());
            throw std::runtime_error("unrecognised trap");
        }
    } catch(boost::property_tree::ptree_error& e) {
        log.error(e.what());
        throw std::runtime_error(e.what());
    }
}


/**
 *  @class CloudParams
 *  @brief Store the number of each type of ion, and their physical properties.
 *
 *  All the parameters are stored in a text input file following the Boost info
 *  file format (see documentation at the [boost website][info_doc]), which is
 *  parsed as a property tree.
 *  [info_doc]: http://www.boost.org/doc/libs/1_46_1/doc/html/boost_propertytree/parsers.html#boost_propertytree.parsers.info_parser
 *
 *  Ions are described as follows: a branch named \c ionnumbers lists one ion
 *  formula followed by the number of this ion in the simulation. We then look
 *  within the \c iontype branch for a sub-branch with the same name as each
 *  formula given in the \c ionnumbers branch. The physical properties of each
 *  ion in use is then loaded.
 *
 *  The definition is arranged in two parts with the number defined separately
 *  to allow the definition of ion physical properties to be easily copied and
 *  pasted between input files. There is no penalty for having more ions defined
 *  than used, the function only looks for definitions matching those listed in
 *  the \c ionnumbers branch.
 *
 *  # Avaliable parameters #
 *  ## iontype ##
 *
 *  The \c lasercooled and \c heating parameters are optional, and are taken
 *  as \c false if not specified (e.g. for CaF in the example below.) the other
 *  parameters marked optional require heating or cooling and are ignored if
 *  these are false.
 *
 *  Parameter      | Description
 *  ---------------|------------------------------------------------------------
 *  \c name        |   Full name of the ion, spaces will be stripped
 *  \c mass        |   Molecular mass in AMU
 *  \c charge      |   Charge in units of fundamental charge (+1, +2, ...)
 *  \c lasercooled |   Is this ion lasercooled (true/false) (**optional**)
 *  \c direction   |   Weighting of laser cooling along -+z (0-1) (**optional**)
 *  \c beta        |   Laser cooling parameter  (**optional**)
 *  \c heating     |   Is the ion heated by photon recoil? (true/false) (**optional**)
 *  \c seed        |   Seed for random number generator, use -1 to generate
 *                 |   automatically, or an integer to remove all randomness.
 *  \c recoil      |   Heating recoil factor (**optional**)
 *
 *
 *  # Example input #
 *  see the description of ccmdsim.h for a full input file, the sections
 *  of the input file relevant to CloudParams is:
 *
 *      ionnumbers {
 *          Ca      100
 *          CaF     50
 *      }
 *      iontype {
 *          CaF {
 *              name        CalciumFluoride
 *              mass        59.0
 *              charge      1
 *          }
 *          Ca {
 *              name        Calcium
 *              mass        40.0
 *              charge      1
 *              lasercooled true
 *              beta        0.6
 *              heating     true
 *              seed        1
 *              recoil      0.01
 *              direction   0.5
 *          }
 *       }
 *
 * .
 */

/**
 *  @brief Load cloud parameters from the given file.
 *
 *  Read each line in the branch \c ionnumbers. For each ion, look up the
 *  parameters in \c iontype, then store the number of ions and its physical
 *  properties in an IonType object and append it to the ionType list.
 *
 *  @param file_name    File containing ion numbers and parameters.
 */
CloudParams::CloudParams(const std::string& file_name) {
    using boost::property_tree::iptree;
    iptree pt;
    Logger& log = Logger::getInstance();
    read_info(file_name, pt);
    iptree numbers_node = pt.get_child("ionnumbers");

    // Look through each node. The label should be the ion formula, and the
    // data should be an integer.
    for (auto& it : numbers_node) {
        // Now, read the parameters for this ion from the iontype node
        iptree ionTypeTree = pt.get_child("iontype." + it.first);
        IonType ionType;
        ionType.number = it.second.get_value<int>();
        ionType.formula = it.first;
        ionType.name = ionTypeTree.get<std::string>("name");
        ionType.mass = ionTypeTree.get<double>("mass");
        ionType.charge = ionTypeTree.get<int>("charge");
        ionType.is_laser_cooled = ionTypeTree.get<bool>("lasercooled", false);
        ionType.recoil = ionTypeTree.get<double>("recoil", 0.0);
        ionType.direction = ionTypeTree.get<double>("direction", 0.5);
		ionType.A21 = ionTypeTree.get<double>("A21",0);
        if (ionType.direction < 0.0) {
            std::stringstream ss;
            ss << "Warning: direction=" << ionType.direction;
            ss << " out of range. Limiting to 0.0";
            log.warn(ss.str());
            ionType.direction = 0.0;
        } else if (ionType.direction > 1.0) {
            std::stringstream ss;
            ss << "Warning: direction=" << ionType.direction;
            ss << " out of range. Limiting to 1.0";
            log.warn(ss.str());
            ionType.direction = 1.0;
        }


        // Append this to the list
        ion_type_list.push_back(std::move(ionType));

        log.info(ionType.name + " ions:");
        log.info("\tNumber: " + std::to_string(ionType.number));
        log.info("\tMass: " + std::to_string(ionType.mass));
        log.info("\tCharge: " + std::to_string(ionType.charge));
        if (ionType.is_laser_cooled) {
            log.info("\tLaser Cooled.");
            log.info(
                    "\tbeta: " + std::to_string(ionType.beta));
            log.info(
                    "\trecoil: " + std::to_string(ionType.recoil));
            log.info(
                    "\tdirection: " + std::to_string(ionType.direction));
        }
    }
}


/**
 * @class SwapParams
 * @brief Store ion type to swap from and to during a simulation.
 */
/*
SwapParams::SwapParams(const std::string& file_name, const CloudParams& cp) {
    Logger& log = Logger::getInstance();
    using boost::property_tree::iptree;
    iptree pt;
    read_info(file_name, pt);
    boost::optional<iptree&> swap = pt.get_child_optional("ionswapper");
    if (swap) {
            log.debug("Loading swap parameters.");
            std::string from_formula = swap.get().get<std::string>("from");
            std::string to_formula = swap.get().get<std::string>("to");
            p = swap.get().get<double>("prob");

            for (auto& type : cp.ion_type_list) {
                if (type.formula == from_formula) {
                    from = type;
                }
                if (type.formula == to_formula) {
                    to = type;
                }
            }
            log.info("Will swap from " + from.formula + " to "
                    + to.formula);
            do_swap=true;
    } else {
        do_swap=false;
    }
}
*/

/**
 *  @class IntegrationParams
 *  @brief Load and store parameters relating to the integrator
 *
 *  All the parameters are stored in a text input file following the Boost info
 *  file format (see documentation at the [boost website][info_doc]), which is
 *  parsed as a property tree.
 *  [info_doc]: http://www.boost.org/doc/libs/1_46_1/doc/html/boost_propertytree/parsers.html#boost_propertytree.parsers.info_parser
 *
 *  # Avaliable parameters #
 *  ## integrator ##
 *
 *  Parameter        | Description
 *  -----------------|---------------------------------------------------------------
 * \c stepsPerPeriod | Number of integrator steps to take in one RF period.
 * \c respasteps     | Number of fast trap force steps to take between each slow
 *                   | Coulomb force calculation. \c respasteps = 1 is equivalent to
 *                   | velocity Verlet algorithm.
 * \c coolperiods    | Number of RF periods for equilibration (no data collected)
 * \c histperiods    | Number of RF periods to propagate while collecting data.
 *
 *  # Example input #
 *  see the description of ccmdsim.h for a full input file, the sections
 *  of the input file relevant to IntegrationParams is:
 *
 *      integrator {
 *          stepsPerPeriod 20
 *          respasteps  50       ; Respa inner loop steps
 *          coolperiods 2000
 *          histperiods   200
 *      }
 *
 */

/**
 *  @brief Load integration parameters from given file.
 *
 *  @param file_name file containing integration parameters.
 */
IntegrationParams::IntegrationParams(const std::string& file_name) {
    double coolperiods;
    double histperiods;

    using boost::property_tree::iptree;
    iptree pt;
    Logger& log = Logger::getInstance();
    read_info(file_name, pt);
    try {
        steps_per_period= pt.get<int>("integrator.stepsPerPeriod");
        respa_steps = pt.get<int>("integrator.respasteps");
        coolperiods   = pt.get<double>("integrator.coolperiods");
        histperiods   = pt.get<double>("integrator.histperiods");
    } catch(const boost::property_tree::ptree_error &e) {
        log.error("Error reading integration params.");
        log.error(e.what());
        throw std::runtime_error("Error reading integration params.");
    }

    time_step = 3.1415926535897932/steps_per_period;
    cool_steps = static_cast<int>(coolperiods*steps_per_period);
    hist_steps = static_cast<int>(histperiods*steps_per_period);

    log.info("Integrator parameters:");
    log.info("\tTime step: " + std::to_string(time_step));
    log.info("\tRESPA steps: " + std::to_string(respa_steps));
    log.info("\tWill take " + std::to_string(cool_steps) +
            " steps to allow ions to equilibrate,");
    log.info("\t then " + std::to_string(hist_steps) +
            " steps while collecting data");
}


/**
 *  @class MicroscopeParams
 *  @brief Stores microscope model parameters
 *
 *  see: A. D. Gingell, D.Phil thesis, University of Oxford
 *  Chapter 3
 *
 *
 * Parameter     | Description
 * --------------|---------------------------------------------------------------
 *  \c makeimage | Boolean. Image will be generated if  \c true or present.
 *  \c scale     | Image scaling in pixels per micron
 *  \c blur      | Blur radius in microns
 *  \c dof       | Depth of field in microns
 *  \c nz        | Number of pixels in z axis
 *  \c nx        | Number of pixels in x axis
 */
MicroscopeParams::MicroscopeParams(const std::string& file_name) {
    using boost::property_tree::iptree;
    iptree pt;
    read_info(file_name, pt);

    try {
        make_image = pt.get<bool>("image.makeimage");
        pixels_to_distance   = pt.get<double>("image.scale");
        w0   = pt.get<double>("image.blur") * pixels_to_distance;
        z0   = pt.get<double>("image.dof");
        nz = pt.get<int>("image.nz");
        nx = pt.get<int>("image.nx");
    } catch(const boost::property_tree::ptree_error &e) {
        throw std::runtime_error("Error reading microscope params.");
    }
}

/**
 *  @class SimParams
 *  @brief Store parameters related to overall simulation
 *
 *  These parameters control how the simulation runs and have minimal influence
 *  on the output. All are optional
 *
 * Parameter     | Description
 * --------------|---------------------------------------------------------------
 *  \c threads   | Number of threads to use in Coulomb force calculation.
 *               | The total number of running threads will be this number plus one.
 *               | Zero performs all calculations in a single thread.
 *  \c seed      | Seed for random number generator. Set to -1 to pick seed from
 *               | system clock.
 */
SimParams::SimParams(const std::string& file_name) {
    using boost::property_tree::iptree;
    iptree pt;
    read_info(file_name, pt);

    boost::optional<iptree&> params = pt.get_child_optional("simulation");
    if (params) {
        coulomb_threads = params.get().get<int>("threads", 0);
        random_seed = params.get().get<int>("seed", -1);
    } else {
        coulomb_threads = 0;
        random_seed = -1;
    }

    Logger& log = Logger::getInstance();
    log.info("Coulomb Force using " + std::to_string(coulomb_threads)
            + " threads.");
    log.info("Random seed " + std::to_string(random_seed));
}

/**
 *  @class LaserParams
 *  @brief Store parameters related to the Laser
 *
 *  These parameters determine the degree of interaction with the laser
 *  All parameters input as scientific format (i.e. X.YeZ)
 *
 * Parameter     | Description
 * --------------|---------------------------------------------------------------
 *  \c wavelength| Wavelength of laser
 *               | 
 *  \c delta     | Detuning of laser from resonance
 *               | 
 *  \c IdIsat    | Intensity of Laser divided by saturation intensity
 */
LaserParams::LaserParams(const std::string& file_name) {
    using boost::property_tree::iptree;
    iptree pt;
    Logger& log = Logger::getInstance();
    read_info(file_name, pt);
	
    try {
        wavelength= pt.get<float>("laser.wavelength");
        delta = pt.get<float>("laser.delta");
        IdIsat   = pt.get<float>("laser.IdIsat");
    } catch(const boost::property_tree::ptree_error &e) {
        log.error("Error reading Laser params.");
        log.error(e.what());
        throw std::runtime_error("Error reading Laser params.");
    }
	
	log.info("Laser parameters:");
    log.info("\tWavelength: " + std::to_string(wavelength));
    log.info("\tdelta: " + std::to_string(delta));
    log.info("\tI/Isat: " + std::to_string(IdIsat));
}
