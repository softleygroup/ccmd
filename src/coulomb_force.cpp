//
//  coulomb_force.cpp
//  CCMD
//
//  Created by Martin Bell on 12/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "coulomb_force.h"
#include "ion_cloud.h"
#include "ion.h"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <exception>

#include <assert.h>

// To do: remove loops using std::transform

Coulomb_force::Coulomb_force(const Ion_cloud& ic)
    : ions(&ic) 
{
    force = std::vector<Vector3D>( ic.number_of_ions() );
}


void Coulomb_force::update() 
{
    direct_force();
    return;
}

void Coulomb_force::direct_force()
{
    // evaluates Coulomb force by direct summation, making use
    // of force antisymmetry, F_ji = -F_ij
    Vector3D r1,r2,f;
    double r,r3;
    int q1,q2;
    
    // reinitialise forces
    Vector3D null_vec = Vector3D(0.0, 0.0, 0.0);
    std::fill(force.begin(),force.end(), null_vec);
        
    // sum Coulomb force over all particles
    for (int i=0; i<ions->ion_vec.size(); ++i) {
        r1 = ions->ion_vec[i]->r();
        q1 = ions->ion_vec[i]->q();
        for (int j=i+1; j<ions->ion_vec.size(); ++j) {
            r2 = ions->ion_vec[j]->r();
            q2 = ions->ion_vec[j]->q();
    
            // range checking disabled in release to improve performance
            assert ( r1 != r2 );
            
            // force term calculation
            r = Vector3D::dist(r1,r2);
            r3 = r*r*r;
            f = (r1-r2)/r3*q1*q2;
            
            // update sum for ion "i"
            force[i] += f;
            // update sum for ion "j"
            force[j] -= f;
        }
    }
}

Vector3D Coulomb_force::get_force(size_t i)
{ 
    if ( i < ions->ion_vec.size() ) {
        return force[i]; 
    } else {
        throw std::runtime_error("Coulomb force: invalid index in get_force");
    }
}
