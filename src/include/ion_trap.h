//
//  Ion_trap.h
//

#ifndef CCMD_Ion_trap_h
#define CCMD_Ion_trap_h

#include "vector3D.h"
#include "ion.h"

#include <cmath>
#include <vector>
#include <boost/shared_ptr.hpp>

class Trap_params;

class Ion_trap {
public:
    Ion_trap(const Trap_params& params);
    
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
 
protected:
    /** @brief Pointer to trap parameters object. */
    const Trap_params* trap_params;
    
    double v_end;           ///< End-cap voltage.
    double v_rf;            ///< RF amplitude.
    double eta;             ///< Trap geometric parameter.
    double r0;              ///< Centre-to-centre trap rod radius.
    double z0;              ///< Length of trap RF electrodes.
    double freq;            ///< RF driving frequency.
    
    double a_unit_mass;     ///< Mathieu \c a parameter.
    double q_unit_mass;     ///< Matheiu \c q parameter.
    double length_scale;    ///< Derived simulation length scale.
    double time_scale;      ///< Derived simulation time scale.
    
    double omega;           ///< Trap voltage angular frequency.
    double time_now;        ///< Current time in simulation units.

    static const double pi;
    static const double epsilon_0;
    static const double electron_charge;
    static const double u_mass;
};


class Cosine_trap : public Ion_trap {
public:
    Cosine_trap(const Trap_params& params);

    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);

private:
    double cos_phase;       ///< Magnitude of the cosine function at current time.
};


class Pulsed_trap : public Ion_trap {
public:
    Pulsed_trap(const Trap_params& params);
    
    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);
    
private:
    double tau;   
    double pulse_height;
};

class Waveform_trap : public Ion_trap {
public:
    Waveform_trap(const Trap_params& params);
    
    Vector3D force_now(const Vector3D& r) const;
    void evolve(double time);
private:
    std::vector<double> amplitudes;
    
    int npts;
    double potential;
};

typedef boost::shared_ptr<Ion_trap> Ion_trap_ptr;

#endif
