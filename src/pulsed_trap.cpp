//
//  pulsed_trap.cpp
//

// To do: Implement parameters for square wave

#include "ion_trap.h"

/**
 *  @brief Create a new digital ion trap, call the parent class to initialise
 *  parameters, then obtain \c tau from the trap parameters.
 *
 *  @param params   Reference to the trap parameters object.
 */
Pulsed_trap::Pulsed_trap(const TrapParams& params) 
    : IonTrap(params)
{
    tau = trapParams.tau;
    pulse_height = trapParams.v_rf;
}


/**
 *  @brief Update the trap by time interval `dt`.
 *
 *  Increment the current time counter and calculate the amplitude of the 
 *  digital waveform at this time.
 *
 *  @param dt   Time step.
 */

void Pulsed_trap::evolve(double dt)
{
    time_now += dt;
    
    // Implementation of square wave pulse shape
    double scaled_time = time_now/pi - floor( time_now/pi );
    
    if (scaled_time <= tau/2) {
        pulse_height = 1.0;
        return;
    } else if (scaled_time < (1.0-tau)/2) {
        pulse_height = 0.0;
        return;
    } else if (scaled_time < (1.0+tau)/2) {
        pulse_height = -1.0;
        return;
    } else if (scaled_time < 1.0-tau/2) {
        pulse_height = 0.0;
        return;
    }
    pulse_height = 1.0;
    return;
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
Vector3D Pulsed_trap::force_now(const Vector3D& r) const
{
    // Force calculation in scaled Mathieu parameter units
    Vector3D f(r);
    f.x *= +2*q_unit_mass*pulse_height - a_unit_mass;
    f.y *= -2*q_unit_mass*pulse_height - a_unit_mass;
    f.z *= 2*a_unit_mass;

    return f;
}
