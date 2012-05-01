//
//  respa_integrator.cpp
//  CCMD
//
//  Created by Martin Bell on 19/04/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "integrator.h"
#include "ccmd_sim.h"
#include "ion_trap.h"
#include "ion_cloud.h"

double RESPA_integrator::default_timestep = 0.1;

RESPA_integrator::RESPA_integrator(Ion_trap& it, Ion_cloud& ic, 
                                   const Integration_params& params)
    : Integrator(it,ic,params)
{
    respa_steps = params.respa_steps;
    default_timestep = params.time_step;
}

void RESPA_integrator::evolve(double dt)
{    
    double half_dt = dt/2.0;
    double dt_respa = dt/respa_steps;
    double half_dt_respa = dt_respa/2.0;
    
    ions->kick(half_dt, f_coulomb.get_force() );
    
    ions->heat(half_dt);

    for (int i=0; i<respa_steps; ++i) {
        trap->evolve(half_dt_respa);
        ions->kick(half_dt_respa);

        ions->drift(dt_respa);
        
        trap->evolve(half_dt_respa);

        ions->kick(half_dt_respa);
        ions->velocity_scale(half_dt_respa);
    }
    
    ions->heat(half_dt);
    
    // get new force
    f_coulomb.update();
    
    ions->kick(half_dt, f_coulomb.get_force());
    
    return;
}
