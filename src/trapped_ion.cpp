/**
 *  @class Trapped_ion
 *  @brief Representation of a trapped ion.
 *  This class adds a kick function that includes the electrostatic trapping
 *  force at the ion's location.
 */
#include <iostream>

#include "ion.h"
#include "ion_trap.h"

/**
 *  Construct a new trapped ion that stores a pointer to the ion trap
 *  @param ion_trap A pointer to the ion trap.
 *  @param type     A pointer to ion parameters.
 */
Trapped_ion::Trapped_ion(const Ion_trap_ptr ion_trap, const Ion_type& type)
    : trap(ion_trap), Ion(type)
{
}

/** 
 *  @brief Change the ion velocity due to the trapping force.
 *  The force at this ion's position is determined from trap object, which 
 *  returns the force calculated from the Matthieu equations, but missing a
 *  factor of charge. This is multipled by the force, before calling the 
 *  `Ion` base class `kick` function.
 *  @param dt   Time step.
 */
inline void Trapped_ion::kick(double dt)
{
    Vector3D f = trap->force_now(pos);
    f *= charge;
    
    this->Ion::kick(dt,f);
}


