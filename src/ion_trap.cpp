/** @class IonTrap @brief Base class defining a quadrupole ion trap.
 *
 *  This base class stores the trap parameters and calculates the parameters
 *  for the Mathieu equation: \c a and \c q. Sub-classes inheriting this class
 *  implement their own time-dependent voltage, and force calculation.
 *
 *  @see CosineTrap @see Pulsed_trap @see Waveform_trap
 */

#include "ion_trap.h"

#include "ccmd_sim.h"

#include <iostream>
#include <cmath>

/** @brief Over-precise value of pi. */
const double IonTrap::pi = 3.141592653589793238462643383279502884;
/** @brief Over-precise value of epsilon_0. */
const double IonTrap::epsilon_0 = 8.85418781762e-12;
/** @brief Over-precise value of electron charge. */
const double IonTrap::electron_charge = 1.60217733e-19;
/** @brief 1 AMU in kg. */
const double IonTrap::u_mass = 1.6605402e-27;

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
IonTrap::IonTrap(const TrapParams& params) 
    : trapParams(params)
{ 
    // Start at zero-time
    time_now = 0.0;
    
    // Calculate derived quantities
    double omega = 2*pi*trapParams.freq;
    a_unit_mass =  -4.0*trapParams.eta*electron_charge*trapParams.v_end
        /(u_mass*omega*omega*trapParams.z0*trapParams.z0);
    q_unit_mass =  2.0*electron_charge*trapParams.v_rf
        /(u_mass*omega*omega*trapParams.r0*trapParams.r0);
    
    // TODO: move into params
    length_scale = electron_charge*electron_charge/(pi*epsilon_0*u_mass*omega*omega);
    length_scale = pow(length_scale, 1.0/3.0);
    time_scale = 1/(pi * freq);
}



