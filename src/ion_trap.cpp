/** @class Ion_trap @brief Base class defining a quadrupole ion trap.
 *
 *  This base class stores the trap parameters and calculates the parameters
 *  for the Mathieu equation: \c a and \c q. Sub-classes inheriting this class
 *  implement their own time-dependent voltage, and force calculation.
 *
 *  @see Cosine_trap @see Pulsed_trap @see Waveform_trap
 */

#include "ion_trap.h"

#include "ccmd_sim.h"

#include <iostream>
#include <cmath>

/** @brief Over-precise value of pi. */
const double Ion_trap::pi = 3.141592653589793238462643383279502884;
/** @brief Over-precise value of epsilon_0. */
const double Ion_trap::epsilon_0 = 8.85418781762e-12;
/** @brief Over-precise value of electron charge. */
const double Ion_trap::electron_charge = 1.60217733e-19;
/** @brief 1 AMU in kg. */
const double Ion_trap::u_mass = 1.6605402e-27;

/**
 *  @brief Create a new ion trap and determine some derived quantities.
 *
 *  The Matthieu parameters \c a and \c q are calculated here without the ion
 *  mass or charge, these are multiplied into the force elsewhere. These
 *  parameters are determined for the peak potential, `v_rf`, then scaled in
 *  the force calculation.
 *
 *  @param params Reference to the trap parameters.
 */
Ion_trap::Ion_trap(const Trap_params& params) 
    : trap_params(&params)
{ 
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
    a_unit_mass =  -4.0*eta*electron_charge*v_end/(u_mass*omega*omega*z0*z0);
    q_unit_mass =  2.0*electron_charge*v_rf/(u_mass*omega*omega*r0*r0);
    
    // TODO: move into params
    length_scale = electron_charge*electron_charge/(pi*epsilon_0*u_mass*omega*omega);
    length_scale = pow(length_scale, 1.0/3.0);
    time_scale = 1/(pi * freq);
}



