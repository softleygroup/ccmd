//
//  pulsed_trap.cpp
//

// To do: Implement parameters for square wave

#include "ion_trap.h"

Pulsed_trap::Pulsed_trap(const Trap_params& params) 
    : Ion_trap(params)
{
    update_trap_params();
    
    // Pulse parameters currently hardcoded
    tau = 0.226; 
    pulse_height = v_rf;
    
    pulse_shape();
    return;
}

void Pulsed_trap::evolve(double dt)
{
    time_now += dt;
    pulse_shape();
}

void Pulsed_trap::pulse_shape()
{
    // Implementation of square wave pulse shape
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

Vector3D Pulsed_trap::force_now(const Vector3D& r) const
{
    // Force calculation in scaled Mathieu parameter units
    Vector3D f(r);
    f.x *= +2*q_unit_mass*pulse_height - a_unit_mass;
    f.y *= -2*q_unit_mass*pulse_height - a_unit_mass;
    f.z *= 2*a_unit_mass;

    return f;
}
