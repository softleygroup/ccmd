//
//  Lasercooled_ion.cpp
//  CCMD
//
//  Created by Martin Bell on 23/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
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
    heater.set_kick_size( ion_type->recoil );
}

inline void Lasercooled_ion::kick(double dt)
{
    this->Trapped_ion::kick(dt);
    
    // Laser cooling friction force
    Vector3D friction( vel*(-beta)/mass );

    this->Ion::kick(dt,friction);
    
    return;
}

void Lasercooled_ion::velocity_scale(double dt)
{
    // Remove previous friction force update
    Vector3D friction( vel*(-beta)/mass );
    this->Ion::kick(dt,-friction);
    
    vel /= 1.0 + dt/2.0*beta;
}

void Lasercooled_ion::heat(double dt)
{
    Vector3D heating = this->heater.random_kick();
    this->Ion::kick( sqrt(dt) ,heating);
}


