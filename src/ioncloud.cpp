/** 
 * @file ioncloud.cpp
 * @brief Function definitions for IonCloud class.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#include "include/ioncloud.h"

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "include/datawriter.h"
#include "include/imagecollection.h"
#include "include/ion.h"
#include "include/ionhistogram.h"
#include "include/logger.h"
#include "include/stats.h"
#include "include/vector3D.h"

class Hist3D;
int get_nearest_cube(int n);
std::vector<Vector3D> get_lattice(size_t n);

struct compare_ions_by_mass {
    bool operator()(const Ion_ptr& lhs, const Ion_ptr& rhs) const {
        return lhs->get_mass() < rhs->get_mass();
    }
};

struct position_ions {
    Ion_ptr& operator() (Ion_ptr& ion, const Vector3D& r) const {
        ion->set_position(r);
        ion->set_ElecState(0);
        return ion;
    }
};

/**
 *  @class IonCloud
 *  @brief Class to hold a collection of ions and perform actions on each.
 *
 *  # Simulation functions
 *
 *  This is the primary way of interacting with the ions in the simulation.
 *  This class holds a list of the Ion base class, and the functions Ion::kick,
 *  Ion::drift and Ion::heat iterate through this list calling the same
 *  function for each ion. The individual ion classes then do the right thing
 *  based on their specific type.
 *
 *  # Information functions
 *
 *  The class will also calculate the total potential and kinetic energy of all
 *  ions in the cloud. During the data-gathering part of the simulation,
 *  update_position_histogram, update_energy_histogram and updateStats should
 *  be called once per time step to generate the statistics required to build
 *  the ion image, and mean energies.
 *
 *  At the end of a simulation, calling saveStats will store all statistics to
 *  a text file. Generating an image, however, is handled by the
 *  ImageCollection class.
 *
 *  # Initialisation
 *
 *  The cloud is generated from the parameters in CloudParams. Each ion is
 *  initialised as either a TrappedIon or LaserCooledIon, and a pointer stored
 *  in a list. The initial position of each ion is arranged on a cubic lattice,
 *  and the initial velocity is zero.
 *
 *  @see Ion, TrappedIon, LaserCooledIon
 */

/**
 *  @brief Build a cloud of ions from the given parameters.
 *
 *  Build a new ion cloud from the parameters held in A CloudParams object. The
 *  trapped ions will be given a pointer to the IonTrap. A pointer to each ion
 *  object is put into the list of all ions, `all_ions`.
 *
 *  Generate a set of lattice points for the initial ion positions, then centre
 *  the ion cloud within the trap.
 *
 * @param ion_trap  A pointer to the ion trap object;
 * @param params    A reference to the cloud parameters object.
 *
 */
IonCloud::IonCloud(const IonTrap_ptr ion_trap, const CloudParams& cp,
        const SimParams& sp, const TrapParams& tp, const LaserParams& lp)
: cloudParams_(cp), simParams_(sp), trapParams_(tp), lp_(lp) {
    // loop over ion types to initialise ion cloud
    for (auto& it : cloudParams_.ion_type_list) {
        // loop over ions number for type, construct ions using *trap to ensure
        // that changes to ion trap parameters are felt by the ions
        for (int i = 0; i < it.number; ++i) {
            if (it.is_laser_cooled) {
                ionVec_.push_back(
                        std::make_shared<LaserCooledIon>(
                            ion_trap, tp, it, simParams_, lp_));
            } else {
                ionVec_.push_back(
                        std::make_shared<TrappedIon>(ion_trap, it, lp_));
            }
        }
    }
    // sort ions by mass
    std::sort(ionVec_.begin(), ionVec_.end(), compare_ions_by_mass() );

    // generate initial positions
    std::vector<Vector3D> lattice = get_lattice(number_of_ions());

    // move ions into position
    std::transform(ionVec_.begin(), ionVec_.end(),
                   lattice.begin(), ionVec_.begin(),
                   position_ions() );

    // move cloud centre to the origin
    Vector3D to_origin = -get_cloud_centre();
    for (auto ion : ionVec_) {
        ion->move(to_origin);
    }

    // r02 = ion_trap->trapParams->r0;
}


