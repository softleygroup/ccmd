/**
 * @file waveformtrap.cpp
 * @brief Function definitions for a waveform trap.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

/**
 *  @class WaveformTrap
 *  @brief Ion trap that uses an arbitraty waveform for the voltage amplitude.
 *
 *  The time-dependent potential is interpolated from a set of points loaded
 *  from a text file. The file should contain only a single column representing
 *  one complete period. In the time units used here the first point is at t=0
 *  and the final at t=1. The waveform will be scaled in time to match the
 *  frequency given in the TrapParams::freq parameter. The peak voltage
 *  should be +-1 as the interpolated value will be multiplied by the
 *  TrapParams::v_rf parameter. Check the waveform carefully to ensure the
 *  ends match without causing sudden jumps in voltage.
 *
 *  The idealised Matthieu equations are used in the force calculation,
 *  therefore the absolute potential is the same on all rods (i.e. the same
 *  place in the waveform) with the negative of this value on rods 1 and 3.
 *  Note this is different to a pi phase difference within the waveform, but
 *  this should not matter if the waveform is symmetric about the centre.
 */

#include "include/iontrap.h"

#include <cmath>
#include <fstream>

#include "include/logger.h"

/**
 *  @brief Create a new waveform ion trap and load the waveform file.
 *
 *  First call the parent class to initalise the trap, then load the waveform
 *  points into the vector and populate the time axis.
 *
 *  @param params   Reference to the trap parameters object.
 */
WaveformTrap::WaveformTrap(const TrapParams& params)
    : IonTrap(params) {
    Logger &log = Logger::getInstance();
    log.info("Initialising a waveform trap...");
    std::ifstream wfFile(params_.waveformFile.c_str(), std::ifstream::in);
    double val;
    while (wfFile.good()) {
        wfFile >> val;
        amplitudes_.push_back(val);
    }
    wfFile.close();
    npts_ = static_cast<int>(amplitudes_.size());
    if (npts_ == 0) {
        log.error("Error: no points loaded from waveform file.");
        log.error("loaded from file " + params_.waveformFile);
        throw std::runtime_error("No waveform loaded");
    }
    log.info("Read " + std::to_string(npts_)
           + " points from waveform file");
}


/**
 *  @brief Update the trap by time interval `dt`.
 *
 *  Increment the current time counter and calculate the amplitude at this new
 *  time by linear interpolation between adjacent points.
 *
 *  @param dt   Time step.
 */
void WaveformTrap::evolve(double dt) {
    time_now_ += dt;
    // Get the fractional part of the time
    double timeFrac = std::fmod(time_now_/pi, 1);
    // Fractional position in the array
    double arrPos = timeFrac * npts_;
    // Get the voltage at the points either side of this, looping back to the
    // first point if we are at the right-most edge of the waveform.
    int i1 = std::floor(arrPos);
    int i2 = std::ceil(arrPos) >= npts_ ? 0 : std::ceil(arrPos);

    if (i1 == i2) {
        // If these are the same, we're directly on a defined voltage, so
        // return that
        potential_ = amplitudes_[i1];
    } else {
        // Otherwise, we have to interpolate.
        double v1 = amplitudes_[i1];
        double v2 = amplitudes_[i2];
        // Perform a linear interpolation between these voltages
        potential_ = v1 + (v2-v1) * (arrPos - std::floor(arrPos));
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
Vector3D WaveformTrap::force_now(const Vector3D& r) const {
    // Force calculation in scaled Mathieu parameter units
    Vector3D f(r);
    f.x *= +2*q_unit_mass_ * potential_ - a_unit_mass_;
    f.y *= -2*q_unit_mass_ * potential_ - a_unit_mass_;
    f.z *= 2*a_unit_mass_;
    return f;
}

