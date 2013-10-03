//
//  ccmd_sim.cpp
//

#include "ccmd_sim.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

// default trap parameters taken from file
string Trap_params::default_trap_file = "../../../config/trap_config.txt";

// path to file containing ion type definitions
std::string Cloud_params::ion_types_file = "../../../config/ion_types.txt";

// path to file containing ion numbers
std::string Cloud_params::ion_numbers_file= "../../../config/ion_numbers.txt";

// path to file containing integration parameters
std::string Integration_params::default_integrator_file = "../../../config/integrator.txt";


std::string console_divide_line = "--------------------------------------------------";

bool is_yes_or_no(const std::string& test_string);
void print_ion_numbers(std::pair<const Ion_type*,int> ion_type_no );
void print_ion_type(const Ion_type& ion);


template <class T> 
T read_parameter(istream& iss)
{
    // this function template tries to read in a parameter of class T 
    // and throws a runtime_error if the input stream fails
    
    // reads numeric value
    T parameter_read;
    iss >> parameter_read;
    if ( !iss ) {
        ostringstream error_msg;
        error_msg << "Problem reading parameter";
        throw runtime_error( error_msg.str() );
    }
    
    // ignores rest of the line
    string line;
    getline(iss, line);
    
    return parameter_read;
}


Trap_params::Trap_params(const std::string& file_name)
{
    // constructor throws runtime_error if problem opening
    // file or reading parameters
    
    // tries to load trap parameters from file
    ifstream ifs( file_name.c_str() );
    if ( !ifs ) {
        ifs.close();
        ostringstream error_msg;
        error_msg   << "Trap parameter file " 
                    << "\"" << file_name << "\"" << " not found"; 
        throw runtime_error( error_msg.str() );
    }
        
    wave = Cosine;      // Cosine trap only for the moment
    
    // reads in parameters from an ordered text file with each
    // line written in the form: "value other_characters"
    // and ignoring the text
    try {
        v_rf    = read_parameter<double>(ifs);
        v_end   = read_parameter<double>(ifs);
        eta     = read_parameter<double>(ifs);
        r0      = read_parameter<double>(ifs);
        z0      = read_parameter<double>(ifs);
        freq    = read_parameter<double>(ifs);
    } catch (runtime_error& e) {
        ostringstream error_msg;
        error_msg << "Error reading parameters from file "
                  << "\"" << file_name << "\"";
        throw runtime_error( error_msg.str() );
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


bool is_yes_or_no(const std::string& test_string)
{
    // checks if string is "yes" or "no", independent of case
    // throws exception if neither of these
    
    string s(test_string); 
    
    // converts string to lower case, pointer function diambiguates: 
    //      int std::toupper(int);                   // from <cctype>
    //  from
    //      template <class chart>
    //      chart std::toupper(charT, const locale&); // from <locale>
    //
    int (*pf)(int)=tolower;
    transform(s.begin(), s.end(), s.begin(), pf);
    
    bool tof = s == "yes";
    if (tof) {
        return tof;
    } else if (s == "no") {
        return tof;
    } else {
        throw runtime_error(test_string);
    }
}

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
    cout << ion.color << ' ';
    if (ion.visible) {
        cout << "Yes" << ' ';
    } else {
        cout << "No" << ' ';
    }
    cout << endl;
}

void Cloud_params::get_ion_types(const std::string& file_name) {
    // tries to load trap parameters from file
    ifstream ifs( file_name.c_str() );
    if ( !ifs ) {
        ifs.close();
        ostringstream error_msg;
        error_msg   << "Ion types definition file " 
        << "\"" << file_name << "\"" << " not found"; 
        throw runtime_error( error_msg.str() );
    }
    
    // reads in ion type definitions from file line by line,
    // with each line in the format:
    // 
    // Name Formula Mass Charge Laser-cooled? Beta ...
    //  Heating? Recoil Colour Visible?
    //
    string line; 
    int lines_read = 0;
    // Get first line
    getline(ifs,line);
    
    while ( ifs ) {
        Ion_type new_type;
        
        string laser_cooled_str;
        string heating_str;
        string visible_str;
        
        istringstream ions_iss(line);
        
        ions_iss >> new_type.name
                >> new_type.formula
                >> new_type.mass
                >> new_type.charge
                >> laser_cooled_str
                >> new_type.beta
                >> heating_str
                >> new_type.recoil
                >> new_type.direction
                >> new_type.color
                >> visible_str;
        
        ++lines_read;
        
        // throws runtime_error if problem reading
        if ( !ions_iss) {
            ostringstream error_msg;
            error_msg << "Error reading parameters from file "
            << "\"" << file_name << "\"" << endl
            << "\t first error on line: " << lines_read;
            throw runtime_error( error_msg.str() );
        }
        
        // converts color to lower case
        int (*pf)(int)=tolower;
        transform(new_type.color.begin(), new_type.color.end(), new_type.color.begin(), pf);
        
        // converts "Yes" or "No" parameters into bool
        // throws exception if this is not possible
        try {
            new_type.is_laser_cooled = is_yes_or_no(laser_cooled_str);
            new_type.is_heated = is_yes_or_no(heating_str);
            new_type.visible = is_yes_or_no(visible_str);
        } catch (std::exception& e) {
            ostringstream error_msg;
            error_msg << "Error reading \"Yes\" or \"No\" in file: "
            << "\"" << file_name << "\"" << endl
            << "\t first error on line: " << lines_read << endl 
            << "\t this string read instead: " << e.what();
            throw runtime_error( error_msg.str() );
        }
        
        // sets redundant parameters to zero
        if (!new_type.is_laser_cooled) new_type.beta = 0.0;
        if (!new_type.is_heated) new_type.recoil = 0.0;
        
        // throws runtime_error if ion names or formula are duplicated
        typedef std::vector<Ion_type>::const_iterator type_itr;
        Compare_ion_names compare_by_names(new_type);
        
        type_itr check_names = find_if(ion_types.begin(), 
                                       ion_types.end(), 
                                       compare_by_names );
        
        Compare_ion_formula compare_by_formula(new_type);
        type_itr check_formula = find_if(ion_types.begin(), 
                                         ion_types.end(), 
                                         compare_by_formula );
        
        // adds new ion type if no duplicates found
        if ( check_names == ion_types.end() &&
             check_formula == ion_types.end()) {
            ion_types.push_back( new_type );
        } else {
            ostringstream error_msg;
            error_msg << "Duplicated ion names in file "
            << "\"" << file_name << "\"" << endl
            << "\t name " << "\"" << new_type.name << "\" "
            << "duplicated on line " << lines_read;     
            throw runtime_error( error_msg.str() );
        }
        
        // tries to get next line
        getline(ifs,line);
    }
    
    // prints ion types to std::cout
    cout << '\n' << console_divide_line << '\n';
    cout << '\n' << "Ion types read from file: " << file_name << "\n\n";
    cout << "Name, Formula, Mass, Charge, Laser-cooled?, Beta, Heating?, Recoil, Colour, Visible?" << "\n\n";
    for_each(ion_types.begin(), ion_types.end(), print_ion_type);
    cout << endl;

}

void print_ion_numbers(std::pair<const Ion_type*,int> ion_type_no ) {
    cout << '\t' << ion_type_no.second << ' ' << ion_type_no.first->formula << endl;
}

void Cloud_params::get_ion_numbers(const std::string& file_name) {
    
    ifstream ifs( file_name.c_str() );
    if ( !ifs ) {
        ifs.close();
        ostringstream error_msg;
        error_msg   << "Ion numbers file " 
        << "\"" << file_name << "\"" << " not found"; 
        throw runtime_error( error_msg.str() );
    }
    
    string line;
    int lines_read = 0;
    // gets first line
    getline(ifs,line);
    
    while ( ifs ) {
        istringstream ion_stream(line);
        int number;
        string name;
        ion_stream >> number >> name;
        ++lines_read;
        
        if ( ion_stream ) {
            
            // dummy ion type to allow searching by name and formula
            Ion_type temp_type;
            temp_type.name = name;
            temp_type.formula = name;
            
            Compare_ion_names compare_by_names(temp_type);
            typedef std::vector<Ion_type>::iterator type_itr;
            type_itr check_names = find_if(ion_types.begin(), 
                                           ion_types.end(), 
                                           compare_by_names );
                        
            // if name found then update map of ion types and numbers
            if (check_names != ion_types.end()) { 
                Ion_type* ion_type_ptr = &(*check_names);
                ion_numbers[ ion_type_ptr ] = number;
                break;
            }
            
            Compare_ion_formula compare_by_formula(temp_type);
            type_itr check_formula = find_if(ion_types.begin(), 
                                             ion_types.end(), 
                                             compare_by_formula );
            
            // if found formula then update map of ion types and numbers
            if (check_formula != ion_types.end()) {  
                Ion_type* ion_type_ptr = &(*check_formula);
                ion_numbers[ ion_type_ptr ] = number;
            }
        }
        
        // tries to get next line
        getline(ifs,line);
    }
    // writes out ion formulas and numbers to std::cout
    cout << '\n' << console_divide_line << '\n';
    cout << '\n' << "Ion numbers read from file: " << file_name << '\n';
    for_each(ion_numbers.begin(), ion_numbers.end(), print_ion_numbers);
    cout << endl;
}

Cloud_params::Cloud_params()
{
    get_ion_types(ion_types_file);
    get_ion_numbers(ion_numbers_file);
}

Cloud_params::Cloud_params(const std::string& numbers_file,
                           const std::string& types_file)
{
    get_ion_types(types_file);
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

const Ion_type& Cloud_params::get_Ion_type_by_name(const std::string& name) const {
    // finds an Ion_type specified by name, throws a runtime_error if type not found
    
    Ion_type_container::const_iterator found_type;
    found_type = find_if(ion_types.begin(), ion_types.end(), 
                         Compare_Ion_types(name) );
    
    if ( found_type == ion_types.end() ) {
        std::ostringstream error_msg;
        error_msg << "Error: unable to find ion type: " << name;
        throw runtime_error( error_msg.str() );
    }
    return *found_type;
}

Integration_params::Integration_params(const std::string& file_name)
{
    // tries to load trap parameters from file
    ifstream ifs( file_name.c_str() );
    if ( !ifs ) {
        ifs.close();
        ostringstream error_msg;
        error_msg   << "Integrator parameter file " 
                    << "\"" << file_name << "\"" << " not found"; 
        throw runtime_error( error_msg.str() );
    }
    
    // reads in parameters from an ordered text file with each
    // line written in the form: value other_characters
    // and ignoring the text
    try {
        time_step = read_parameter<double>(ifs);
        respa_steps = read_parameter<int>(ifs);
    } catch (runtime_error& e) {
        ostringstream error_msg;
        error_msg << "Error reading parameters from file "
        << "\"" << file_name << "\"";
        throw runtime_error( error_msg.str() );
    }
    
    cout << '\n' << console_divide_line << '\n';
    cout << '\n' << "Integrator parameters read from file: " << file_name << '\n';
    cout << "\tTime step: " << time_step << '\n'
         << "\tRESPA steps: " << respa_steps << '\n';
    cout << endl;
}
