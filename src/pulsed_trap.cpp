//
//  pulsed_trap.cpp
//  CCMD
//
//  Created by Martin Bell on 02/03/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ion_trap.h"

Pulsed_trap::Pulsed_trap(const Trap_params& params) 
    : Ion_trap(params)
{
    update_trap_params();
    
    //***To be completed*** 
    // Pulse parameters
    tau = 0.1; 
    pulse_height = v_rf;
    
    pulse_shape();
    return;
}

void Pulsed_trap::update_trap_params()
{
    Ion_trap::update_trap_params();
}


void Pulsed_trap::evolve(double dt)
{
    time_now += dt;
    pulse_shape();
}

void Pulsed_trap::pulse_shape()
{
    double scaled_time = time_now/pi - floor( time_now/pi );

    if (scaled_time <= tau/2) {
        pulse_height = 1.0;
        return;
    } else if (scaled_time < (1.0-tau)/2) {
        pulse_height = 0.0;
        return;
    } else if (scaled_time < (1.0+tau)/2) {
        pulse_height = -1.0;
        return;
    } else if (scaled_time < 1.0-tau/2) {
        pulse_height = 0.0;
        return;
    }
    pulse_height = 1.0;
    return;
}

Vector3D Pulsed_trap::force_now(const Vector3D& r, double a, double q) const
{
    Vector3D f(r);
    f.x *= +2*q*pulse_height - a;
    f.y *= -2*q*pulse_height - a;
    f.z *= 2*a;

    return f;
}
