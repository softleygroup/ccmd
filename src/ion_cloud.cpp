
#include "ion_cloud.h"

#include "ion.h"
#include "vector3D.h"
#include "ImageCollection.h"
#include "IonHistogram.h"
#include "logger.h"
#include "stats.h"
#include "DataWriter.h"

#include <functional>
#include <algorithm>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>

// To do:   fix aspect ratio
//#include "eigs.h"

using namespace std;

class Hist3D;
int get_nearest_cube(int n);
std::vector<Vector3D> get_lattice(size_t n);

struct compare_ions_by_mass {
    bool operator()(const Ion_ptr& lhs, const Ion_ptr& rhs) const
    {
        return lhs->get_mass() < rhs->get_mass();
    }
};

struct position_ions {
    Ion_ptr& operator() (Ion_ptr& ion, const Vector3D& r) const
    {
        ion->set_position(r);
        return ion;
    }
};

/**
 *  @class Ion_cloud
 *  @brief Class to hold a collection of ions and perform actions on each.
 *
 *  # Simulation functions  
 *
 *  This is the primary way of interacting with the ions in the simulation. This 
 *  class holds a list of the Ion base class, and the functions Ion::kick, 
 *  Ion::drift and 
 *  Ion::heat iterate through this list calling the same function for each ion. 
 *  The individual ion classes then do the right thing based on their specific     
 *  type.
 *
 *  # Information functions
 *
 *  The class will also calculate the total potential and kinetic energy of 
 *  all ions in the cloud. During the data-gathering part of the simulation, 
 *  update_position_histogram, update_energy_histogram and updateStats should be 
 *  called once per time step to generate the statistics required to build the 
 *  ion image, and mean energies.
 *
 *  At the end of a simulation, calling saveStats will store all statistics to     
 *  a text file. Generating an image, however, is handled by the ImageCollection 
 *  class.
 *
 *  # Initialisation
 *
 *  The cloud is generated from the parameters in 
 *  Cloud_params. Each ion is initialised as either a Trapped_ion or 
 *  Lasercooled_ion, and a pointer stored in a list. The initial 
 *  position of each ion is arranged on a cubic lattice, and the initial 
 *  velocity is zero.
 *
 *  @see Ion, Trapped_ion, Lasercooled_ion
 */

/**
 *  @brief Build a cloud of ions from the given parameters.
 *
 *  Build a new ion cloud from the parameters held in A Cloud_params object. The
 *  trapped ions will be given a pointer to the Ion_trap. A pointer to each ion
 *  object is put into the list of all ions, `all_ions`.
 *
 *  Generate a set of lattice points for the initial ion positions, then centre
 *  the ion cloud within the trap.
 *
 * @param ion_trap  A pointer to the ion trap object;
 * @param params    A reference to the cloud parameters object.
 *
 */
Ion_cloud::Ion_cloud(const Ion_trap_ptr& ion_trap, const Cloud_params& params)
: cloud_params(&params)
{
    // loop over ion types to initialise ion cloud
    for (std::list<Ion_type>::const_iterator it=cloud_params->ionTypeList.begin();
         it!=cloud_params->ionTypeList.end(); ++it) {
        // loop over ions numeber for type, construct ions using *trap to ensure
        // that changes to ion trap parameters are felt by the ions
        for (int i=0; i<it->number; ++i) {
            if (it->is_laser_cooled) {
                ion_vec.push_back(
                                  boost::make_shared<Lasercooled_ion>(ion_trap, *it));
            } else {
                
                //add_ion(new Trapped_ion(trap, *it));
                ion_vec.push_back(
                                  boost::make_shared<Trapped_ion>(ion_trap, *it));
            }
        }
    }
    // sort ions by mass
    sort(ion_vec.begin(),ion_vec.end(), compare_ions_by_mass() );
    
    // generate initial positions
    vector<Vector3D> lattice = get_lattice( number_of_ions() );
    
    // move ions into position
    std::transform(ion_vec.begin(), ion_vec.end(),
                   lattice.begin(), ion_vec.begin(),
                   position_ions() );
    
    // move cloud centre to the origin
    Vector3D to_origin = -get_cloud_centre();
    BOOST_FOREACH(Ion_ptr ion, ion_vec) {
        ion->move(to_origin);
    }

    r02 = ion_trap->trap_params->r0;
}


/** @brief Clears the list of pointers to ions.
 */
Ion_cloud::~Ion_cloud()
{
    ion_vec.clear();
}


/**
 *  @brief Call the Ion::drift function on each ion.
 *
 *  @param dt   Time step.
 */
void Ion_cloud::drift(double dt)
{
    BOOST_FOREACH(Ion_ptr ion, ion_vec) {
        ion->drift(dt);
    }
}


/**
 *  @brief Call the Ion::kick function on each ion.
 *
 *  @param dt   Time step.
 */
void Ion_cloud::kick(double dt)
{
    BOOST_FOREACH(Ion_ptr ion, ion_vec) {
        ion->kick(dt);
    }
}

