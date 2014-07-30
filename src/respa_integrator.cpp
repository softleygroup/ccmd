//
//  respa_integrator.cpp
//

#include "integrator.h"
#include "ccmd_sim.h"
#include "ion_trap.h"
#include "ion_cloud.h"

/**
 *  @class RESPA_integrator
 *  @brief Numerical integration using the RESPA algorithm.
 *
 *  The integrator only updates the Coulomb force every \c respa_steps steps.
 *  This force is used in each 
 *
 *  See: M. Tuckerman, B. J. Berne and G. J. Martyna,
 *  J. Chem. Phys. 92, 1990 (1992)
 */


/**
 *  @brief Create a new RESPA integrator.
 *
 *  @param it       Pointer to ion trap object.
 *  @param ic       Pointer to ion cloud object.
 *  @param ip       Reference to integrator parameters.
 *  @param sp       Reference to simulation parameters.
 */
RESPA_integrator::RESPA_integrator(const Ion_trap_ptr it, Ion_cloud& ic,
                                   const Integration_params& params,
                                   const Sim_params& sp)
    : Integrator(it, ic, params, sp)
{
    respa_steps = params.respa_steps;
}


/**
 *  @brief Increment by one set of respa time steps.
 *
 *  This function moves the ions and updates the trap by 2*\c dt. The ions are
 *  updated using the old Coulomb force, and the new force calculated. Then
 *  a set of respa sub-steps are taken in which only the laser and trap forces
 *  are used. Finally, a second half-step of Coulomb force and ion heating is applied.
 *
 *  @param dt   Time step.
 */
void RESPA_integrator::evolve(double dt)
{    
    double half_dt = dt/2.0;
    double dt_respa = dt/respa_steps;
    double half_dt_respa = dt_respa/2.0;
    
    // slow Coulomb force half-kick
    ions.kick(half_dt, f_coulomb.get_force() );
    // get new slow force
    f_coulomb.update();
    
    // ion stochastic heating for half-step
    ions.heat(half_dt);

    // Velocity Verlet style evolution for fast forces
    for (int i=0; i<respa_steps; ++i) {
        // update trap by half_dt_respa
        trap->evolve(half_dt_respa);
        // kick ions with resulting force
        ions.kick(half_dt_respa);

        // free flight evolution
        ions.drift(dt_respa);
        
        // update trap by half_dt_respa
        trap->evolve(half_dt_respa);
        // kick ions with resulting force
        ions.kick(half_dt_respa);

        // correct for friction forces in Velocity Verlet algorithm
        // see: M. Tuckerman and B. J. Berne, 
        //      J. Chem. Phys. 95, 4389 (1991), Eqn. 3.7 
        ions.velocity_scale(half_dt_respa);
    }
    
    // ion stochastic heating for half-step
    ions.heat(half_dt);
    
    // slow Coulomb force half-kick
    ions.kick(half_dt, f_coulomb.get_force());
    
    return;
}