/** @brief Clears the list of pointers to ions.
 */
IonCloud::~IonCloud() {
    Logger& log = Logger::getInstance();
    log.debug("Deconstructing ion cloud.");
    ionVec_.clear();
}


/**
 *  @brief Call the Ion::drift function on each ion.
 *
 *  @param dt   Time step.
 */
void IonCloud::drift(double dt) {
    for (auto ion : ionVec_) {
        ion->drift(dt);
    }
}


/**
 *  @brief Call the Ion::kick function on each ion.
 *
 *  @param dt   Time step.
 */
void IonCloud::kick(double dt) {
    for (auto ion : ionVec_) {
        ion->kick(dt);
    }
}

/**
 *  @brief Delete ions if their radial position is greater than r0.
 *
 *  Simulate trap ejection and electrode collision by removing ions. Note the
 *  radial-only test is not a physical representation of the electrodes.
 */
/* IonCloud::collide
void IonCloud::collide() {
    Logger& log = Logger::getInstance();
    int n=0;
    for (auto it = ionVec_.begin(); it != ionVec_.end();) {
        Vector3D p = (*it)->get_pos();
        double r2 = p.x*p.x + p.y*p.y;
        if (r2 > r02) {
            it = ionVec_.erase(it);
            ++n;
        } else {
            ++it;
        }
    }
    std::stringstream ss;
    ss << "Removed " << n << " ions."<< std::endl;
    log.info(ss.str());
}
*/

/**
 * @brief Swap the identity of the first ion matching from to to.
 *
 * Change the name, mass and charge of ion to the values given in to.
 */
void IonCloud::swap_first(const IonType& from, const IonType& to) {
    Logger& log = Logger::getInstance();
    for (auto ion : ionVec_) {
        if (ion->name() == from.name) {
            log.debug("Found first ion named " + from.name);
            ion->update_from(to);
            return;
        }
    }
    log.warn("Did not find ion named " + from.name);
}

/**
 *  @brief Call the Ion::kick function on each ion with a specific force.
 *
 *  @param dt   Time step.
 *  @param f    Force vector.
 */
void IonCloud::kick(double dt, const std::vector<Vector3D>& f) {
    for (int i = 0; i < ionVec_.size(); ++i) {
        ionVec_[i]->kick(dt, f[i]);
    }
}


/**
 *  @brief Call the Ion::velocity_scale function on each ion.
 *
 *  This function provides the rescaling of the velocity to correct for friction
 *  forces in Velocity Verlet based integrators.
 *
 *  See: Tuckerman and Berne, J. Chem. Phys. 95, 4389 (1991), Eqn. 3.7
 *
 *  @param dt   Time step.
 */
void IonCloud::velocity_scale(double dt) {
    for (auto ion : ionVec_) {
        ion->velocity_scale(dt);
    }
}


/**
 *  @brief Call the Ion::heat function on each ion.
 *
 *  @param dt   Time step.
 */
void IonCloud::heat(double dt) {
    for (auto ion : ionVec_) {
        ion->heat(dt);
    }
}


/**
 *  @brief Determine the total kinetic energy of the ion cloud.
 *
 *  The function sums the kinetic energy of each ion.
 *
 *  @return The total kinetic energy.
 */
double IonCloud::kinetic_energy() const {
    double e = 0;
    double m;
    for (auto ion : ionVec_) {
        m = ion->get_mass();
        e += 0.5*m*(ion->get_vel().norm_sq());
    }
    return e;
}


/**
 *  @brief Determine the total Coulomb energy of the ion cloud.
 *
 *  The function sums the Coulomb energy contribution of each ion.
 *
 *  @return The total Coulomb energy.
 */
double IonCloud::coulomb_energy() const {
    Vector3D r1, r2;
    int q1, q2;
    double e = 0;
    double r;

    for (int i = 0; i < ionVec_.size(); ++i) {
        r1 = ionVec_[i]->get_pos();
        q1 = ionVec_[i]->get_charge();
        for (int j = i+1; j < ionVec_.size(); ++j) {
            r2 = ionVec_[j]->get_pos();
            q2 = ionVec_[j]->get_charge();

            if (r1 != r2) {
                r = Vector3D::dist(r1, r2);
            } else {
                std::cout << i << ' ' << r1 << ' ' << j << ' ' << r2 << '\n';
                std::abort();
            }
        e += q1*q2/r;
        }
    }
    return e;
}


