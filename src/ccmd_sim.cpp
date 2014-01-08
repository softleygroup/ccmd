//
//  ccmd_sim.cpp
//

#include "ccmd_sim.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/exceptions.hpp>

using namespace std;

/**
 *  @file ccmd_sim.h
 *  @brief Load parameters from the file, check for correctness and store.
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
 *  # Example input file #
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
 *      integrator {
 *          timestep    0.1
 *          respasteps  5       ; Respa inner loop steps
 *          coolsteps   20000
 *          histsteps   20000
 *      }
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
 *              recoil      0.01
 *              direction   0.5
 *          }
 *      }
 *
 *  [info_doc]: http://www.boost.org/doc/libs/1_46_1/doc/html/boost_propertytree/parsers.html#boost_propertytree.parsers.info_parser
 *
 */

/**
 *  @class Trap_params
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
 *  \c vrf      | Peak RF voltage, i.e. half the amplitude
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
 *  see the description of ccmd_sim.h for a full input file, possible parameter
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
 * .
 */

/**
 *  @brief Load trap parameters from given file.
 *
 *  @param file_name file containing trap parameters.
 */
Trap_params::Trap_params(const std::string& file_name)
{
    
    using boost::property_tree::iptree;
    iptree pt;
    read_info(file_name, pt);
    try {
        v_rf    = pt.get<double>("trap.vrf");
        v_end   = pt.get<double>("trap.vend");
        eta     = pt.get<double>("trap.eta");
        r0      = pt.get<double>("trap.r0");
        z0      = pt.get<double>("trap.z0");
        freq    = pt.get<double>("trap.freq");
        
        std::string typeString = pt.get<std::string>("trap.type.name");
        ostringstream error_msg;
        if (typeString == "cosine") {
            cout << "Making a cosine trap\n";
            wave = cosine;
        } else if (typeString == "digital") {
            cout << "Making a digital trap\n";
            wave = digital;
            tau = pt.get<double>("trap.type.tau");
            if (tau<0.0) {
                cout << "Warning: Tau="<< tau << " out of range. Limiting to 0.0";
                tau = 0.0;
            } else if (tau>1.0) {
                cout << "Warning: Tau="<< tau << " out of range. Limiting to 1.0";
                tau = 1.0;
            }
        } else {
            cout << "Unrecognised trap type " << typeString << "\n";
            throw runtime_error("unrecognised trap");
        }
    } catch(boost::property_tree::ptree_error& e) {
        ostringstream error_msg;
        error_msg << e.what();
        throw runtime_error(error_msg.str());
    }
}


/**
 *  @class Cloud_params
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
 *  formula given in the \c ionnumbers branch. The physical properties of each ion
 *  in use is then loaded.
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
 *  \c direction   |   Weighting of laser cooling from left and right (0-1) (**optional**)
 *  \c beta        |   Laser cooling parameter  (**optional**)
 *  \c heating     |   Is the ion heated by photon recoil? (true/false) (**optional**)
 *  \c recoil      |   Heating recoil factor (**optional**)
 *
 *
 *  # Example input #
 *  see the description of ccmd_sim.h for a full input file, the sections 
 *  of the input file relevant to Cloud_params is:
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
 *  properties in an Ion_type object and append it to the ionType list.
 *
 *  @param file_name    File containing ion numbers and parameters.
 */
Cloud_params::Cloud_params(const std::string& file_name)
{
    using boost::property_tree::iptree;
    iptree pt;
    read_info(file_name, pt);
    iptree numbers_node = pt.get_child("ionnumbers");
    
    // Look through each node. The label should be the ion formula, and the
    // data should be an integer.
    for (iptree::iterator it=numbers_node.begin(); it!=numbers_node.end(); ++it) {
        // Now, read the parameters for this ion from the iontype node
        iptree ionTypeTree = pt.get_child("iontype."+it->first);
        Ion_type ionType;
        ionType.number = it->second.get_value<int>();
        ionType.formula = it->first;
        ionType.name = ionTypeTree.get<std::string>("name");
        ionType.mass = ionTypeTree.get<double>("mass");
        ionType.charge = ionTypeTree.get<int>("charge");
        ionType.is_laser_cooled = ionTypeTree.get<bool>("lasercooled", false);
        ionType.is_heated = ionTypeTree.get<bool>("heated", false);
        ionType.beta = ionTypeTree.get<double>("beta", 0.0);
        ionType.recoil = ionTypeTree.get<bool>("recoil", 0.0);
        ionType.direction = ionTypeTree.get<bool>("direction", 0.5);
        if (ionType.direction<0.0) {
            cout << "Warning: direction="<< ionType.direction;
            cout << " out of range. Limiting to 0.0";
            ionType.direction = 0.0;
        } else if (ionType.direction>1.0) {
            cout << "Warning: direction="<< ionType.direction;
            cout << " out of range. Limiting to 1.0";
            ionType.direction = 1.0;
        }

        
        // Append this to the list
        ionTypeList.push_back(ionType);
        
        cout << "Loaded " << ionType.number;
        if (ionType.is_laser_cooled) cout << " laser cooled";
        cout << ionType.name << " ions" << endl;
    }
}


/**
 *  @class Integration_params
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
 *  Parameter   | Description
 *  ------------|---------------------------------------------------------------
 *  timestep    | Stepsize for integrator loop in units of 2/\Omega = 1/(\pi f)
 *  respasteps  | Number of fast trap force steps to take between each slow 
 *              | Coulomb force calculation. \c respasteps = 1 is equivalent to
 *              | velocity Verlet algorithm.
 *  coolsteps   | Number of steps to take letting the ions cool (no data collected)
 *  histsteps   | Number of steps to take while collecting data.
 *
 *  # Example input #
 *  see the description of ccmd_sim.h for a full input file, the sections
 *  of the input file relevant to Integration_params is:
 *
 *      integrator {
 *          timestep    0.1
 *          respasteps  5       ; Respa inner loop steps
 *          coolsteps   20000
 *          histsteps   20000
 *      }
 *
 */

/**
 *  @brief Load integration parameters from given file.
 *
 *  @param file_name file containing integration parameters.
 */
Integration_params::Integration_params(const std::string& file_name)
{
    using boost::property_tree::iptree;
    iptree pt;
    read_info(file_name, pt);
    try {
        time_step   = pt.get<double>("integrator.timestep");
        respa_steps = pt.get<double>("integrator.respasteps");
        cool_steps   = pt.get<int>("integrator.coolsteps");
        hist_steps   = pt.get<int>("integrator.histsteps");
    } catch(const boost::property_tree::ptree_error &e) {
        ostringstream error_msg;
        cout << e.what() << endl;
        throw runtime_error("Error reading integration params.");
    }

    
    cout << '\n' << "Integrator parameters:\n";
    cout << "\tTime step: " << time_step << endl;
    cout << "\tRESPA steps: " << respa_steps << endl;
    cout << "\tWill take " << cool_steps << "to allow ions to equilibrate," << endl;
    cout << "\t then " << hist_steps << "while collecting data" << endl;
}
