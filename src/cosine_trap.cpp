//
//  cosine_trap.cpp
//

#include "ion_trap.h"

Cosine_trap::Cosine_trap(const Trap_params& params) 
    : Ion_trap(params)
{
    update_trap_params();
    phase = phi0;
    cos_phase = cos(phase);
}

void Cosine_trap::update_trap_params()
{
    Ion_trap::update_trap_params();
    cos_phase = cos(phase);
}

void Cosine_trap::evolve(double dt)
{
    time_now += dt;
    cos_phase = cos(2.0*time_now);
}

Vector3D Cosine_trap::force_now(const Vector3D& r, double a, double q) const
{
    // Force calculation in scaled Mathieu parameter units
    Vector3D f(r);
    f.x *= +2*q*cos_phase - a;
    f.y *= -2*q*cos_phase - a;
    f.z *= 2*a;
    return f;
}
