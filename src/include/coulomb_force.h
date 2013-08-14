//
//  coulomb_force.h
//


#ifndef CCMD_Coulomb_force_h
#define CCMD_Coulomb_force_h

#include "vector3D.h"
#include <vector>

class Ion_cloud;

// This class calculates and stores a vector of Coulomb 
// forces for an ion cloud

class Coulomb_force {
public:
    Coulomb_force(const Ion_cloud& ic);  
    const std::vector<Vector3D>& get_force() const { return force; };
    
    // return Coulomb force on a particular ion
    Vector3D get_force(size_t i);
    
    // recalculate forces
    void update();
private:
    const Ion_cloud* ions;
    std::vector<Vector3D> force;
    void direct_force();
};

#endif
