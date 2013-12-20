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

//string Trap_params::default_info_file = "../../../config/trap_config.info";

// default trap parameters taken from file
string Trap_params::default_trap_file = "../../../config/trap_config.txt";

// path to file containing ion type definitions
std::string Cloud_params::ion_types_file = "../../../config/ion_types.txt";

// path to file containing ion numbers
std::string Cloud_params::ion_numbers_file= "../../../config/ion_numbers.txt";

// path to file containing integration parameters
std::string Integration_params::default_integrator_file = "../../../config/integrator.txt";


std::string console_divide_line = "--------------------------------------------------";

void print_ion_numbers(std::pair<const Ion_type*,int> ion_type_no );
void print_ion_type(const Ion_type& ion);

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
    } catch(boost::property_tree::ptree_error& e) {
        ostringstream error_msg;
        error_msg << e.what();
        throw runtime_error(error_msg.str());
    }
}

struct Compare_ion_names {
    // compare Ion_types class by alphabetic ordering on name field

    Compare_ion_names(const Ion_type& new_type) 
        : rhs(new_type) {}
    bool operator()(const Ion_type& lhs) const 
    {
        return lhs.name == rhs.name;
    }
    bool operator<(const Ion_type& lhs) const 
    {
        return lhs.name < rhs.name;
    }
private:
    Ion_type rhs;
};

struct Compare_ion_formula {
    // compare Ion_types class by alphabetic ordering on formula field

    Compare_ion_formula(const Ion_type& new_type) 
        : rhs(new_type) {}
    bool operator()(const Ion_type& lhs) const 
    {
        return lhs.formula == rhs.formula;
    }
private:
    Ion_type rhs;
};


void print_ion_type(const Ion_type& ion)
{
    // prints Ion_type fields to std::cout

    cout << ion.name << ' '
         << ion.formula << ' '
         << ion.mass << ' '
         << ion.charge << ' ' ;
    if (ion.is_laser_cooled) {
        cout << "Yes" << ' ' << ion.beta << ' ';
    } else {
        cout << "No" << ' ' << "#" << ' ';
    }
    if (ion.is_heated) {
        cout << "Yes" << ' ' << ion.recoil << ' ';
    } else {
        cout << "No" << ' ' << "#" << ' ';
    }
    cout << endl;
}


void print_ion_numbers(std::pair<const Ion_type*,int> ion_type_no ) {
    cout << '\t' << ion_type_no.second << ' ' << ion_type_no.first->formula << endl;
}

void Cloud_params::get_ion_numbers(const std::string& file_name) {
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
        
        // Append this to the list
        ionTypeList.push_back(ionType);
    }
}

Cloud_params::Cloud_params()
{
//    get_ion_types(ion_types_file);
    get_ion_numbers(ion_numbers_file);
}

Cloud_params::Cloud_params(const std::string& numbers_file)
{
//    get_ion_types(types_file);
    get_ion_numbers(numbers_file);
}

struct Compare_Ion_types {
    // case-insensitive compare for Ion_type names

    Compare_Ion_types(const std::string& name) : name_(name)
    {
        lowercase = tolower;
        transform(name_.begin(), name_.end(), name_.begin(), lowercase);
    }
    bool operator()(const Ion_type& type) const {
        // converts to lower case
        string type_name = type.name;
        transform(type_name.begin(), type_name.end(), type_name.begin(), lowercase);

        return type_name == name_;
    }
private:
    std::string name_;
    int (*lowercase)(int);      // pointer function to make lowercase
};

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

    
    cout << '\n' << console_divide_line << '\n';
    cout << '\n' << "Integrator parameters read from file: " << file_name << '\n';
    cout << "\tTime step: " << time_step << '\n'
         << "\tRESPA steps: " << respa_steps << '\n';
    cout << endl;
}
