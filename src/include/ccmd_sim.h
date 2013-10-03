//
//  ccmd_sim.h
//

// 
// These classes contain parameters which are used to define the 
// molecular dynamics simulation.
//
// Improvments: use XML to store/load parameters
//              simulation parent class
//

#ifndef CCMD_ccmd_sim_h
#define CCMD_ccmd_sim_h

#include <string>
#include <vector>
#include <map>

#include <stdexcept>

/*-----------------------------------------------------------------
Trap_params: this struct is constructed by specifying a text file
             containing the ion trap parameters.

To do: implement pulsed trap type (currently only cosine defined
       in constructor)
 
Trap parameter files are assumed to be of the form:
 
200.0		//	Vrf 
2.0         //	Vend
0.342		//	eta
3.5e-3		//	r0
2.75e-3		//	z0
3.85e6		//	freq
-----------------------------------------------------------------*/

class Trap_params {
public:
    Trap_params(const std::string& file_name=default_trap_file);

    // Radiofrequency parameters
    enum Waveform {Cosine, Pulsed };
    Waveform wave;      // Type of RF waveform applied to trap
    double freq;        // Trap RF frequency            (Hz)
    
    // Trap voltage parameters
    double v_rf;        // Amplitude of RF voltage      (V)
    double v_end;       // Endcap electrode voltage     (V)
    
    // Trap geometry parameters
    double eta;         // Trap geometry parameter
    double r0;          // Electrode radius             (m)
    double z0;          // Central electrode length     (m)
    
private:
    std::string static default_trap_file;
};


/*-----------------------------------------------------------------
 Integration_params: this struct is constructed by specifying a text 
 file containing the parameters for the Integrator class.
 
 Integrator parameter files are assumed to be of the form:
 
 0.1			//	Timestep
 5              //	Respa inner loop steps
-----------------------------------------------------------------*/

class Integration_params {
public:
    Integration_params(const std::string& file_name = default_integrator_file);
    
    double time_step;       // Units are 2/Omega = 1/(pi*f))
    
    // respa_steps == RESPA algorithm inner loop number 
    //
    // respa_steps == 1 is equivalent to a Velocity Verlet 
    // or Leapfrog algorithm
    //
    // See: M. Tuckerman, B. J. Berne and G. J. Martyna
    //      J. Chem. Phys. 92, 1990 (1992)
    // 
    int respa_steps; 
    
private:
    std::string static default_integrator_file;
};

class Ion_type {
    public:
    std::string name;       // Name to call ion
    std::string formula;    // Chemical formula
    bool visible;           // Visibility in camera image
    std::string color;      // Colour used for visualisation
    
    // Ion physical properties
    double mass;            // Mass of ion in atomic mass units
    int charge;             // Charge of ion in electron charge units
    double beta;            // Velocity damping coefficient for laser cooling
    double recoil;          // Magnitude of stochastic heating
    double direction;       // Ratio of left-to-right cooling intensity
    
    bool is_laser_cooled;
    bool is_heated;
    
    // Comparison operator needed for map
    bool operator<(const Ion_type& rhs) const
    {
        // compares by ion name
        return this->name < rhs.name;
    }
};

/*-----------------------------------------------------------------
 Cloud_params: this struct is constructed by specifying two text 
              files, 
 
 File 1: contains numbers of each ion type in the cloud (required)

 Assumed to be in the format (e.g.): 
 
 200 Ca
 100 Xe
 50 ND3
 
 File 2: contains the ion type definitions (optional)
 
 Types assumed to be in the format:
 
 Name Formula Mass Charge Laser-cooled? Beta Heating? ...
    Recoil Colour Visible?
 
 e.g.
 Calcium   Ca	40.0	1	Yes	 0.05	Yes	 0.03	White	Yes

-----------------------------------------------------------------*/

class Cloud_params {
public:
    Cloud_params();
    Cloud_params(const std::string& numbers_file,
                 const std::string& types_file = ion_types_file);
    
    typedef std::vector<Ion_type> Ion_type_container;
    Ion_type_container ion_types;
    
    // Functor to compare Ion_type pointer names 
    struct ptr_name_less_than {
        bool operator()(const Ion_type* lhs, const Ion_type* rhs) const
        {
            return lhs->name < rhs->name;
        }
    };
    
    const Ion_type& get_Ion_type_by_name(const std::string&) const;
    
    // store ion numbers in a map ordered by names    
    std::map<Ion_type*,int,ptr_name_less_than> ion_numbers;
    
private:
    static std::string ion_types_file;
    static std::string ion_numbers_file;

    void get_ion_types(const std::string& file_name);
    void get_ion_numbers(const std::string& file_name);
};
        
//
// *** Not currently implemented ***
// Class designed to store simulation parameters
//
class ccmd_sim {
    double time_end;
        
    Trap_params pTrap;
    Integration_params pIntegrator;
    Cloud_params pCloud;
          
public:
    ccmd_sim();
    
    void get_trap_params(Trap_params& params) const {
        params = pTrap;
    }
    
    void set_trap_params(const Trap_params& params) {
        pTrap = params;
    }
    
    void get_integrator_params(Integration_params& params) const {
        params = pIntegrator;
    }
    
    void set_integrator_params(const Integration_params& params) {
        pIntegrator = params;
    }
    
};

#endif
