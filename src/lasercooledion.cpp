/**
 * @file lasercooledion.cpp
 * @brief Function definitions for class derived from Ion
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

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

#include "include/ccmdsim.h"

#include <math.h>

#include <stdlib.h>

#include <assert.h>

#include <cmath>

#include "include/vector3D.h"


/**
 *  Construct a new laser cooled ion. The trap  are passed up to the
 *  `TrappedIon` parent class, and laser cooling parameters are stored.
 *  @param ion_trap A pointer to the ion trap.
 *  @param type     A pointer to ion parameters.
 */
LaserCooledIon::LaserCooledIon(const IonTrap_ptr ion_trap,const TrapParams& trap_params, const IonType& type, const SimParams& sp, const LaserParams& lp): 
	TrappedIon(ion_trap, type, lp), heater_(sp.random_seed), trap_params(trap_params) {
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

    const double dtred = dt;
    const double force_scale = std::pow(trap_params.energy_scale, 0.5)/trap_params.time_scale;
    // 1D Laser cooling friction force
    // This force must be evaluated last to allow its effect to be
    // undone by the call to velocity_scale
	Vector3D f(0,0,0);
    double time_per_loop = (1e-9)/trap_params.time_scale;
    for(double i = 0.0; i < (dtred); i += time_per_loop){
       double fs1 = fscatt(1)*time_per_loop;
       double fs2 = fscatt(-1)*time_per_loop;
       double amu = 1.66053904e-27;
       //std::cout<<fscatt(1)<<"\n"<<std::flush;
       assert(fs1<1 && fs2<1);
       if (ElecState == 1){
           if (fs1>fs2 && heater_.testfscatt(fs1 + (time_per_loop*ionType_.A21))) {f = Emit(time_per_loop)*1.0/(time_per_loop*ionType_.mass*amu); this->Ion::kick(time_per_loop, f);}
	       if (fs2>fs1 && heater_.testfscatt(fs2 + (time_per_loop*ionType_.A21))) {f = Emit(time_per_loop)*1.0/(time_per_loop*ionType_.mass*amu); this->Ion::kick(time_per_loop, f);}
       
       }
       else if (ElecState == 0) {
           if (fs1>fs2 && heater_.testfscatt(fs1)) {f = Absorb(time_per_loop) *-1.0/(time_per_loop*ionType_.mass*amu); this->Ion::kick(time_per_loop, f);} 
           if (fs2>fs1 && heater_.testfscatt(fs2)) {f = Absorb(time_per_loop) *1.0/(time_per_loop*ionType_.mass*amu); this->Ion::kick(time_per_loop, f);}
       }
    //std::cout<<fs<<"f\n"<<std::flush;
    }
    return;
}

/**
 * @brief Find the stimulated emission/absorption probability based on the spontaneous emission probability and information about the
 * laser beam
 *
 * @param lp	A pointer to the laser parameters
 * @param type	A pointer to the ion parameters 
 */
double LaserCooledIon::fscatt(double LaserDirection) {
    
    const double pi = 3.14159265359;
    double Gamma = ionType_.A21*trap_params.time_scale;
    const double IdIsat = 1;
    double delta = lp_.delta*trap_params.time_scale;
	const double k = (2*pi*trap_params.length_scale) / lp_.wavelength ;
    
    double gamma = 0.5 * (Gamma*Gamma*Gamma);
    gamma *= IdIsat;
    const double x = delta - LaserDirection*vel_.z * k;
    gamma /= (Gamma*Gamma + (4 * x*x));
    return gamma;	
}

/**
 * @brief Increase the velocity by a vector orientated randomly over a sphere
 */
Vector3D LaserCooledIon::Emit(double dt) {
    
    const double h = 6.62607e-34;
    Vector3D SphVec = heater_.random_sphere_vector();
	SphVec *=(h/lp_.wavelength);
	ElecState = 0;
    return SphVec;
}	
 
Vector3D LaserCooledIon::Absorb(double dt){
 
    const double h = 6.62607e-34;
    const double recoil_momentum = (h/lp_.wavelength);
    Vector3D slow = Vector3D(0.0,0.0,recoil_momentum);
	ElecState = 1;
	return slow;
}

/**
 *  @brief Calculate laser cooling friction vector.
 *
 *  @return Friction vector.
 */
Vector3D LaserCooledIon::get_friction() const {
    return Vector3D(0.0, 0.0, ionType_.mass*ionType_.beta*vel_.z);
    assert(false);
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


