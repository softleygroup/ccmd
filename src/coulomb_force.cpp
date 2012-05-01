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
    // Evaluate Coulomb force by direct summation
    Vector3D r1,r2,f;
    double r,r3;
    int q1,q2;
    
    // Reinitialise forces
    Vector3D null_vec = Vector3D(0.0,0.0,0.0);
    std::fill(force.begin(),force.end(), null_vec);
        
    // Sum Coulomb force over all particles
    for (int i=0; i<ions->ion_vec.size(); ++i) {
        r1 = ions->ion_vec[i]->r();
        q1 = ions->ion_vec[i]->q();
        for (int j=i+1; j<ions->ion_vec.size(); ++j) {
            r2 = ions->ion_vec[j]->r();
            q2 = ions->ion_vec[j]->q();
            // Force term calculation
            /*
             // Checking
            if (r1 != r2) {
                r = Vector3D::dist(r1,r2);
            } else {
                std::abort();
            }
            */
            r = Vector3D::dist(r1,r2);

            //r3 = pow(r,3);
            r3 = r*r*r;
            f = (r1-r2)/r3*q1*q2;
            
            force[i] += f;
            force[j] -= f;
        }
    }
}

Vector3D Coulomb_force::get_force(int i)
{ 
    if ( i>=0 && i<ions->ion_vec.size() )
        return force[i]; 
    // To be added: throw exception
    std::abort();
}
