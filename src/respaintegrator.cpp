/**
 * @file respaintegrator.cpp
 * @brief Function definitions for implementation of RESPA motion integrator.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#include "include/ccmdsim.h"
#include "include/integrator.h"
#include "include/ioncloud.h"
#include "include/iontrap.h"
#include "include/logger.h"

/**
 *  @class RespaIntegrator
 *  @brief Numerical integration using the RESPA algorithm.
 *
 *  The integrator only updates the Coulomb force every \c respa_steps steps.
 *  This force is used in each
 *
 *  See: M. Tuckerman, B. J. Berne and G. J. Martyna,
 *  J. Chem. Phys. 97, 1990 (1992)
 */


/**
 *  @brief Create a new RESPA integrator.
 *
 *  @param it       Pointer to ion trap object.
 *  @param ic       Pointer to ion cloud object.
 *  @param ip       Reference to integrator parameters.
 *  @param sp       Reference to simulation parameters.
 */
RespaIntegrator::RespaIntegrator(const IonTrap_ptr it, const IonCloud_ptr ic,
                                   const IntegrationParams& integrationParams,
                                   const SimParams& sp)
    : Integrator(it, ic, integrationParams, sp) {
        Logger& log = Logger::getInstance();
        log.info("Verlet integration.");
        n_iter_ = 0;
}


/** @brief Increment by one set of respa time steps.
 *
 *  This function moves the ions and updates the trap by 2*\c dt. The ions are
 *  updated using the old Coulomb force, and the new force calculated. Then a
 *  set of respa sub-steps are taken in which only the laser and trap forces
 *  are used. Finally, a second half-step of Coulomb force and ion heating is
 *  applied.
 *
 *  @param dt   Time step.
 */
void RespaIntegrator::evolve(double dt) {
    double half_dt = dt/2.0;
    double dt_respa = dt/params_.respa_steps;
    double half_dt_respa = dt_respa/2.0;
    // slow Coulomb force half-kick
    ions_->kick(half_dt, coulomb_.get_force() );
    // get new slow force
    coulomb_.update();
    // ion stochastic heating for half-step
    ions_->heat(half_dt);
    // Velocity Verlet style evolution for fast forces
    for (int i = 0; i <params_.respa_steps; ++i) {
        // update trap by half_dt_respa
        trap_->evolve(half_dt_respa);
        // kick ions with resulting force
        ions_->kick(half_dt_respa);
        // free flight evolution
        ions_->drift(dt_respa);
        // update trap by half_dt_respa
        trap_->evolve(half_dt_respa);
        // kick ions with resulting force
        ions_->kick(half_dt_respa);
        // correct for friction forces in Velocity Verlet algorithm
        // see: M. Tuckerman and B. J. Berne,
        //      J. Chem. Phys. 95, 4389 (1991), Eqn. 3.7
        ions_->velocity_scale(half_dt_respa);
    }
    // ion stochastic heating for half-step
    ions_->heat(half_dt);
    // slow Coulomb force half-kick
    ions_->kick(half_dt, coulomb_.get_force());

    // Tell everyone we're done
    notifyListeners(n_iter_++);
}
