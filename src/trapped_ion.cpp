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
Trapped_ion::Trapped_ion(const Ion_trap& ion_trap, const Ion_type& type) 
    : trap(&ion_trap), Ion(type)   
{
    update_ion_type();
}

void Trapped_ion::update_ion_type()
{
    Ion::update_ion_type();
    
    // trapping parameters depend on mass and charge
//    update_trap_force();
}

//void Trapped_ion::update_trap_force()
//{
//    // calculate a and q trapping parameters
//    double charge_over_mass = charge/mass;
//    a_trap = trap->a_unit_mass*charge_over_mass; 
//    q_trap = trap->q_unit_mass*charge_over_mass;
//    
//    // trap pseudopotential angular frequencies, in units of omega/2
//    trap_omega.x = sqrt(a_trap + q_trap*q_trap/2);
//    trap_omega.y = trap_omega.x;
//    trap_omega.z = sqrt(-2*a_trap);
//    plasma_omega = sqrt( pow(trap_omega.z,2) + 2*pow(trap_omega.x,2) );
//}

/** 
 * Kick the trapped ion 
 */
inline void Trapped_ion::kick(double dt)
{
    // trap force
    Vector3D f = trap->force_now(pos);
    //f *= mass * charge;
    f *= charge;
    
    // heating
    this->Ion::kick(dt,f);
}


