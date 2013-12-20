//
//  Ion_trap.h
//

#ifndef CCMD_Ion_trap_h
#define CCMD_Ion_trap_h

#include "vector3D.h"

#include <cmath>

class Trap_params;

// Base class to provide interface for ion trap types
class Ion_trap {
    friend class Trapped_ion;
public:
    Ion_trap();
    Ion_trap(const Trap_params& params);
    virtual ~Ion_trap();
    
    // returns the force at a particular position in the trap
    virtual Vector3D force_now(const Vector3D& r, double a, double q) const = 0;
    
    // evolves trap through a timestep
    virtual void evolve(double time) = 0;
    
    // allows parameter update if trap_params have changed
    virtual void update_trap_params();
    
    // the ion trap parameters sets real length scale
    double get_length_scale() const { return length_scale; }
    double get_time_scale() const { return time_scale; }

private:    
    double v_end;
    double eta;
    double r0;
    double z0;
    double freq;
    double omega;
    
    double a_unit_mass;
    double q_unit_mass;
    double length_scale;
    double time_scale;
 
protected:
    const Trap_params* trap_params;
    
    double time_now;
    double phi0;
    double v_rf;

    static const double pi;
    static const double epsilon_0;
    static const double electron_charge;
    static const double u_mass;
};

//
// Linear Paul ion with sinusoidal RF voltages
//
class Cosine_trap : public Ion_trap {
public:
    Cosine_trap(const Trap_params& params);

    Vector3D force_now(const Vector3D& r, double a, double q) const; 
    void evolve(double time);

    void update_trap_params();

    friend class Trapped_ion;
private:
    double phase;
    double cos_phase;
};

//
// Linear Paul ion with pulsed/square-wave RF voltages
//
class Pulsed_trap : public Ion_trap {
public:
    Pulsed_trap(const Trap_params& params);
    
    virtual Vector3D force_now(const Vector3D& r, double a, double q) const; 
    virtual void evolve(double time);

    void update_trap_params();
    
    friend class Trapped_ion;
private:
    double tau;   
    double pulse_height;
    void pulse_shape();
};

#endif
