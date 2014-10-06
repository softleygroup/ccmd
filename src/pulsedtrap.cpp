//
//  pulsed_trap.cpp
//

// To do: Implement parameters for square wave

#include "include/iontrap.h"

/**
 *  @brief Create a new digital ion trap, call the parent class to initialise
 *  parameters, then obtain \c tau from the trap parameters.
 *
 *  @param params   Reference to the trap parameters object.
 */
PulsedTrap::PulsedTrap(const TrapParams& params) 
    : IonTrap(params)
{
    pulse_height_ = 0;
}


/**
 *  @brief Update the trap by time interval `dt`.
 *
 *  Increment the current time counter and calculate the amplitude of the 
 *  digital waveform at this time.
 *
 *  @param dt   Time step.
 */

void PulsedTrap::evolve(double dt)
{
    time_now_ += dt;
    
    // Implementation of square wave pulse shape
    double scaled_time = time_now_/pi - std::floor( time_now_/pi );
    
    if (scaled_time <= params_.tau/2) {
        pulse_height_ = 1.0;
        return;
    } else if (scaled_time < (1.0-params_.tau)/2) {
        pulse_height_ = 0.0;
        return;
    } else if (scaled_time < (1.0+params_.tau)/2) {
        pulse_height_ = -1.0;
        return;
    } else if (scaled_time < 1.0-params_.tau/2) {
        pulse_height_ = 0.0;
        return;
    }
    pulse_height_ = 1.0;
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
Vector3D PulsedTrap::force_now(const Vector3D& r) const
{
    // Force calculation in scaled Mathieu parameter units
    Vector3D f(r);
    f.x *= +2*q_unit_mass_ * pulse_height_ - a_unit_mass_;
    f.y *= -2*q_unit_mass_ * pulse_height_ - a_unit_mass_;
    f.z *= 2*a_unit_mass_;

    return f;
}
