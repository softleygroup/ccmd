//
//  integrator.cpp
//


#include "include/integrator.h"
#include "include/ion.h"
#include "include/ion_trap.h"
#include "include/ccmd_sim.h"

Integrator::Integrator(const IonTrap_ptr it, const IonCloud_ptr ic,
                       const IntegrationParams& params, const SimParams& sp)
    : trap(it), ions(ic), f_coulomb(ic, sp), integrationParams(params) {
    // get Coulomb forces on construction
    f_coulomb.update();
}

Integrator::~Integrator() {}