/**
 *  @brief Determine the total energy of the ion cloud.
 *
 *  The function sums the Coulomb and kinetic energy contribution of each ion
 *  by calling the IonCloud::coulomb_energy() and IonCloud::kinetic_energy()
 *  functions.
 *
 *  @return The total energy.
 */
double IonCloud::total_energy() const {
    return kinetic_energy() + coulomb_energy();
}

/**
 *  @brief Call the Ion::updateStats function on each ion.
 *
 *  @param dt   Time step.
 */
void IonCloud::updateStats() {
    for (auto ion : ionVec_) {
        ion->updateStats();
    }
}


/**
 *  @brief Save the position and velocity statistics of each ion to a file.
 *
 *  This function should be called at the end of a simulation. It saves the
 *  average velocity and position to the rows of a comma-delimited file; one
 *  file for each ion type, with the ion name in the file name. The function
 *  also writes the last position and velocity vector to another file.
 *
 *  Scaling values need to be provided here to transform from simulation units
 *  to S.I. units. Currently these are stored in IonTrap::length_scale, and
 *  IonTrap::time_scale.
 *
 *  @param basePath     Directory in which to save files.
 *  @param length_scale Factor to convert distance to S.I. units.
 *  @param time_scale   Factor to convert time to S.I. units.
 */
/* IonCloud::saveStats
void IonCloud::saveStats(const std::string basePath,
                          const double length_scale,
                          const double time_scale) const {
    std::string statsFileEnding = "_stats.csv";
    std::string posFileEnding = "_pos.csv";

    double vel_scale = length_scale/time_scale;
    double x, y, z, rotated_x, rotated_y;
    double sqrt2 = 1.414213562;
    DataWriter writer(",");
    typedef Ion_ptr_vector::const_iterator ion_itr;
    std::list<double> rowdata;
    std::string name;

    // Write the header for each file
      std::string statsHeader="avg(r), var(r), avg(z), var(z), avg(KE), var(KE)";
      std::string posHeader="x, y, z, vx, vy, vz";
    for (auto& type : cloudParams_.ion_type_list) {
      name = basePath + type.name + statsFileEnding;
      writer.writeComment(name, statsHeader);
      name = basePath + type.name + posFileEnding;
      writer.writeComment(name, posHeader);
    }

    for (auto ion : ionVec_) {
        // Write the final position and velocity for each ion.
        name = basePath + ion->name() + posFileEnding;
        rowdata.clear();
        // Scale reduced units to real-world units and rotate to align to
        // axes between rods (calculation has axes crossing rods.)
        x = (ion->get_pos())[0] * length_scale;
        y = (ion->get_pos())[1] * length_scale;
        z = (ion->get_pos())[2] * length_scale;
        rotated_x = (x+y)/sqrt2;
        rotated_y = (x-y)/sqrt2;
        rowdata.push_back(rotated_x);
        rowdata.push_back(rotated_y);
        rowdata.push_back(z);

        x = (ion->get_vel())[0] * vel_scale;
        y = (ion->get_vel())[1] * vel_scale;
        z = (ion->get_vel())[2] * vel_scale;
        rotated_x = (x+y)/sqrt2;
        rotated_y = (x-y)/sqrt2;
        rowdata.push_back(rotated_x);
        rowdata.push_back(rotated_y);
        rowdata.push_back(z);

        writer.writeRow(name, rowdata);

        // Write the average data for each ion.
        name = basePath + ion->name() + statsFileEnding;
        rowdata.clear();

        Stats<double> vel = (ion->get_velStats());
        Stats<Vector3D> pos = (ion->get_posStats());
        double mon2 = (ion->get_mass())/2;
        double avg_energy = (vel.average() * vel.average()) * (mon2 * trapParams_.energy_scale);
        double var_energy = (vel.variance()/vel.average()) * (avg_energy * 1.41);
        Vector3D avg_pos = pos.average() * trapParams_.length_scale;
        Vector3D var_pos = pos.variance() * trapParams_.length_scale;

        rowdata.push_back(avg_pos[0]);
        rowdata.push_back(var_pos[0]);
        rowdata.push_back(avg_pos[1]);
        rowdata.push_back(var_pos[1]);
        rowdata.push_back(avg_energy);
        rowdata.push_back(var_energy);

        writer.writeRow(name, rowdata);
    }
}
*/


