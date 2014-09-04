/** 
 * @file cosine_decay_trap.cpp
 * @brief Function definitions a cosine trap.
 *
 * @author Atreju Tauchinsky
 * @copyright Copyright 2014 University of Oxford.
 */

/**
 *  @class CosineDecayTrap
 *  @brief Defines an ideal cosine waveform ion trap with exponential decay in
 *  the end.
 *
 *  The time-dependent potential is a pure cosine waveform, at equal phases on
 *  all rods. The force on an ion is determined via the Mathieu equations. At
 *  time -DeltaT (from end of simulation) the cosine amplitude decays with
 *  time constant tau.
 */

#include "include/ion_trap.h"

/**
 *  @brief Create a new cosine ion trap, call the parent class to initialise 
 *  parameters.
 *
 *  @param params   Reference to the trap parameters object.
 */
CosineDecayTrap::CosineDecayTrap(const TrapParams& params)
    : CosineTrap(params) {
        cos_phase = 0.0;
        tau = params.tau;
        deltaT = params.deltaT;
    }


/**
 *  @brief Update the trap by time interval `dt`.
 *
 *  Increment the current time counter and calculate the amplitude of a cosine
 *  at this new time. If time > maxtime-DeltaT, reduce amplitude by exp(-t/tau)
 *
 *  @param dt   Time step.
 */
void CosineDecayTrap::evolve(double dt) {
    time_now += dt;
    cos_phase = cos(2.0*time_now);
    if (time_now > deltaT) {
        cos_phase *= exp(-(time_now-deltaT)/tau);
    }
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
Vector3D CosineDecayTrap::force_now(const Vector3D& r) const {
    // Force calculation in scaled Mathieu parameter units

    Vector3D f(r);
    f.x *= +2*q_unit_mass*cos_phase - a_unit_mass;
    f.y *= -2*q_unit_mass*cos_phase - a_unit_mass;
    f.z *= 2*a_unit_mass;
    return f;
}

