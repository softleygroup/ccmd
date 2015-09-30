/**
 * @file integrator.h
 * @brief Class declaration for abstract integrator and RESPA implementation.

 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#ifndef INCLUDE_INTEGRATOR_H_
#define INCLUDE_INTEGRATOR_H_

#include "coulombforce.h"
#include "iontrap.h"
#include "ioncloud.h"
#include "integratorlistener.h"

class Vector3D;
class IntegrationParams;

class Integrator {
    friend class CoulombForce;
 public:
    Integrator(const IonTrap_ptr it, const IonCloud_ptr ic,
               const IntegrationParams& integrationParams,
               const SimParams& sp);

    void registerListener(const IntegratorListener_ptr& l);
    void deregisterListener(const IntegratorListener_ptr& l);
    void notifyListeners(const int i) const;

    virtual ~Integrator();
    virtual void evolve(double dt)=0;

    Integrator(const Integrator& ) = delete;
    Integrator& operator=(const Integrator&) = delete;
 protected:
    IonCloud_ptr ions_;
    IonTrap_ptr trap_;
    CoulombForce coulomb_;
    const IntegrationParams& params_;
    std::vector<IntegratorListener_ptr> listeners_;
};

//
// Numerical integration using the RESPA algorithm
//
// See: M. Tuckerman, B. J. Berne and G. J. Martyna
//      J. Chem. Phys. 92, 1990 (1992)
//
class RespaIntegrator : public Integrator {
 public:
    RespaIntegrator(const IonTrap_ptr it, const IonCloud_ptr ic,
                     const IntegrationParams& integrationParams,
                     const SimParams& sp);

    void evolve(double dt);

    RespaIntegrator(RespaIntegrator&) = delete;
    const RespaIntegrator operator=(const RespaIntegrator&) = delete;
 private:
    int n_iter_;
};

class VerletIntegrator : public Integrator {
 public:
    VerletIntegrator(const IonTrap_ptr it, const IonCloud_ptr ic,
                     const IntegrationParams& integrationParams,
                     const SimParams& sp);

    void evolve(double dt);

    VerletIntegrator(VerletIntegrator&) = delete;
    const VerletIntegrator operator=(const VerletIntegrator&) = delete;
 private:
    int n_iter_;
};
#endif  // INCLUDE_INTEGRATOR_H_
