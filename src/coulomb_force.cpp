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
#include <boost/thread.hpp>

#include <assert.h>


// To do: remove loops using std::transform

Coulomb_force::Coulomb_force(const Ion_cloud& ic)
    : ionCloud(&ic)
{
    force = std::vector<Vector3D>( ic.number_of_ions() );
    ionsCopy = std::vector<Vector3D>(ic.number_of_ions());
}


void Coulomb_force::update() 
{
    //direct_force();
    // Start calculating force in another thread so the integrator can get on with the
    // fast force calculation. Create a local copy of the ion positions so we can
    // work on them without them changing
    for (int i=0; i<ionsCopy.size(); i++) {
        ionsCopy[i] = ionCloud->ion_vec[i]->r();
    }
    m_Thread = boost::thread(&Coulomb_force::direct_force, this);
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
    for (int i=0; i<ionsCopy.size(); ++i) {
        r1 = ionsCopy[i];
        q1 = ionCloud->ion_vec[i]->q();
        for (int j=i+1; j<ionsCopy.size(); ++j) {
            r2 = ionsCopy[j];
            q2 = ionCloud->ion_vec[j]->q();
    
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

const std::vector<Vector3D>& Coulomb_force::get_force()
{
    //Ensure the thread has finished before returning the new force.
    m_Thread.join();
    return force;
};

Vector3D Coulomb_force::get_force(size_t i)
{ 
    if ( i < ionCloud->ion_vec.size() ) {
        return force[i]; 
    } else {
        throw std::runtime_error("Coulomb force: invalid index in get_force");
    }
}
