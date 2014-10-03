/** 
 * @file cosine_trap.cpp
 * @brief Function definitions a cosine trap.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

/**
 *  @class CosineTrap
 *  @brief Defines an ideal cosine waveform ion trap.
 *
 *  The time-dependent potential is a pure cosine waveform, at equal phases on 
 *  all rods. The force on an ion is determined via the Mathieu equations.
 */

#include "include/iontrap.h"

/**
 *  @brief Create a new cosine ion trap, call the parent class to initialise 
 *  parameters.
 *
 *  @param params   Reference to the trap parameters object.
 */
CosineTrap::CosineTrap(const TrapParams& params)
    : IonTrap(params) {
        cos_phase_ = 0.0;
    }


/**
 *  @brief Update the trap by time interval `dt`.
 *
 *  Increment the current time counter and calculate the amplitude of a cosine
 *  at this new time.
 *
 *  @param dt   Time step.
 */
void CosineTrap::evolve(double dt) {
    time_now_ += dt;
    cos_phase_ = cos(2.0*time_now_);
}


/**
 * @brief Calculate the force on an ion from the Mathieu equations. 
 *
 * This does not need the ion mass, but the result needs to be multiplied by the
 * charge to give the correct scaling and sign.
 *
 * @param r Ion position.
 * @return The force vector.
 */
Vector3D CosineTrap::force_now(const Vector3D& r) const {
    // Force calculation in scaled Mathieu parameter units
    Vector3D f(r);
    f.x *= +2*q_unit_mass_ * cos_phase_ - a_unit_mass_;
    f.y *= -2*q_unit_mass_ * cos_phase_ - a_unit_mass_;
    f.z *= 2*a_unit_mass_;
    return f;
}
