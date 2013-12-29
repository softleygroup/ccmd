//
//  ion_cloud.h
//  CCMD
//
//  Created by Martin Bell on 09/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CCMD_ion_cloud_h
#define CCMD_ion_cloud_h

#include "ion_trap.h"
#include "ion.h"
#include <vector>

class Ion;

class ImageCollection;
class IonHistogram;
class Cloud_params;
class Vector3D;
class Ion_type;

class Ion_cloud {
public:
    Ion_cloud(const Ion_trap_ptr& ion_trap, const Cloud_params& params);
    ~Ion_cloud();


    // Call the same-named function for each ion in turn
    void drift(double t);
    void kick(double t);
    void kick(double t, const std::vector<Vector3D>& fc);
    void heat(double t);
    void velocity_scale(double dt);
    
    // direct calculation of Coulomb sum
    double coulomb_energy() const;
    double kinetic_energy() const;
    double total_energy() const;
    
    double aspect_ratio() const;
    
    size_t number_of_ions() const { return ion_vec.size(); }
    
    void update_position_histogram(ImageCollection&) const;
    void update_energy_histogram(IonHistogram&) const;
    
    
    void startStats() {runStats = true;}
    void stopStats() {runStats = false;}
    void updateStats();
    void saveStats(const std::string basePath, const double length_scale,
                   const double time_scale) const;
private:
    const Ion_trap_ptr trap;
    const Cloud_params* cloud_params;
    friend class Coulomb_force;
    
    typedef std::vector<Ion_ptr> Ion_ptr_vector;
    Ion_ptr_vector ion_vec;
    
    bool runStats;
    
    Vector3D get_cloud_centre() const;
    void move_centre(const Vector3D& v);

    static std::vector<Vector3D> get_lattice(size_t n);
    static int get_nearest_cube(int n);
};

#endif

