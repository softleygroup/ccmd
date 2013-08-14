//
//  Ion_trap.cpp
//

#include "ion_trap.h"

#include "ccmd_sim.h"

#include <boost/math/constants/constants.hpp>
#include <iostream>
#include <cmath>

const double Ion_trap::pi = boost::math::constants::pi<double>();
const double Ion_trap::epsilon_0 = 8.85418781762e-12;
const double Ion_trap::electron_charge = 1.60217733e-19;
const double Ion_trap::u_mass = 1.6605402e-27;

Ion_trap::Ion_trap(const Trap_params& params) 
    : trap_params(&params)
{ 
    update_trap_params(); 
    
    // Starting RF phase in rad
    phi0 = 0.0;
    
    // Start at zero-time
    time_now = 0.0;
}

void Ion_trap::update_trap_params()
{
    // Update trap variables
    v_rf = trap_params->v_rf;
    v_end = trap_params->v_end;
    eta = trap_params->eta;
    r0 = trap_params->r0;
    z0 = trap_params->z0;
    freq = trap_params->freq;
    
    // Update derived quantities
    omega = 2*pi*freq;
    a_unit_mass =  -4.0*eta*electron_charge*v_end/(u_mass*omega*omega*z0*z0);
    q_unit_mass =  2.0*electron_charge*v_rf/(u_mass*omega*omega*r0*r0);
    
    length_scale = electron_charge*electron_charge/(pi*epsilon_0*u_mass*omega*omega);
    length_scale = pow(length_scale, 1.0/3.0);
}

Ion_trap::~Ion_trap() {
} 