/** @brief Save the current position and velocity of all ions in the cloud.
 *
 * Can be called at any time during the simulation to save positions to a
 * comma-delimited file. No header is added, and each line is terminated with a
 * comma for Povray compatability.
 *
 *  Scaling values need to be provided here to transform from simulation units
 *  to S.I. units. Currently these are stored in IonTrap::length_scale, and
 *  IonTrap::time_scale.
 *
 *  @param basePath     Directory in which to save files.
 *  @param length_scale Factor to convert distance to S.I. units.
 *  @param time_scale   Factor to convert time to S.I. units.
 */
/* IonCloud::savePos
void IonCloud::savePos(const std::string basePath,
                          const double length_scale,
                          const double time_scale) const {
    std::string fileEnding = ".csv";

    double vel_scale = length_scale/time_scale;
    double x, y, z, rotated_x, rotated_y;
    double sqrt2 = 1.414213562;
    DataWriter writer(",");
    typedef Ion_ptr_vector::const_iterator ion_itr;
    std::list<double> rowdata;
    std::string name;

    for (auto ion : ionVec_) {
        // Write the final position and velocity for each ion.
        name = basePath + ion->name() + fileEnding;
        rowdata.clear();
        // Scale reduced units to real=world units and rotate to align to
        // axes between rods (calculation has axes crossing rods.)
        x = (ion->get_pos())[0] * length_scale;
        y = (ion->get_pos())[1] * length_scale;
        z = (ion->get_pos())[2] * length_scale;
        rotated_x = (x+y)/sqrt2;
        rotated_y = (x-y)/sqrt2;
        rowdata.push_back(rotated_x);
        rowdata.push_back(rotated_y);
        rowdata.push_back(z);

        x = (ion->get_vel())[0] * vel_scale;
        y = (ion->get_vel())[1] * vel_scale;
        z = (ion->get_vel())[2] * vel_scale;
        rotated_x = (x+y)/sqrt2;
        rotated_y = (x-y)/sqrt2;
        rowdata.push_back(rotated_x);
        rowdata.push_back(rotated_y);
        rowdata.push_back(z);

        writer.writeRow(name, rowdata);
    }
}
*/

/**
 *  @brief Collect the kinetic energy of all ions into a histogram.
 *
 *  Call the function for each ion that calculates instantaneous kinetic energy
 *  and appends it to an `IonHistogram` object.
 *
 *  @param h    The histogram object in which to record energies.
 */
void IonCloud::update_energy_histogram(IonHistogram_ptr h) const {
    for (auto ion : ionVec_) {
        ion->recordKE(h, trapParams_);
    }
}

/**
 *  @brief Add the current position of each ion to a 3D histogram.
 *
 *  The position of each ion is added to an `ImageCollection`, tagged by the
 *  name of the ion.
 *
 *  @param h    The `ImageCollection` object that holds the histogram.
 */
/*
void IonCloud::update_position_histogram(ImageCollection& h) const {
    Vector3D posn;
    Vector3D rotated_pos;
    typedef Ion_ptr_vector::const_iterator ion_itr;
    for (auto ion : ionVec_) {
        posn = ion->get_pos();
        rotated_pos.x = (posn.x+posn.y)/sqrt(2.0);
        rotated_pos.y = (posn.x-posn.y)/sqrt(2.0);
        rotated_pos.z = posn.z;
        h.addIon(ion->name(), rotated_pos);
    }
}
*/

