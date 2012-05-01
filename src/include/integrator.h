//
//  integrator.h
//  CCMD
//
//  Created by Martin Bell on 13/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CCMD_integrator_h
#define CCMD_integrator_h

#include "ion_cloud.h" 
#include "coulomb_force.h" 

class Ion_trap;
class Vector3D;
class Integration_params;

class Integrator {
    friend class Coulomb_force;
protected:
    Ion_cloud* ions;
    Ion_trap* trap;
    Coulomb_force f_coulomb;  
public:
    Integrator(Ion_trap& it, Ion_cloud& ic, const Integration_params& params);
    virtual ~Integrator();
    virtual void evolve(double dt)=0;
};

class RESPA_integrator : public Integrator
{
public:
    RESPA_integrator(Ion_trap& it, Ion_cloud& ic, const Integration_params& params);
 
    void set_fast_steps(int steps) { respa_steps = steps; }
    void evolve(double dt=default_timestep);

protected:
    int respa_steps;
    static double default_timestep;
};

#endif

