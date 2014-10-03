/**
 * @file integrator.cpp
 * @brief Constructor definition for integrator base class.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#include "include/ccmdsim.h"
#include "include/integrator.h"
#include "include/ion.h"
#include "include/iontrap.h"

/** 
 * @class Integrator
 * @brief Base class for equation of motion integrators.
 *
 * This base class just initialises some local references to the trap, cloud
 * and parameters. Initialises the Coulomb force object.
 */
Integrator::Integrator(const IonTrap_ptr it, const IonCloud_ptr ic,
                       const IntegrationParams& params, const SimParams& sp)
    : trap_(it), ions_(ic), coulomb_(ic, sp), params_(params) {
    // get Coulomb forces on construction
    coulomb_.update();
}

Integrator::~Integrator() {}

