/** @class TwoFreq_trap @brief Defines a trap based on two cosine waveforms.
 *
 *  The trap frequency parameter sets the frequency of one waveform, the
 *  frequency of the second is a multiple of this. The multiple need not be an
 *  integer.
 *
 *  The time-dependent potential is a pure cosine waveform, at equal phases on
 *  all rods. The force on an ion is determined via the Mathieu equations.
 */

#include "include/iontrap.h"

/**
 *  @brief Create a new TwoFrequency ion trap, call the parent class to initialise 
 *  parameters.
 *
 *  @param params   Reference to the trap parameters object.
 */
TwoFreq_trap::TwoFreq_trap(const TrapParams& params) 
    : IonTrap(params)
{
    cos_phase = 0.0;//cos(2.0*time_now_);
    freq_mult = params.freq_mult;
}


/** @brief Update the trap by time interval `dt`.
 *
 *  Increment the current time counter and calculate the trapping voltage
 *  amplitude from the sum of a cosine at the initial trapping frequency and a
 *  second at the multiplier factor.
 *
 *  @param dt   Time step.
 */
void TwoFreq_trap::evolve(double dt) {
    time_now_ += dt; 
    cos_phase = 0.5 * (cos(2.0*time_now_) + cos (2.0*time_now_*freq_mult)); 
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
Vector3D TwoFreq_trap::force_now(const Vector3D& r) const
{
    // Force calculation in scaled Mathieu parameter units
    Vector3D f(r);
    f.x *= +2*q_unit_mass_ * cos_phase - a_unit_mass_;
    f.y *= -2*q_unit_mass_ * cos_phase - a_unit_mass_;
    f.z *= 2*a_unit_mass_;
    return f;
}
