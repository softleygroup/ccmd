/**
 * @file trappedion.cpp
 * @brief Function definitions for class derived from Ion
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#include "include/ion.h"
#include "include/iontrap.h"

/**
 *  @class TrappedIon
 *  @brief Representation of a trapped ion.
 *  This class adds a kick function that includes the electrostatic trapping
 *  force at the ion's location.
 */

/**
 *  Construct a new trapped ion that stores a pointer to the ion trap
 *  @param ion_trap A pointer to the ion trap.
 *  @param type     A pointer to ion parameters.
 */
TrappedIon::TrappedIon(const IonTrap_ptr trap, const IonType& type, const LaserParams& lp)
    : trap_(trap), Ion(type, lp) {
}

/**
 *  @brief Change the ion velocity due to the trapping force.
 *  The force at this ion's position is determined from trap object, which
 *  returns the force calculated from the Matthieu equations, but missing a
 *  factor of charge. This is multipled by the force, before calling the
 *  `Ion` base class `kick` function.
 *  @param dt   Time step.
 */
inline void TrappedIon::kick(double dt) {
    Vector3D f = trap_->force_now(pos_);
    f *= ionType_.charge;
    this->Ion::kick(dt, f);
}


