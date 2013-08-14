//
//  respa_integrator.cpp
//

#include "integrator.h"
#include "ccmd_sim.h"
#include "ion_trap.h"
#include "ion_cloud.h"

//
// Numerical integration using the RESPA algorithm
//
// See: M. Tuckerman, B. J. Berne and G. J. Martyna
//      J. Chem. Phys. 92, 1990 (1992)
//

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
    
    // slow Coulomb force half-kick
    ions->kick(half_dt, f_coulomb.get_force() );
    
    // ion stochastic heating for half-step
    ions->heat(half_dt);

    // Velocity Verlet style evolution for fast forces
    for (int i=0; i<respa_steps; ++i) {
        // update trap by half_dt_respa
        trap->evolve(half_dt_respa);
        // kick ions with resulting force
        ions->kick(half_dt_respa);

        // free flight evolution
        ions->drift(dt_respa);
        
        // update trap by half_dt_respa
        trap->evolve(half_dt_respa);
        // kick ions with resulting force
        ions->kick(half_dt_respa);

        // correct for friction forces in Velocity Verlet algorithm
        // see: M. Tuckerman and B. J. Berne, 
        //      J. Chem. Phys. 95, 4389 (1991), Eqn. 3.7 
        ions->velocity_scale(half_dt_respa);
    }
    
    // ion stochastic heating for half-step
    ions->heat(half_dt);
    
    // get new slow force
    f_coulomb.update();
    
    // slow Coulomb force half-kick
    ions->kick(half_dt, f_coulomb.get_force());
    
    return;
}