/**
 *  @brief Delete ions if their radial position is greater than r0.
 *
 *  Simulate trap ejection and electrode collision by removing ions. Note the
 *  radial-only test is not a physical representation of the electrodes.
 */
void Ion_cloud::collide()
{
    Logger& log = Logger::getInstance();
    int n=0;
    for (auto it = ion_vec.begin(); it != ion_vec.end();) {
        Vector3D p = (*it)->get_pos();
        double r2 = p.x*p.x + p.y*p.y;
        if (r2 > r02) {
            it = ion_vec.erase(it);
            ++n;
        } else {
            ++it;
        }
    }
    std::stringstream ss;
    ss << "Removed " << n << " ions."<< std::endl;
    log.log(Logger::info, ss.str());
}

/**
 *  @brief Call the Ion::kick function on each ion with a specific force.
 *
 *  @param dt   Time step.
 *  @param f    Force vector.
 */
void Ion_cloud::kick(double dt, const std::vector<Vector3D>& f)
{
    for (int i=0; i<ion_vec.size(); ++i) {
        ion_vec[i]->kick(dt, f[i] );
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
void Ion_cloud::velocity_scale(double dt)
{
    BOOST_FOREACH(Ion_ptr ion, ion_vec) {
        ion->velocity_scale(dt);
    }
}


/**
 *  @brief Call the Ion::heat function on each ion.
 *
 *  @param dt   Time step.
 */
void Ion_cloud::heat(double dt)
{
    BOOST_FOREACH(Ion_ptr ion, ion_vec) {
        ion->heat(dt);
    }
}


/**
 *  @brief Determine the total kinetic energy the ion cloud.
 *
 *  The function sums the kinetic energy of each ion.
 *
 *  @return The total kinetic energy.
 */
double Ion_cloud::kinetic_energy() const
{
    double e = 0;
    double m;
    BOOST_FOREACH(Ion_ptr ion, ion_vec) {
        m = ion->get_mass();
        e += 0.5*m*( ion->get_vel().norm_sq() );
    }
    return e;
}


/**
 *  @brief Determine the total Coulomb energy the ion cloud.
 *
 *  The function sums the Coulomb energy contribution of each ion.
 *
 *  @return The total Coulomb energy.
 */
double Ion_cloud::coulomb_energy() const
{
    Vector3D r1,r2;
    int q1,q2;
    double e = 0;
    double r;
    
    for (int i=0; i<ion_vec.size(); ++i) {
        r1 = ion_vec[i]->get_pos();
        q1 = ion_vec[i]->get_charge();
        for (int j=i+1; j<ion_vec.size(); ++j) {
            r2 = ion_vec[j]->get_pos();
            q2 = ion_vec[j]->get_charge();
            
            if (r1 != r2) {
                r = Vector3D::dist(r1,r2);
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
 *  @brief Determine the total energy the ion cloud.
 *
 *  The function sums the Coulomb and kinetic energy contribution of each ion 
 *  by calling the Ion_cloud::coulomb_energy() and Ion_cloud::kinetic_energy()
 *  functions.
 *
 *  @return The total energy.
 */
double Ion_cloud::total_energy() const
{
    return kinetic_energy() + coulomb_energy();
}

/**
 *  @brief Call the Ion::updateStats function on each ion.
 *
 *  @param dt   Time step.
 */
void Ion_cloud::updateStats()
{
    BOOST_FOREACH(Ion_ptr ion, ion_vec) {
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
 *  to S.I. units. Currently these are stored in Ion_trap::length_scale, and
 *  Ion_trap::time_scale.
 *
 *  @param basePath     Directory in which to save files.
 *  @param length_scale Factor to convert distance to S.I. units.
 *  @param time_scale   Factor to convert time to S.I. units.
 */
void Ion_cloud::saveStats(const std::string basePath,
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
      std::string statsHeader="<KE_x>\tvar(KE_x)\t<KE_y>\tvar(KE_y)\t<KE_z>\tvar(KE_z)\t<pos_x>\tvar(pos_x)\t<pos_y>\tvar(pos_y)\t<pos_z>\tvar(pos_z)";
      std::string posHeader="x\ty\tz\tvx\tvy\tvz";
    BOOST_FOREACH(Ion_type type, cloud_params->ionTypeList)
    {
      name = basePath + type.name + statsFileEnding;
      writer.writeComment(name, statsHeader);
      name = basePath + type.name + posFileEnding;
      writer.writeComment(name, posHeader);
    }
    
    BOOST_FOREACH(Ion_ptr ion, ion_vec)
    {
        // Write the final position and velocity for each ion.
    	name = basePath + ion->name() + posFileEnding;
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
        
        // Write the average data for each ion.
        name = basePath + ion->name() + statsFileEnding;
        rowdata.clear();
        
        Stats<Vector3D> energy = ion->get_velStats();
        Stats<Vector3D> pos = ion->get_posStats();
        double mon2 = (ion->get_mass())/2;
        Vector3D avg_energy = energy.average();
        Vector3D var_energy = energy.variance();
        Vector3D avg_pos = pos.average();
        Vector3D var_pos = pos.variance();
        
        rowdata.push_back(avg_energy[0]*mon2);
        rowdata.push_back(var_energy[0]*mon2);
        rowdata.push_back(avg_energy[1]*mon2);
        rowdata.push_back(var_energy[1]*mon2);
        rowdata.push_back(avg_energy[2]*mon2);
        rowdata.push_back(var_energy[2]*mon2);
        rowdata.push_back(avg_pos[0]);
        rowdata.push_back(var_pos[0]);
        rowdata.push_back(avg_pos[1]);
        rowdata.push_back(var_pos[1]);
        rowdata.push_back(avg_pos[2]);
        rowdata.push_back(var_pos[2]);
        
        writer.writeRow(name, rowdata);
    }
}

/** @brief Save the current position and velocity of all ions in the cloud.
 *
 * Can be called at any time during the simulation to save positions to a
 * comma-delimited file. No header is added, and each line is terminated with a
 * comma for Povray compatability.
 *
 *  Scaling values need to be provided here to transform from simulation units
 *  to S.I. units. Currently these are stored in Ion_trap::length_scale, and
 *  Ion_trap::time_scale.
 *
 *  @param basePath     Directory in which to save files.
 *  @param length_scale Factor to convert distance to S.I. units.
 *  @param time_scale   Factor to convert time to S.I. units.
 */
void Ion_cloud::savePos(const std::string basePath,
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

    BOOST_FOREACH(Ion_ptr ion, ion_vec)
    {
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

/**
 *  @brief Collect the kinetic energy of all ions into a histogram.
 *
 *  Call the function for each ion that calculates instantaneous kinetic energy
 *  and appends it to an `IonHistogram` object.
 *
 *  @param h    The histogram object in which to record energies.
 */
void Ion_cloud::update_energy_histogram(IonHistogram& h) const
{
    BOOST_FOREACH(Ion_ptr ion, ion_vec )
    {
        ion->recordKE(h);
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
void Ion_cloud::update_position_histogram(ImageCollection& h) const
{
    Vector3D posn;
    Vector3D rotated_pos;
    typedef Ion_ptr_vector::const_iterator ion_itr;
    BOOST_FOREACH(Ion_ptr ion, ion_vec )
    {
        posn = ion->get_pos();
        rotated_pos.x = (posn.x+posn.y)/sqrt(2.0);
        rotated_pos.y = (posn.x-posn.y)/sqrt(2.0);
        rotated_pos.z = posn.z;
        h.addIon(ion->name(), rotated_pos);
    }
}


/**
 *  @brief Determine the aspect ratio of the ion cloud.
 *
 *  Treating the ion cloud as a cylinder, calculate and return the aspect ratio.
 *  The aspect ratio is the ratio of the largest z coordinate to the largest of
 *  the \c x or \c y coordinates.
 *
 *  @return The aspect ratio.
 */
double Ion_cloud::aspect_ratio() const
{
    // aspect ratio defined by maxmimum extent of ions
    // in x or y and z directions 
    using namespace std;
    
    double x_max = 0.0;
    double y_max = 0.0;
    double z_max = 0.0;

    for (int i=0; i<ion_vec.size(); ++i) {
        x_max = max(x_max, abs(ion_vec[i]->get_pos().x) );
        y_max = max(y_max, abs(ion_vec[i]->get_pos().y) );
        z_max = max(z_max, abs(ion_vec[i]->get_pos().z) );
    };
    return z_max/max(x_max,y_max); 
    
/*
    // Aspect ratio defined by moment of inertia ellipsoid
    // Requires diagonalisation routine
    // NOT PORTABLE YET
 
    double w[3];
    double x,y,z = 0.0;
    
    // Calculate moment of inertia tensor
    double I[9] = {0,0,0,0,0,0,0,0,0};
    for (int i=0; i<ion_vec.size(); ++i) {
        x = ion_vec[i]->pos.x;
        y = ion_vec[i]->pos.y;
        z = ion_vec[i]->pos.z;
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
std::vector<Vector3D> Ion_cloud::get_lattice(size_t n)
{
    int side = int(ceil( pow(n,1.0/3.0) ));
    std::vector<Vector3D> lattice;
    
	Vector3D grid_pos;
    double scale = 2.0;
    
    Vector3D to_centre = Vector3D(0.5,0.5,0.5)*side*scale;
	// Move ions to cubic lattice sites
	for (int i=0; i<pow(side,3.0); ++i) {
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

int Ion_cloud::get_nearest_cube(int n)
{
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
Vector3D Ion_cloud::get_cloud_centre() const
{
    // 	unweighted geometric centre of ion cloud
	Vector3D centre;
    double n_ions = ion_vec.size();
    
    BOOST_FOREACH(Ion_ptr ion, ion_vec) {
        centre += ion->get_pos();
    }
    
	centre /= n_ions;
	return centre;
}

