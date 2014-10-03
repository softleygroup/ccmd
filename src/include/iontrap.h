/**
 * @file iontrap.h
 * @brief Class declarations for all ion trap types.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */
#ifndef INCLUDE_IONTRAP_H_
#define INCLUDE_IONTRAP_H_

#include <cmath>
#include <vector>

#include "include/ion.h"
#include "include/vector3D.h"

class TrapParams;

class IonTrap {
 public:
    explicit IonTrap(const TrapParams& params);

    /** @brief returns the force at a particular position in the trap.
     *
     *  Subclasses need to provide their own force calculation.
     *
     *  @param r Position vector at which to calculate force.
     *
     *  @return The force as a vector.
     */
    virtual Vector3D force_now(const Vector3D& r) const = 0;

    /** @brief evolves trap through a timestep.
     *
     * Subclasses use this function to update the time-dependent voltage on the
     * trap electrodes.
     *
     * @param dt Time step size
     */
    virtual void evolve(double dt) = 0;

    // the ion trap parameters sets real length scale
    double get_length_scale() const { return length_scale_; }
    double get_time_scale() const { return time_scale_; }

    IonTrap(const IonTrap&) = delete;
    const IonTrap& operator=(const IonTrap&) = delete;

 protected:
    double a_unit_mass_;     ///< Mathieu \c a parameter.
    double q_unit_mass_;     ///< Matheiu \c q parameter.
    double length_scale_;    ///< Derived simulation length scale.
    double time_scale_;      ///< Derived simulation time scale.

    double time_now_;        ///< Current time in simulation units.

    static const double pi;
    static const double epsilon_0;
    static const double electron_charge;
    static const double u_mass;

    /** @brief Reference to trap parameters object. */
    const TrapParams& params_;
};


class CosineTrap : public IonTrap {
 public:
    explicit CosineTrap(const TrapParams& params);

    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);

    CosineTrap(const CosineTrap&) = delete;
    const CosineTrap& operator=(const CosineTrap&) = delete;

 private:
    double cos_phase_;    ///< Magnitude of the cosine function at current time.
};

class CosineDecayTrap : public CosineTrap {
 public:
    explicit CosineDecayTrap(const TrapParams& params);
    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);

    CosineDecayTrap(const CosineDecayTrap&) = delete;
    const CosineDecayTrap operator=(const CosineDecayTrap&) = delete;

 private:
    double cos_phase_;    ///< Magnitude of the cosine function at current time.
};

class TwoFreq_trap : public IonTrap {
 public:
    explicit TwoFreq_trap(const TrapParams& params);

    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);

    TwoFreq_trap(const TwoFreq_trap&) = delete;
    const TwoFreq_trap& operator=(const TwoFreq_trap&) = delete;

 private:
    double cos_phase;    ///< Magnitude of the cosine function at current time.
    double freq_mult;    ///< Multiplier for second frequency
};

class PulsedTrap : public IonTrap {
 public:
    explicit PulsedTrap(const TrapParams& params);

    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);

    PulsedTrap(const PulsedTrap&) = delete;
    const PulsedTrap& operator=(const PulsedTrap&) = delete;

 private:
    double pulse_height_;
};

class WaveformTrap : public IonTrap {
 public:
    explicit WaveformTrap(const TrapParams& params);

    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);

    WaveformTrap(const WaveformTrap&) = delete;
    const WaveformTrap& operator=(const WaveformTrap&) = delete;
 private:
    std::vector<double> amplitudes_;

    int npts_;
    double potential_;
};

#endif  // INCLUDE_IONTRAP_H_