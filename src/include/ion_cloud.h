//
//  ion_cloud.h
//  CCMD
//
//  Created by Martin Bell on 09/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CCMD_ion_cloud_h
#define CCMD_ion_cloud_h

#include "ion.h"
#include <vector>
#include <boost/shared_ptr.hpp>

class ImageCollection;
class IonHistogram;
class Cloud_params;
class Vector3D;
class Ion_type;

class Ion_cloud {
public:
    Ion_cloud(const Ion_trap_ptr& ion_trap, const Cloud_params& cp,
            const Sim_params& sp);
    ~Ion_cloud();


    // Call the same-named function for each ion in turn
    void drift(double t);
    void kick(double t);
    void kick(double t, const std::vector<Vector3D>& fc);
    void heat(double t);
    void velocity_scale(double dt);
    void updateStats();
    
    double coulomb_energy() const;
    double kinetic_energy() const;
    double total_energy() const;
    
    double aspect_ratio() const;
    
    size_t number_of_ions() const { return ion_vec.size(); }
    
    void update_position_histogram(ImageCollection&) const;
    void update_energy_histogram(IonHistogram&) const;
    
    void saveStats(const std::string basePath, const double length_scale,
                   const double time_scale) const;
    void savePos (const std::string basePath, const double length_scale,
                   const double time_scale) const;
    //void collide ();

    void swap_first(const Ion_type& from, const Ion_type& to);
private:
    /** @brief Keep a reference to the parameters. */
    const Cloud_params& cloud_params;
    /** @brief Keep a reference to the parameters. */
    const Sim_params& sim_params;
    typedef std::vector<Ion_ptr> Ion_ptr_vector;
    /** A list of pointers to the ion objects. */
    Ion_ptr_vector ion_vec;
    //double r02; ///< Square of the electrode spacing.
    
    Vector3D get_cloud_centre() const;
    void move_centre(const Vector3D& v);
    static std::vector<Vector3D> get_lattice(size_t n);
    static int get_nearest_cube(int n);
    
    /** @brief Coulomb_force needs direct access to the list of ions. */
    friend class Coulomb_force;

    // Prevent copying
    Ion_cloud(const Ion_cloud&) = delete;
    const Ion_cloud& operator=(const Ion_cloud&) = delete;
};

typedef boost::shared_ptr<Ion_cloud> Ion_cloud_ptr;

#endif

