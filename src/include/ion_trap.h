//
//  Ion_trap.h
//  CCMD
//
//  Created by Martin Bell on 16/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CCMD_Ion_trap_h
#define CCMD_Ion_trap_h

#include "vector3D.h"

#include <cmath>

class Trap_params;

class Ion_trap {
    friend class Trapped_ion;
public:
    Ion_trap();
    Ion_trap(const Trap_params& params);
    virtual ~Ion_trap();
             
    virtual Vector3D force_now(const Vector3D& r, double a, double q) const = 0; 
    virtual void evolve(double time) = 0;
    
    virtual void update_trap_params();
    double get_length_scale() const { return length_scale; }

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
