//
//  cosine_trap.cpp
//

#include "ion.h"
#include "ion_trap.h"

Cosine_trap::Cosine_trap(const Trap_params& params) 
    : Ion_trap(params)
{
    update_trap_params();
    phase = phi0;
    cos_phase = cos(phase);
    
    a_unit_mass =  -4.0*eta*electron_charge*v_end/(u_mass*omega*omega*z0*z0);
    q_unit_mass =  2.0*electron_charge*v_rf/(u_mass*omega*omega*r0*r0);
}

/**
 *  @brief Update the trap by time interval `dt`.
 *  Increment the current time counter and calculate the amplitude of a cosine
 *  at this new time.
 *
 *  @param dt   Time step.
 */
void Cosine_trap::evolve(double dt)
{
    time_now += dt;
    cos_phase = cos(2.0*time_now);
}

/**
 * @briefCalculate the force on an ion at position r from the Mathieu equations. 
 * This does not use the ion mass, but the result needs to be multiplied by the
 * charge to give the correct scaling and sign.
 *
 * @param r Ion position.
 * @return The force vector.
 */
Vector3D Cosine_trap::force_now(const Vector3D& r) const
{
    // Force calculation in scaled Mathieu parameter units
    Vector3D f(r);
    f.x *= +2*q_unit_mass*cos_phase - a_unit_mass;
    f.y *= -2*q_unit_mass*cos_phase - a_unit_mass;
    f.z *= 2*a_unit_mass;
    return f;
}
