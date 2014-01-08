//
//  integrator.h
//

#ifndef CCMD_integrator_h
#define CCMD_integrator_h

#include "ion_cloud.h" 
#include "coulomb_force.h" 
#include "ion_trap.h"

class Vector3D;
class Integration_params;

// Base class to provide interface for integrator classes
class Integrator {
    friend class Coulomb_force;
public:
    Integrator(const Ion_trap_ptr& it, Ion_cloud& ic, const Integration_params& params);
    virtual ~Integrator();
    virtual void evolve(double dt)=0;
protected:
    Ion_cloud* ions;
    Ion_trap_ptr trap;
    Coulomb_force f_coulomb;  
private:
    // prevent copying Integrator to avoid ambiguous evolution
    // of Ion_cloud and Trap classes
    Integrator(const Integrator& integrator_in);                // not implemented
    Integrator& operator=(const Integrator& integrator_in);     // not implemented
};

//
// Numerical integration using the RESPA algorithm
//
// See: M. Tuckerman, B. J. Berne and G. J. Martyna
//      J. Chem. Phys. 92, 1990 (1992)
//
class RESPA_integrator : public Integrator {
public:
    RESPA_integrator(const Ion_trap_ptr it, Ion_cloud& ic, const Integration_params& params);
 
    void set_fast_steps(int steps) { respa_steps = steps; }
    void evolve(double dt=default_timestep);

protected:
    int respa_steps;
    static double default_timestep;
};

#endif

