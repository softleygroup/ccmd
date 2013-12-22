/**
 *  @class Ion_trap
 *  @brief Base class defining a quadrupole ion trap.
 *  This base class stores the trap parameters.
 */

#include "ion_trap.h"

#include "ccmd_sim.h"

#include <iostream>
#include <cmath>

const double Ion_trap::pi = 3.141592653589793238462643383279502884;
const double Ion_trap::epsilon_0 = 8.85418781762e-12;
const double Ion_trap::electron_charge = 1.60217733e-19;
const double Ion_trap::u_mass = 1.6605402e-27;

/**
 * @brief Create a new ion trap and determine some derived quantities.
 */
Ion_trap::Ion_trap(const Trap_params& params) 
    : trap_params(&params)
{ 
    // Starting RF phase in rad
    phi0 = 0.0;
    
    // Start at zero-time
    time_now = 0.0;

    // Update trap variables
    v_rf = trap_params->v_rf;
    v_end = trap_params->v_end;
    eta = trap_params->eta;
    r0 = trap_params->r0;
    z0 = trap_params->z0;
    freq = trap_params->freq;
    
    // Update derived quantities
    omega = 2*pi*freq;
    
    // TODO: move into params
    length_scale = electron_charge*electron_charge/(pi*epsilon_0*u_mass*omega*omega);
    length_scale = pow(length_scale, 1.0/3.0);
    time_scale = 1/(pi * freq);
}

Ion_trap::~Ion_trap() {
} 

