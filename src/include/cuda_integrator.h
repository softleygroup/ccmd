//
//  cuda_integrator.h
//

#ifndef CCMD_cuda_integrator_h
#define CCMD_cuda_integrator_h

#include "integrator.h" 

#include <cutil_inline.h>    // includes cuda.h and cuda_runtime_api.h
#include "bodysystemcuda.h"

class IonCloud;
class IonTrap;
class IntegrationParams;

//template <typename T>
//class BodySystemCUDA;

class CUDA_integrator : public RespaIntegrator 
{
public:
    CUDA_integrator(IonTrap& it, IonCloud& ic, const IntegrationParams& params);
    ~CUDA_integrator();

    void evolve(double dt=RespaIntegrator::default_timestep);
    
private:
    size_t nbody;       // Number of ions

    // CUDA device parameters
    int num_devices;
    int p;              // Threads per block
    int q;              // Split body data over threads

    // CUDA simulation system
    BodySystemCUDA<float>* CUDA_nbody;

    // GPU data manipulation 
    void allocate_arrays();
    void arrays_to_gpu();
    void arrays_from_gpu();

    void kick(double dt);
};

#endif

