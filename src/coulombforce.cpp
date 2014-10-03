/** 
 * @brief Function definitions for coulomb force calculation.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#include "include/coulombforce.h"

#include <assert.h>

#include <cmath>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "include/ioncloud.h"
#include "include/ion.h"

/** @class CoulombForce @brief Calculates a vector of Coulomb interaction
 * forces for the ion positions when update is called.
 *
 * Stores a pointer to the IonCloud for positions and charges. On calling
 * update, the current positions are copied into another vector. This allows
 * the slower Coulomb force calculation to be done in another thread while the
 * trapping forces are calculated and updated.
 */

/** @brief Construct a new CoulombForce object that stores a pointer to the
 * IonCloud and SimParams.
 *
 */
CoulombForce::CoulombForce(const IonCloud_ptr ic, const SimParams& sp)
    : cloud_(ic), params_(sp) {
    }


/** @brief Start calculating Coulomb force vector.
 *
 *  A copy of the ion positions is made first. The calculation is either
 *  performed in this thread by calling direct_force, or a set of sub-threads
 *  started that calls split_force. Multithreading is set through a SimParams
 *  parameter coulomb_threads.
 */
void CoulombForce::update() {
    // Initialise vector that will contain force on each ion when we're done.
    force_ = std::vector<Vector3D>(cloud_->number_of_ions());
    // Take a local copy of the ion positions.
    pos_ = std::vector<Vector3D>(cloud_->number_of_ions());
    for (int i = 0; i < pos_.size(); i++) {
        pos_[i] = cloud_->ionVec_[i]->get_pos();
    }

    // Select function from number of threads.
    switch (params_.coulomb_threads) {
        case 0:
            direct_force();
            break;
        case 1:
            m_Thread_ = std::thread(&CoulombForce::direct_force, this);
            break;
        default:
            for (int i_thread = 0; i_thread < params_.coulomb_threads;
                    i_thread++) {
                threads_.push_back(std::thread(
                            &CoulombForce::split_force, this, i_thread));
            }
            break;
    }
}

/** @brief Calculate the Coulomb force for a sub-block of the total number of
 * ions.
 *
 * The Coulomb calculation is split into SimParams::coulomb_threads pieces.
 * This function works on block \i out of the total array of NxN. This function
 * does not make use of force antisymmetry, so may be slower than direct_force
 * for some crystal sizes.
 *
 * @parameter n Sub-block index. Must be less than SimParams::coulomb_threads.
 */
void CoulombForce::split_force(int n) {
    Vector3D r1, r2;
    double r, r3;
    int q1, q2;

    // Determine the column ranges of this sub-block from the NxN array of ion
    // pairs.
    int imin = std::floor(static_cast<double>(n)
            /static_cast<double>(params_.coulomb_threads) * pos_.size());
    int imax = std::floor(
            static_cast<double>(n+1)
            /static_cast<double>(params_.coulomb_threads) * pos_.size());
    // Assert the limits are valid.
    assert(imin >= 0);
    assert(imax <= pos_.size());
    // Sum Coulomb force over all particles.
    for (int i = imin; i < imax; ++i) {
        Vector3D force_local = Vector3D(0.0, 0.0, 0.0);
        r1 = pos_[i];
        q1 = cloud_->ionVec_[i]->get_charge();
        for (int j = 0; j < pos_.size(); ++j) {
            r2 = pos_[j];
            q2 = cloud_->ionVec_[j]->get_charge();

            // Skip the diagonal elements - same ion.
            if (r1 == r2) continue;

            // force term calculation
            r = Vector3D::dist(r1, r2);
            r3 = r*r*r;
            force_local += (r1-r2)/r3*q1*q2;
        }
        // Get a mutex lock before inserting this force into the final vector.
        mutex_.lock();
        force_[i] = force_local;
        mutex_.unlock();
    }
}

/** @brief Generate vector of Coulomb interaction forces in a single thread.
 *
 * This function is used for either zero or one threads and makes use of the
 * antisymmetric nature of the force : F_ji = -F_ij, so only calculates the
 * upper triangle of the NxN array.
 */
void CoulombForce::direct_force() {
    Vector3D r1, r2, f;
    double r, r3;
    int q1, q2;

    // reinitialise forces
    Vector3D null_vec = Vector3D(0.0, 0.0, 0.0);
    std::fill(force_.begin(), force_.end(), null_vec);

    // sum Coulomb force over all particles
    for (int i = 0; i < pos_.size(); ++i) {
        r1 = pos_[i];
        q1 = cloud_->ionVec_[i]->get_charge();
        for (int j = i+1; j < pos_.size(); ++j) {
            r2 = pos_[j];
            q2 = cloud_->ionVec_[j]->get_charge();

            // force term calculation
            r = Vector3D::dist(r1, r2);
            r3 = r*r*r;
            f = (r1-r2)/r3*q1*q2;

            // update sum for ion "i"
            force_[i] += f;
            // update sum for ion "j"
            force_[j] -= f;
        }
    }
}

/** @brief Ensure all threads have finished, and return the force vector.
 */
const std::vector<Vector3D>& CoulombForce::get_force() {
    // Ensure the thread has finished before returning the new force.
    if (params_.coulomb_threads > 1) {
        for (auto& i : threads_)
            if (i.joinable()) i.join();
        threads_.clear();
    } else {
        if (m_Thread_.joinable()) m_Thread_.join();
    }
    return force_;
}
