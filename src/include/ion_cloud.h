//
//  ion_cloud.h
//  CCMD
//
//  Created by Martin Bell on 09/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CCMD_ion_cloud_h
#define CCMD_ion_cloud_h

#include <vector>

class Ion;
class Ion_trap;
class ImageCollection;
class IonHistogram;
class Cloud_params;
class Vector3D;
class Ion_type;

class Ion_cloud {
public:
    Ion_cloud(const Ion_trap& ion_trap, const Cloud_params& params);
    ~Ion_cloud();

    // Update changes in ion types or trap parameters
    void update_params();

    void drift(double t);
    void kick(double t);
    void kick(double t, const std::vector<Vector3D>& fc);
    void heat(double t);
    
    //
    // This function provides the rescaling of the velocity to correct for friction 
    // forces in Velocity Verlet based integrators.
    //
    // See: Tuckerman and Berne, J. Chem. Phys. 95, 4389 (1991), Eqn. 3.7 
    //
    void velocity_scale(double dt);
    
    Vector3D get_cloud_centre(const Ion_cloud& ) const;
    void move_centre(const Vector3D& v);
    
    // scales all ion positions
    void scale(double scale_factor);
    
    Vector3D ion_position(size_t ion_index) const;
    Vector3D ion_velocity(size_t ion_index) const;
    void set_ion_position(size_t ion_index, const Vector3D& r);
    void set_ion_velocity(size_t ion_index, const Vector3D& r);
    
    int ion_charge(size_t ion_index) const;
    double ion_mass(size_t ion_index) const;
    std::string ion_name(size_t ion_index) const;
    std::string ion_formula(size_t ion_index) const;
    std::string ion_color(size_t ion_index) const;
    
    // direct calculation of Coulomb sum
    double coulomb_energy() const;
    double kinetic_energy() const;
    double total_energy() const { return kinetic_energy() + coulomb_energy(); }
    
    double aspect_ratio() const;
    
    size_t number_of_ions() const { return ion_vec.size(); }
    
    friend std::ostream& operator<<(std::ostream& os, const Ion_cloud& ic);
    
    void update_position_histogram(ImageCollection&) const;
    void update_energy_histogram(IonHistogram&) const;
    
    // changes one ion from one type to another, returns true if successful
    bool change_ion(const std::string& name_in, const std::string& name_out);
    
    void startStats() {runStats = true;}
    void stopStats() {runStats = false;}
    void updateStats();
    void saveStats(std::string basePath) const;
private:
    const Ion_trap* trap;
    const Cloud_params* cloud_params;
    friend class Coulomb_force;
    
    typedef std::vector<Ion*> Ion_ptr_vector;
    Ion_ptr_vector ion_vec;
    
    bool runStats;
    
    void add_ion(Ion* ion_ptr) { ion_vec.push_back(ion_ptr); }
    
    // changes one ion from one type to another, returns true if successful
    bool change_ion(const Ion_type& type_in, const Ion_type& type_out);
};

#endif