/**
 *  @brief Determine the aspect ratio of the ion cloud.
 *
 *  Treating the ion cloud as a cylinder, calculate and return the aspect ratio.
 *  The aspect ratio is the ratio of the largest z coordinate to the largest of
 *  the \c x or \c y coordinates.
 *
 *  @return The aspect ratio.
 */
double IonCloud::aspect_ratio() const {
    // aspect ratio defined by maxmimum extent of ions
    // in x or y and z directions

    double x_max = 0.0;
    double y_max = 0.0;
    double z_max = 0.0;

    for (auto ion : ionVec_) {
        x_max = std::max(x_max, std::abs(ion->get_pos().x) );
        y_max = std::max(y_max, std::abs(ion->get_pos().y) );
        z_max = std::max(z_max, std::abs(ion->get_pos().z) );
    }
    return z_max/std::max(x_max, y_max);

/*
    // Aspect ratio defined by moment of inertia ellipsoid
    // Requires diagonalisation routine
    // NOT PORTABLE YET

    double w[3];
    double x,y,z = 0.0;

    // Calculate moment of inertia tensor
    double I[9] = {0,0,0,0,0,0,0,0,0};
    for (int i=0; i<ionVec_.size(); ++i) {
        x = ionVec_[i]->pos.x;
        y = ionVec_[i]->pos.y;
        z = ionVec_[i]->pos.z;
        I[0] += y*y + z*z;
        I[1] += -x*y;
        I[2] += -x*z;
        I[4] += x*x + z*z;
        I[5] += -y*z;
        I[8] += x*x + y*y;
    };
    I[3] = I[1];
    I[6] = I[2];
    I[7] = I[5];

    get_eigenvalues(I,w,3);

    //for (int i=0; i<3; ++i) std::cout << w[i] << std::endl;

    return w[2]/w[0];
*/
}

// Utility functions for generating initial positions

/**
 *  @brief Determine the coordinates of an \c n item lattice.
 *
 *  Utility function for initial position. The ions are initially positioned on
 *  a three-dimensional lattice to avoid problems arising from random positions.
 *
 *  @param n    Number of ions to generate lattice positions for.
 *
 *  @return     A list of coordinates, one for each ion.
 */
std::vector<Vector3D> IonCloud::get_lattice(size_t n) {
    int side = static_cast<int>(std::ceil(std::pow(n, 1.0/3.0)));
    std::vector<Vector3D> lattice;

    Vector3D grid_pos;
    double scale = 2.0;

    Vector3D to_centre = Vector3D(0.5, 0.5, 0.5)*side*scale;
    // Move ions to cubic lattice sites
    for (int i = 0; i < std::pow(side, 3.0); ++i) {
        grid_pos.x =               i%side;
        grid_pos.y =        (i/side)%side;
        grid_pos.z = (i/(side*side))%side;

        grid_pos *= scale;
        grid_pos -= to_centre;
        lattice.push_back(grid_pos);
    }
    std::sort(lattice.begin(), lattice.end());
    lattice.resize(n);

    return lattice;
}


/**
 *  @brief Find the integer that is closest to \c n when cubed.
 *
 *  Utility function for initial position. The ions are initially positioned on
 *  a three-dimensional lattice to avoid problems arising from random positions.
 *
 *  @param n    The number of ions.
 *
 *  @return     An integer close to \n^3
 */
int IonCloud::get_nearest_cube(int n) {
    // returns integer which when cubed is closest to n
    int min_cube = 1;
    int max_cube = 1;
    int i = 1;
    while (max_cube <=n) {
        min_cube = max_cube;
        ++i;
        max_cube = i*i*i;
    }
    return (n-min_cube) < (max_cube-n) ? i-1 : i;
}


/**
 *  @brief Determine the geometric centre of the cloud of ions.
 *
 *  Used during construction of a new ion cloud to centre in the trap geometry.
 *
 *  @return Cloud centre as a vector.
 */
Vector3D IonCloud::get_cloud_centre() const {
    // unweighted geometric centre of ion cloud
    Vector3D centre;
    double n_ions = ionVec_.size();

    for (auto ion : ionVec_) {
        centre += ion->get_pos();
    }

    centre /= n_ions;
    return centre;
}

