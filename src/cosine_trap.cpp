/**
 *  @class Cosine_trap
 *  @brief Defines an ideal cosine waveform ion trap.
 *  The time-dependent potential is a pure cosine waveform, at equal phases on 
 *  all rods. The force on an ion is determined via the Mathieu equations.
 */
#include "ion.h"
#include "ion_trap.h"

/**
 *  @brief Create a new cosine ion trap, call the parent class to initialise 
 *  parameters.
 *
 *  @param params   Reference to the trap parameters object.
 */
Cosine_trap::Cosine_trap(const Trap_params& params) 
    : Ion_trap(params)
{
    cos_phase = cos(2.0*time_now);
}


/**
 *  @brief Update the trap by time interval `dt`.
 *
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
 * @brief Calculate the force on an ion at position r from the Mathieu equations. 
 *
 * This does not need the ion mass, but the result needs to be multiplied by the
 * charge to give the correct scaling and sign.
 *
 * @param r Ion position.
 * @return The force vector.
 */
Vector3D Cosine_trap::force_now(const Vector3D& r) const
{
    // Force calculation in scaled Mathieu parameter units
    Vector3D f(r);
    double qterm = q_unit_mass*cos_phase;
    f.x *= +2*qterm - a_unit_mass;
    f.y *= -2*qterm - a_unit_mass;
    f.z *= 2*a_unit_mass;
    return f;
}
