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
 * This function makes use of the antisymmetric nature of the force : F_ji =
 * -F_ij, so only calculates the upper triangle of the NxN array.
 */
void CoulombForce::update() {
    Vector3D r1, r2, f;
    double r, r3;
    int q1, q2;
    int cloud_size = cloud_->number_of_ions();

    // Initialise vector that will contain force on each ion when we're done.
    force_ = std::vector<Vector3D>(cloud_->number_of_ions());
    Vector3D null_vec = Vector3D(0.0, 0.0, 0.0);
    std::fill(force_.begin(), force_.end(), null_vec);

    // sum Coulomb force over all particles
    for (int i = 0; i < cloud_size; ++i) {
        r1 = cloud_->ionVec_[i]->get_pos();
        q1 = cloud_->ionVec_[i]->get_charge();
        for (int j = i+1; j < cloud_size; ++j) {
            r2 = cloud_->ionVec_[j]->get_pos();
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
    return force_;
}
