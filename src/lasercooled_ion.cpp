/**
 *  @class Lasercooled_ion
 *  @brief Representation of a trapped and laser cooled ion.
 *  This class extends the trapped ion class to include frictional cooling 
 *  forces and photon recoil heating force.
 *
 *  Heating term arising from photon recoil is implemented as a Langevin process
 *  with a Gaussian momentum distribution.
 */
#include <iostream>
#include <assert.h>

#include "ion.h"
#include "ccmd_sim.h"

/**
 *  Construct a new laser cooled ion. The trap parameters are passed up to the
 *  `Trapped_ion` parent class, and laser cooling parameters are stored.
 *  @param ion_trap A pointer to the ion trap.
 *  @param type     A pointer to ion parameters.
 */
Lasercooled_ion::Lasercooled_ion(const Ion_trap_ptr& ion_trap, const Ion_type& type)
    : Trapped_ion(ion_trap,type)   
{
    assert(ion_type->is_laser_cooled==true);
    beta = ion_type->beta;
    heater.set_kick_size( sqrt(ion_type->recoil) );
}

/**
 *  @brief Change the ion velocity due to the laser cooling and trapping forces.
 *  The trapping force is handled first by calling the parent class kick 
 *  function. The radiation pressure and friction force (cooling) are calculated
 *  and applied.
 *
 *  @param dt   Time step.
 */
inline void Lasercooled_ion::kick(double dt)
{
    this->Trapped_ion::kick(dt);
    
    // 1D radiation pressure force.
    Vector3D pressure(0.0, 0.0, 0.015);
    // Randomly apply pressure from positive or negative \c z direction,
    // weighted by the `direction` parameter.
    if (heater.kick_direction(ion_type->direction))
        this->Ion::kick(dt,pressure);
    else
        this->Ion::kick(dt,-pressure);

    // 1D Laser cooling friction force
    // This force must be evaluated last to allow its effect to be
    // undone by the call to velocity_scale
    Vector3D friction = get_friction();
    this->Ion::kick(dt,-friction);
    
    return;
}

/**
 *  @brief Calculate laser cooling friction vector.
 *
 *  @return Friction vector.
 */
Vector3D Lasercooled_ion::get_friction() const 
{
    return Vector3D(0.0, 0.0, mass*beta*vel.z);
}

/**
 *  @brief Correct for friction forces in Velocity Verlet algorithm.
 *  This follows the algorithm given by:
 *      M. Tuckerman and B. J. Berne,
 *      J. Chem. Phys. 95, 4389 (1991), Eqn. 
 *  Note that this routine is called with dt/2, so use of "dt" referring to the
 *  complete timestep requires multiplication by 2.
 *
 *  @param dt   Time step.
 */

void Lasercooled_ion::velocity_scale(double dt)
{
    // Note that this routine is called with dt/2, so use 
    // of "dt" referring to the complete timestep requires
    // multiplication by 2.
    // double two_dt = 2.0*dt;

    // Eqn. 3.6, undo friction term
    Vector3D friction = get_friction();
    this->Ion::kick(dt, friction);
    
    // Eqn. 3.7
    //vel.z /= 1.0 + two_dt*dt*beta/mass;
    vel.z /= 1.0 + dt*beta/mass;
}

/**
 *  @brief Change velocity due to heating.
 *  The photon recoil force is generated as a random vector.
 *
 *  @param dt   Time step size.
 */
void Lasercooled_ion::heat(double dt)
{
    Vector3D heating = this->heater.random_kick();
    this->Ion::kick( sqrt(dt) , heating);
}


