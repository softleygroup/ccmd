//
//  integrator.cpp
//  CCMD
//
//  Created by Martin Bell on 13/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <algorithm>

#include "integrator.h"
#include "ion.h"
#include "ion_trap.h"
#include "ccmd_sim.h"

Integrator::Integrator(Ion_trap& it, Ion_cloud& ic, 
                       const Integration_params& params)
    : trap(&it), ions(&ic), f_coulomb(ic) 
{
    f_coulomb.update();
}

Integrator::~Integrator() {
}

