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

#include <iostream>
#include <algorithm>

/** 
 * @class Integrator
 * @brief Base class for equation of motion integrators.
 *
 * This base class just initialises some local references to the trap, cloud
 * and parameters. Initialises the Coulomb force object.
 */
Integrator::Integrator(const IonTrap_ptr it, const IonCloud_ptr ic,
                       const IntegrationParams& params, const SimParams& sp)
    : trap_(it), ions_(ic), coulomb_(ic, sp), params_(params), listeners_(){
    // get Coulomb forces on construction
    coulomb_.update();
    }

Integrator::~Integrator() {}

void Integrator::registerListener(const IntegratorListener_ptr& l) {
    l->setCloud(ions_);
    listeners_.push_back(l);
}

void Integrator::deregisterListener(const IntegratorListener_ptr& l) {
    listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), l),
                     listeners_.end());
    l->finished();
}

void Integrator::notifyListeners(const int i) const {
    for (auto l : listeners_) {
        l->update(i);
    }
}
