//
//  coulomb_force.h
//


#ifndef CCMD_Coulomb_force_h
#define CCMD_Coulomb_force_h

#include "vector3D.h"
#include "ion_cloud.h"
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class IonCloud;

// This class calculates and stores a vector of Coulomb 
// forces for an ion cloud

class Coulomb_force {
public:
    Coulomb_force(const IonCloud_ptr ic, const SimParams& sp);
    const std::vector<Vector3D>& get_force();
    
    // return Coulomb force on a particular ion
    Vector3D get_force(size_t i);
    
    // recalculate forces
    void update();

private:
    const IonCloud_ptr ionCloud;
    const SimParams& sim_params;

    void direct_force();
    void split_force(int n);

    std::vector<Vector3D> ionsCopy;
    std::vector<Vector3D> force;
    int max_thread;
    std::thread m_Thread;
    std::mutex m_Mutex;
    std::vector<std::thread> threads;
    
    // Prevent copying
    Coulomb_force( const Coulomb_force & other ) = delete;
    Coulomb_force& operator=( const Coulomb_force& ) = delete;
};

#endif
