//
//  integrator.cpp
//

#include <iostream>
#include <algorithm>

#include "integrator.h"
#include "ion.h"
#include "ion_trap.h"
#include "ccmd_sim.h"

Integrator::Integrator(const IonTrap_ptr it, IonCloud& ic,
                       const IntegrationParams& params, const SimParams& sp)
    : trap(it), ions(ic), f_coulomb(ic, sp), integrationParams(params)
{
    // get Coulomb forces on construction
    f_coulomb.update();
}

Integrator::~Integrator() {}

