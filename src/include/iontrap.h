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
#include <memory>
#include <vector>

#include "ccmdsim.h"
#include "vector3D.h"

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


    // Return the current value of the trapping voltage multiplier.
    virtual double get_phase() = 0;

    IonTrap(const IonTrap&) = delete;
    const IonTrap& operator=(const IonTrap&) = delete;

 protected:
    double a_unit_mass_;     ///< Mathieu \c a parameter.
    double q_unit_mass_;     ///< Matheiu \c q parameter.

    double time_now_;        ///< Current time in simulation units.

    static const double pi;
    static const double epsilon_0;
    static const double electron_charge;
    static const double u_mass;

    /** @brief Reference to trap parameters object. */
    const TrapParams& params_;
};
typedef std::shared_ptr<IonTrap> IonTrap_ptr;

class CosineTrap : public IonTrap {
 public:
    explicit CosineTrap(const TrapParams& params);

    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);
    double get_phase () { return cos_phase_; }

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
    double get_phase () { return cos_phase_; }

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
    double get_phase () { return cos_phase; }

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
    double get_phase () { return pulse_height_; }

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
    double get_phase () { return potential_; }

    WaveformTrap(const WaveformTrap&) = delete;
    const WaveformTrap& operator=(const WaveformTrap&) = delete;
 private:
    std::vector<double> amplitudes_;

    int npts_;
    double potential_;
};

#endif  // INCLUDE_IONTRAP_H_
