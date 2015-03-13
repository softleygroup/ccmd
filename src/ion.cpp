/** 
 * @file ion.cpp
 * @brief Definition of ion base class
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#include "include/ion.h"

#include "include/ccmdsim.h"
#include "include/ionhistogram.h"
#include "include/stats.h"

/**
 *  @class Ion
 *  @brief Base class for all ion types.
 *
 *  The ion base class stores position and velocity, and provides functions to 
 *  update the position due to free-flight, and acceleration due to a force.
 *  This class also stores ion information: mass, charge, name and formula.
 *
 *  Two member variables posStats_ and velStats_ are used to calculate the mean 
 *  and standard deviation of position and velocity. The function updateStats
 *  should be called once per time step to update these.
 *
 *  Subclasses TrappedIon and LaserCooledIon provide the correct functionality
 *  for the Ion::kick function. The TrappedIon determines the trapping force 
 *  and uses this to update the velocity, and the LaserCooledIon uses both this 
 *  and laser forces.
 *
 *  The position and velocity vectors are hidden as private variables; they
 *  can be read-out, but only modified correctly by a force or free flight.
 *
 *  @see TrappedIon, LaserCooledIon
 */

Ion::Ion(const IonType& type)
: ionType_(type) {
}

/**
 *  @brief Update position after free flight.
 *
 *  Moves the ion at a constant velocity for a time `dt`.
 *
 *  @param dt Time step.
 */
void Ion::drift(double dt) {
    pos_ += vel_*dt;
}

/**
 *  @brief Calculate the acceleration due to the force, and update the velocity.
 *  @param dt   Time step.
 *  @param f    Force vector.
 */
inline void Ion::kick(const double dt, const Vector3D& f) {
    double time_over_mass = dt/ionType_.mass;
    vel_ += f*time_over_mass;
}

/**
 *  @brief Add the kinetic energy of this ion to a histogram.
 *
 *  Calculate the total kinetic energy and kinetic energy directed along each
 *  axis, then give this to an `IonHistogram` object, with the name of this ion
 *  as a label.
 *
 *  @param ionHistogram A reference to the histogram object to update.
 *  @param trapParams A reference to the trap parameters for simulation conversion factors.
 */
void Ion::recordKE(IonHistogram_ptr ionHistogram, const TrapParams& trapParams) const {
    double energy;
    double mon2 = 0.5 * ionType_.mass;
    // total
    energy = mon2 * vel_.norm_sq() * trapParams.energy_scale;
    ionHistogram->addIon(name() + "_total", energy);
    // x - directed
    energy = mon2 * vel_[0] * vel_[0] * trapParams.energy_scale;
    ionHistogram->addIon(name() + "_x", energy);
    // y - directed
    energy = mon2 * vel_[1] * vel_[1] * trapParams.energy_scale;
    ionHistogram->addIon(name() + "_y", energy);
    // z - directed
    energy = mon2 * vel_[2] * vel_[2] * trapParams.energy_scale;
    ionHistogram->addIon(name() + "_z", energy);
}

/** 
 *  @brief Update the statistics stored by this ion.
 *
 *  Appends the current velocity and position to the `Stats` objects. Position
 *  given only as average radial distance from trap centre, and average z
 *  position because orbit of ions around trap centre results in mean x and y
 *  coordinates around zero. The velocity vector is recorded as the speed.
 */
void Ion::updateStats() {
    double r = sqrt(pos_.x*pos_.x + pos_.y * pos_.y);
    posStats_.append(Vector3D(r, pos_.z, 0));
    velStats_.append(vel_.norm());
}

/**
 * @brief take new values from the given IonType.
 *
 * To swap ion to a new identity, take mass, charge and name.
 */
void Ion::update_from(const IonType& from) {
    // ionType_ = from;
    std::cout << "NO UPDATING!";
}

