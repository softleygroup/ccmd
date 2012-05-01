//
//  coulomb_force.h
//  CCMD
//
//  Created by Martin Bell on 12/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


#ifndef CCMD_Coulomb_force_h
#define CCMD_Coulomb_force_h

//#include "Vector3D.h"
//#include "ion_cloud.h" 

#include "vector3D.h"
#include <vector>

class Ion_cloud;

class Coulomb_force {
    const Ion_cloud* ions;
    std::vector<Vector3D> force;
    void direct_force();
public:
    Coulomb_force(const Ion_cloud& ic);  
    const std::vector<Vector3D>& get_force() const { return force; };
    Vector3D get_force(int i);
    void update();
};

#endif
