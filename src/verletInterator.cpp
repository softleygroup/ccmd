#include <vector>

#include "include/integrator.h"
#include "include/ion.h"
#include "include/ioncloud.h"
#include "include/logger.h"
#include "include/vector3D.h"

VerletIntegrator::VerletIntegrator(const IonTrap_ptr it, const IonCloud_ptr ic,
                                   const IntegrationParams& integrationParams,
                                   const SimParams& sp)
    : Integrator(it, ic, integrationParams, sp) {
        Logger& log = Logger::getInstance();
        log.info("Verlet integration.");
        n_iter_ = 0;
}

void VerletIntegrator::evolve(double dt) {
    double half_dt = dt/2.0;

    std::vector<Vector3D> coulomb_force = coulomb_.get_force();
    int i = 0;
    for (auto ion : ions_->ions() ) {
        // Calculate velocity at half time-step, uses Coulomb force from
        // previous time step.
        ion->kick(half_dt, coulomb_force[i++]);
        ion->heat(half_dt);   // Heating
        ion->kick(half_dt);   // Trap, plus heating if LaserCooled.

        // Update positions by full time step
        ion->drift(dt);
    }

    // Calculate new acceleration
    coulomb_.update();
    trap_->evolve(half_dt);

    coulomb_force = coulomb_.get_force();
    i = 0;
    for (auto ion : ions_->ions() ) {
        // Update velocity over second half time-step
        ion->kick(half_dt, coulomb_force[i] );
        ion->heat(half_dt);   // Heating
        ion->kick(half_dt);   // Trap, plus heating if LaserCooled.
    }

    // Update trap again.
    coulomb_.update();
    trap_->evolve(half_dt);

    // Tell everyone we're done
    notifyListeners(n_iter_++);
}
