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
#include <array>
#include <string.h>
#ifdef _OPENMP
#include <omp.h>
#endif

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
    Vector3D r1, r2, f, tot;
    double r, r3;
    int q1, q2;
    int i,j;
    int cloud_size = cloud_->number_of_ions();

    // Initialise vector that will contain force on each ion when we're done.
    force_ = std::vector<Vector3D>(cloud_->number_of_ions());
    Vector3D null_vec = Vector3D(0.0, 0.0, 0.0);
    std::fill(force_.begin(), force_.end(), null_vec);

#ifdef _OPENMP

#pragma omp parallel default(shared) private(i,j, r1, q1, r2, q2, r3, f, r)
{   
#pragma omp for
#endif

    // sum Coulomb force over all particles
    for (i = 0; i < cloud_size; ++i) {
        Vector3D forces[cloud_size];
        for (j = 0; j < cloud_size; ++j) {
            if (i==j) {
                forces[j] = Vector3D(0,0,0);
            }
            else
            {
                r1 = cloud_->ionVec_[i]->get_pos();
                q1 = cloud_->ionVec_[i]->get_charge();
                r2 = cloud_->ionVec_[j]->get_pos();
                q2 = cloud_->ionVec_[j]->get_charge();

                // force term calculation
                r = Vector3D::dist(r1, r2);
                r3 = r*r*r;
                forces[j] = (r1-r2)/r3*q1*q2;
            }
        }
        Vector3D totalforce = Reduction(forces,cloud_size);
        tot += totalforce;
        force_[i] = totalforce;            
    }
#ifdef _OPENMP
}
#endif    
}


/** @brief Ensure all threads have finished, and return the force vector.
 */
const std::vector<Vector3D>& CoulombForce::get_force() {
    return force_;
}


Vector3D CoulombForce::Reduction(Vector3D x[], int len) {
    Vector3D s;
    if (len < 4) {
        int i = 0;
        for (i=0; i < len; i++) { s += x[i]; }
    }
    else
    {
        int halflen = floor(len/2);
        Vector3D array1[halflen];
        Vector3D array2[len-halflen];
        memcpy(array1, x, halflen * sizeof(Vector3D)); 
        memcpy(array2, &x[halflen], (len-halflen) * sizeof(Vector3D));
        s = Reduction(array1, halflen) + Reduction(array2, len-halflen);
    }
    return s;
}
