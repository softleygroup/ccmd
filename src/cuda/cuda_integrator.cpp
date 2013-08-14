//
//  cuda_integrator.cpp
//


#include "cuda_integrator.h"
#include "ccmd_sim.h"
#include "ion_trap.h"
#include "ion_cloud.h"

#include "bodysystemcuda.h"
//
//  CUDA-enabled GPU-based integrator
//  see: http://http.developer.nvidia.com/GPUGems3/gpugems3_ch31.html
//

CUDA_integrator::CUDA_integrator(Ion_trap& it, Ion_cloud& ic, const Integration_params& params)
    : RESPA_integrator(it,ic,params)
{
    nbody = ions->number_of_ions();
    num_devices = 1;    // CUDA compatible cards

    // need to have a whole number of tiles > 0
    p = 256;            // threads per block
    q = 1;              // split body data over threads

    // Calculate the maximum number of threads per block
    int n = static_cast<int>(nbody);
    p = std::min(p,n);
    while ( n%p != 0 && p > 1 ) p--;

    // Write number of threads per block to cout
    //std::cout << "\n\t p = " << p << "\n\n";

    // Allocate gpu simulation
    CUDA_nbody = new BodySystemCUDA<float>(nbody,num_devices,p,q);
}

CUDA_integrator::~CUDA_integrator()
{
    delete CUDA_nbody;
}

void CUDA_integrator::arrays_to_gpu()
{
    float* pos_ptr = CUDA_nbody->getArray(BODYSYSTEM_POSITION);
    float* vel_ptr = CUDA_nbody->getArray(BODYSYSTEM_VELOCITY);
    for (int i=0; i<nbody; ++i) {
        int body_index = 4*i;

        // Assign positions
        Vector3D pos = ions->ion_position(i);
        pos_ptr[0+body_index] = static_cast<float>( pos.x );
        pos_ptr[1+body_index] = static_cast<float>( pos.y );
        pos_ptr[2+body_index] = static_cast<float>( pos.z );
        
        // Assign charges
        pos_ptr[3+body_index] = static_cast<float>( ions->ion_charge(i) );
         
        // Assign velocities
        Vector3D vel = ions->ion_velocity(i);
        vel_ptr[0+body_index] = static_cast<float>( vel.x );
        vel_ptr[1+body_index] = static_cast<float>( vel.y );
        vel_ptr[2+body_index] = static_cast<float>( vel.z );

        // Assign masses 
        vel_ptr[3+body_index] = static_cast<float>( ions->ion_mass(i) );
    }
    CUDA_nbody->setArray(BODYSYSTEM_POSITION,pos_ptr);
    CUDA_nbody->setArray(BODYSYSTEM_VELOCITY,vel_ptr);
    return;
}

void CUDA_integrator::arrays_from_gpu()
{
    float* vel_ptr = CUDA_nbody->getArray(BODYSYSTEM_VELOCITY);
    
    for (int i=0; i<nbody; ++i) {
        int body_index = 4*i;

        // Reassign velocities
        Vector3D vel;
        vel.x = static_cast<double>( vel_ptr[0+body_index] );
        vel.y = static_cast<double>( vel_ptr[1+body_index] );
        vel.z = static_cast<double>( vel_ptr[2+body_index] );

        ions->set_ion_velocity(i, vel);
    }
    return;
}


//
//  CUDA evolve: algorithm is identical to RESPA_integrator 
//
void CUDA_integrator::evolve(double dt)
{
    double half_dt = dt/2.0;
    double dt_respa = dt/respa_steps;
    double half_dt_respa = dt_respa/2.0;
    
    // transfer positions and velocities to GPU memory
    arrays_to_gpu();
    
    // kick with old force here
    kick(half_dt);
    
    // return velocities from GPU memory
    arrays_from_gpu();

    ions->heat(half_dt);
    for (int i=0; i<respa_steps; ++i) {
        trap->evolve(half_dt_respa);
        ions->kick(half_dt_respa);
        
        ions->drift(dt_respa);
        
        trap->evolve(half_dt_respa);
        ions->kick(half_dt_respa);
        ions->velocity_scale(half_dt_respa);
    }

    ions->heat(half_dt);

    // transfer positions and velocities to GPU memory
    arrays_to_gpu();
    kick(half_dt);
    // return velocities from GPU memory
    arrays_from_gpu();    

    return;
}

void CUDA_integrator::kick(double dt)
{
    CUDA_nbody->update( static_cast<float>(dt) );
}

