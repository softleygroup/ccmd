/** @file coulombforce.h
 *
 * @brief Function declarations for Coulomb force calculation.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#ifndef INCLUDE_COULOMBFORCE_H_
#define INCLUDE_COULOMBFORCE_H_

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "vector3D.h"
#include "ioncloud.h"

class IonCloud;

class CoulombForce {
 public:
    CoulombForce(const IonCloud_ptr ic, const SimParams& sp);
    const std::vector<Vector3D>& get_force();
    void update();

    CoulombForce( const CoulombForce & other ) = delete;
    CoulombForce& operator=( const CoulombForce& ) = delete;
 private:
    void direct_force();
    void split_force(int n);

    const IonCloud_ptr cloud_;   ///< Pointer to IonCloud.
    const SimParams& params_;  ///< Simulation parameters; uses coulomb_threads.
    std::vector<Vector3D> pos_;  ///< Local working copy of ion positions.
    std::vector<Vector3D> force_;   ///< Vector of forces when completed.
    std::thread m_Thread_;
    std::mutex mutex_;
    std::vector<std::thread> threads_;
};

#endif  // INCLUDE_COULOMBFORCE_H_
