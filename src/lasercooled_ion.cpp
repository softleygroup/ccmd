/**
 *  @class LaserCooledIon
 *  @brief Representation of a trapped and laser cooled ion.
 *  This class extends the trapped ion class to include frictional cooling
 *  forces and photon recoil heating force.
 *
 *  Heating term arising from photon recoil is implemented as a Langevin process
 *  with a Gaussian momentum distribution.
 */

#include "include/ion.h"

#include <assert.h>

#include "include/ccmd_sim.h"

/**
 *  Construct a new laser cooled ion. The trap  are passed up to the
 *  `TrappedIon` parent class, and laser cooling parameters are stored.
 *  @param ion_trap A pointer to the ion trap.
 *  @param type     A pointer to ion parameters.
 */
LaserCooledIon::LaserCooledIon(const IonTrap_ptr ion_trap, const IonType& type,
        const SimParams& sp)
    : TrappedIon(ion_trap, type), heater_(sp.random_seed) {
    assert(ionType_.is_laser_cooled == true);
    heater_.set_kick_size(sqrt(ionType_.recoil));
}

/**
 *  @brief Change the ion velocity due to the laser cooling and trapping forces.
 *  The trapping force is handled first by calling the parent class kick
 *  function. The radiation pressure and friction force (cooling) are calculated
 *  and applied.
 *
 *  @param dt   Time step.
 */
inline void LaserCooledIon::kick(double dt) {
    this->TrappedIon::kick(dt);

    // 1D radiation pressure force.
    Vector3D pressure(0.0, 0.0, 0.015);
    // Randomly apply pressure from positive or negative \c z direction,
    // weighted by the `direction` parameter.
    if (heater_.kick_direction(ionType_.direction))
        this->Ion::kick(dt, pressure);
    else
        this->Ion::kick(dt, -pressure);

    // 1D Laser cooling friction force
    // This force must be evaluated last to allow its effect to be
    // undone by the call to velocity_scale
    Vector3D friction = get_friction();
    this->Ion::kick(dt, -friction);

    return;
}

/**
 *  @brief Calculate laser cooling friction vector.
 *
 *  @return Friction vector.
 */
Vector3D LaserCooledIon::get_friction() const {
    return Vector3D(0.0, 0.0, ionType_.mass*ionType_.beta*vel_.z);
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

void LaserCooledIon::velocity_scale(double dt) {
    // Note that this routine is called with dt/2, so use
    // of "dt" referring to the complete timestep requires
    // multiplication by 2.
    // double two_dt = 2.0*dt;

    // Eqn. 3.6, undo friction term
    Vector3D friction = get_friction();
    this->Ion::kick(dt, friction);

    // Eqn. 3.7
    // vel_.z /= 1.0 + two_dt*dt*ionType_.beta/ionType_.mass;
    vel_.z /= 1.0 + dt*ionType_.beta/ionType_.mass;
}

/**
 *  @brief Change velocity due to heating.
 *  The photon recoil force is generated as a random vector.
 *
 *  @param dt   Time step size.
 */
void LaserCooledIon::heat(double dt) {
    Vector3D heating = this->heater_.random_kick();
    this->Ion::kick(dt, heating);
}


