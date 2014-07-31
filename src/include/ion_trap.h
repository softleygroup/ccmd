//
//  IonTrap.h
//

#ifndef CCMD_IonTrap_h
#define CCMD_IonTrap_h

#include "vector3D.h"
#include "ion.h"

#include <cmath>
#include <vector>
#include <boost/shared_ptr.hpp>

class TrapParams;

class IonTrap {
public:
    IonTrap(const TrapParams& params);
    
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
    double get_length_scale() const { return length_scale; }
    double get_time_scale() const { return time_scale; }
 
    /** @brief Reference to trap parameters object. */
    const TrapParams& trapParams;

    //IonTrap& operator=(IonTrap&& other) = default;
    IonTrap(const IonTrap&) = delete;
    const IonTrap& operator=(const IonTrap&) = delete;
protected:
    
    double freq;            ///< RF driving frequency.
    
    double a_unit_mass;     ///< Mathieu \c a parameter.
    double q_unit_mass;     ///< Matheiu \c q parameter.
    double length_scale;    ///< Derived simulation length scale.
    double time_scale;      ///< Derived simulation time scale.
    
    double time_now;        ///< Current time in simulation units.

    static const double pi;
    static const double epsilon_0;
    static const double electron_charge;
    static const double u_mass;
};


class CosineTrap : public IonTrap {
public:
    CosineTrap(const TrapParams& params);

    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);
    double phase_now() const {return cos_phase;}

    CosineTrap(const CosineTrap&) = delete;
    const CosineTrap& operator=(const CosineTrap&) = delete;

private:
    double cos_phase;       ///< Magnitude of the cosine function at current time.
};

class CosineDecayTrap : public CosineTrap {
public:
    CosineDecayTrap(const TrapParams& params);
    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);

    CosineDecayTrap(const CosineDecayTrap&) = delete;
    const CosineDecayTrap operator=(const CosineDecayTrap&) = delete;

private:
    double cos_phase;       ///< Magnitude of the cosine function at current time.
    double tau;
    double deltaT;
    double maxTime;
};

class TwoFreq_trap : public IonTrap {
public:
    TwoFreq_trap(const TrapParams& params);

    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);

    TwoFreq_trap(const TwoFreq_trap&) = delete;
    const TwoFreq_trap& operator=(const TwoFreq_trap&) = delete;

private:
    double cos_phase;       ///< Magnitude of the cosine function at current time.
    double freq_mult;       ///< Multiplier for second frequency
};

class PulsedTrap : public IonTrap {
public:
    PulsedTrap(const TrapParams& params);
    
    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);

    PulsedTrap(const PulsedTrap&) = delete;
    const PulsedTrap& operator=(const PulsedTrap&) = delete;
    
private:
    double tau;   
    double pulse_height;
};

class WaveformTrap : public IonTrap {
public:
    WaveformTrap(const TrapParams& params);
    
    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);

    WaveformTrap(const WaveformTrap&) = delete;
    const WaveformTrap& operator=(const WaveformTrap&) = delete;
private:
    std::vector<double> amplitudes;
    
    int npts;
    double potential;
};

#endif
