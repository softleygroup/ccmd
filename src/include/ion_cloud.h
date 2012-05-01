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
class Hist3D;
class Cloud_params;
class Vector3D;

class Ion_cloud {
public:
    Ion_cloud(const Ion_trap& ion_trap, const Cloud_params& params);

    ~Ion_cloud();

    void update_params();

    void drift(double t);
    void kick(double t);
    void kick(double t, const std::vector<Vector3D>& fc);
    void heat(double t);
    void velocity_scale(double dt);
    void move_centre(const Vector3D& v);
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
    
    double coulomb_energy() const;
    double kinetic_energy() const;
    double total_energy() const { return kinetic_energy() + coulomb_energy(); }
    double aspect_ratio() const;
    
    Vector3D get_cloud_centre(const Ion_cloud& ) const;
    size_t number_of_ions() const { return ion_vec.size(); }
    
    friend std::ostream& operator<<(std::ostream& os, const Ion_cloud& ic);
    
    void update_position_histogram(Hist3D&) const;
    
private:
    const Ion_trap* trap;
    const Cloud_params* cloud_params;
    friend class Coulomb_force;
    
    void add_ion(Ion* ion_ptr) { ion_vec.push_back(ion_ptr); }
    std::vector<Ion*> ion_vec;
};

#endif

