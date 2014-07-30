//
//  integrator.h
//

#ifndef CCMD_integrator_h
#define CCMD_integrator_h

#include "ion_cloud.h" 
#include "coulomb_force.h" 
#include "ion_trap.h"

class Vector3D;
class IntegrationParams;

// Base class to provide interface for integrator classes
class Integrator {
    friend class Coulomb_force;
 public:
    Integrator(const IonTrap_ptr it, IonCloud& ic,
               const IntegrationParams& integrationParams,
               const SimParams& sp);
    virtual ~Integrator();
    virtual void evolve(double dt)=0;

    Integrator(const Integrator& ) = delete;
    Integrator& operator=(const Integrator&) = delete;
 protected:
    IonCloud& ions;
    IonTrap_ptr trap;
    Coulomb_force f_coulomb;  
    const IntegrationParams& integrationParams;
};

//
// Numerical integration using the RESPA algorithm
//
// See: M. Tuckerman, B. J. Berne and G. J. Martyna
//      J. Chem. Phys. 92, 1990 (1992)
//
class RESPA_integrator : public Integrator {
 public:
    RESPA_integrator(const IonTrap_ptr it, IonCloud& ic,
                     const IntegrationParams& integrationParams,
                     const SimParams& sp);
 
    void evolve(double dt);

    RESPA_integrator(RESPA_integrator&) = delete;
    const RESPA_integrator operator=(const RESPA_integrator&) = delete;
};

#endif

