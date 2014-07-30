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

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include <assert.h>


// To do: remove loops using std::transform

Coulomb_force::Coulomb_force(const IonCloud& ic, const SimParams& sp)
    : ionCloud(ic), sim_params(sp)
{
    force = std::vector<Vector3D>( ionCloud.number_of_ions() );
    ionsCopy = std::vector<Vector3D>(ionCloud.number_of_ions());
}


void Coulomb_force::update() 
{
    //direct_force();
    // Start calculating force in another thread so the integrator can get on with the
    // fast force calculation. Create a local copy of the ion positions so we can
    // work on them without them changing
    for (int i=0; i<ionsCopy.size(); i++) {
        ionsCopy[i] = ionCloud.ionVec_[i]->get_pos();
    }

    // Multi thread
    switch (sim_params.coulomb_threads) {
        case 0:
            direct_force();
            break;
        case 1:
            m_Thread = std::thread(&Coulomb_force::direct_force, this);
            break;
        default:
            for (int i_thread=0; i_thread<sim_params.coulomb_threads; i_thread++) {
                threads.push_back(std::thread(&Coulomb_force::split_force, this, i_thread));
            }
            break;
    }
}

void Coulomb_force::split_force(int n)
{
    // evaluates Coulomb force by direct summation, making use
    // of force antisymmetry, F_ji = -F_ij
    Vector3D r1,r2;
    double r,r3;
    int q1,q2;
    
    int imin = std::floor((double)n/(double)max_thread * ionsCopy.size());
    int imax = std::floor((double)(n+1)/(double)max_thread * ionsCopy.size());
    assert(imin>=0);
    assert(imax<=ionsCopy.size());
    // sum Coulomb force over all particles
    for (int i=imin; i<imax; ++i) {
        Vector3D force_local = Vector3D(0.0, 0.0, 0.0);
        r1 = ionsCopy[i];
        q1 = ionCloud.ionVec_[i]->get_charge();
        for (int j=0; j<ionsCopy.size(); ++j) {
            r2 = ionsCopy[j];
            q2 = ionCloud.ionVec_[j]->get_charge();
    
            if (r1==r2) continue;
            // range checking disabled in release to improve performance
            assert ( r1 != r2 );
            
            // force term calculation
            r = Vector3D::dist(r1,r2);
            r3 = r*r*r;
            force_local += (r1-r2)/r3*q1*q2;
        }
        m_Mutex.lock();
        force[i] = force_local;
        m_Mutex.unlock();
    }
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
        q1 = ionCloud.ionVec_[i]->get_charge();
        for (int j=i+1; j<ionsCopy.size(); ++j) {
            r2 = ionsCopy[j];
            q2 = ionCloud.ionVec_[j]->get_charge();
    
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
    if (sim_params.coulomb_threads>1) {
        for (auto& i : threads)
            if(i.joinable()) i.join();
        threads.clear();
    } else {
        if (m_Thread.joinable()) m_Thread.join();
    }
    return force;
};

Vector3D Coulomb_force::get_force(size_t i)
{ 
    if ( i < ionCloud.ionVec_.size() ) {
        return force[i]; 
    } else {
        throw std::runtime_error("Coulomb force: invalid index in get_force");
    }
}
