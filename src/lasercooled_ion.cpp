//
//  Lasercooled_ion.cpp
//

#include <iostream>

#include "ion.h"
#include "ccmd_sim.h"

Lasercooled_ion::Lasercooled_ion(const Ion_trap& ion_trap, const Ion_type& type) 
    : Trapped_ion(ion_trap,type)   
{
    update_ion_type();
}

void Lasercooled_ion::update_ion_type()
{
    Trapped_ion::update_ion_type();
    beta = ion_type->beta;
    heater.set_kick_size( sqrt(ion_type->recoil) );
}

inline void Lasercooled_ion::kick(double dt)
{
    this->Trapped_ion::kick(dt);
    
    // 1D radiation pressure force
    Vector3D pressure(0.0, 0.0, 0.015);
    this->Ion::kick(dt,pressure);


    // 1D Laser cooling friction force
    // This force must be evaluated last to allow its effect to be
    // undone by the call to velocity_scale
    Vector3D friction = get_friction();
    this->Ion::kick(dt,-friction);
    
    return;
}

Vector3D Lasercooled_ion::get_friction() const 
{
    return Vector3D(0.0, 0.0, mass*beta*vel.z);
}


void Lasercooled_ion::velocity_scale(double dt)
{
    // correct for friction forces in Velocity Verlet algorithm
    // see: M. Tuckerman and B. J. Berne, 
    //      J. Chem. Phys. 95, 4389 (1991), Eqn. 3.7 /

    // Note that this routine is called with dt/2, so use 
    // of "dt" referring to the complete timestep requires
    // multiplication by 2.
    // double two_dt = 2.0*dt;

    // Eqn. 3.6, undo friction term
    Vector3D friction = get_friction();
    this->Ion::kick(dt, friction);
    
    // Eqn. 3.7
    //vel.z /= 1.0 + two_dt*dt*beta/mass;
    vel.z /= 1.0 + dt*beta/mass;
}

void Lasercooled_ion::heat(double dt)
{
    Vector3D heating = this->heater.random_kick();
    this->Ion::kick( sqrt(dt) , heating);
}


