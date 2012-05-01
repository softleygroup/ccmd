//
//  ccmd_sim.h
//  CCMD
//
//  Created by Martin Bell on 16/04/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CCMD_ccmd_sim_h
#define CCMD_ccmd_sim_h

#include <string>
#include <vector>
#include <map>

#include <stdexcept>

struct Trap_params {
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


struct Integration_params {
    Integration_params(const std::string& file_name = default_integrator_file);
    
    double time_step;       // Units are 2/Omega = 1/(pi*f))
    
    // RESPA algorithm inner loop number 
    //
    // respa_steps == 1 is equivalent to a velocity Verlet or
    // Leapfrog algorithm, modified for friction
    int respa_steps; 
    
private:
    std::string static default_integrator_file;
};

struct Ion_type {
    
    std::string name;       // Name to call ion
    std::string formula;    // Chemical formula
    bool visible;           // Visibility in camera image
    std::string color;      // Colour used for visualisation
    
    // Ion physical properties
    double mass;            // Mass of ion in atomic mass units
    int charge;             // Charge of ion in electron charge units
    double beta;            // Velocity damping coefficient for laser cooling
    double recoil;          // Magnitude of stochastic heating 
    
    bool is_laser_cooled;
    bool is_heated;
    
    // Comparison operator needed for map
    bool operator<(const Ion_type& rhs) const
    { 
        return this->name < rhs.name;
    }

};

// Functor to compare Ion_type pointer names 
struct ptr_name_less_than {
    bool operator()(const Ion_type* lhs, const Ion_type* rhs) const
    {
        return lhs->name < rhs->name;
    }
};

struct Cloud_params {
    Cloud_params();
    Cloud_params(const std::string& numbers_file_name,
                 const std::string& types_file_name=ion_types_file);
    
    std::vector<Ion_type> ion_types;
    
    // Store ion numbers in a map ordered by names    
    std::map<Ion_type*,int,ptr_name_less_than> ion_numbers;
    //std::map<Ion_type*,int> ion_numbers;
    
private:
    static std::string ion_types_file;
    static std::string ion_numbers_file;

    void get_ion_types(const std::string& file_name);
    void get_ion_numbers(const std::string& file_name);
};
        
class ccmd_sim {
    double time_end;
        
    Trap_params pTrap;
    Integration_params pIntegrator;
    Cloud_params pCloud;
          
public:
    ccmd_sim();
    
    void get_trap_params(Trap_params& params) const
    {
        params = pTrap;
    }
    
    void set_trap_params(const Trap_params& params)
    {
        pTrap = params;
    }
    
    void get_integrator_params(Integration_params& params) const
    {
        params = pIntegrator;
    }
    
    void set_integrator_params(const Integration_params& params)
    {
        pIntegrator = params;
    }
    
};

#endif
