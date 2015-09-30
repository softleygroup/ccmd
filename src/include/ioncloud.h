/** 
 * @file ioncloud.h
 * @brief Class declaration for a cloud of ions.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#ifndef INCLUDE_IONCLOUD_H_
#define INCLUDE_IONCLOUD_H_

#include <memory>
#include <string>
#include <vector>

#include "ion.h"
#include "iontrap.h"

class ImageCollection;
class IonHistogram;
class IonTrap;
class CloudParams;
class TrapParams;
class Vector3D;
class IonType;

typedef std::vector<Ion_ptr> Ion_ptr_vector;

class IonCloud {
 public:
    IonCloud(const IonTrap_ptr ion_trap, const CloudParams& cp,
            const SimParams& sp, const TrapParams& tp);
    ~IonCloud();

    void drift(double t);
    void kick(double t);
    void kick(double t, const std::vector<Vector3D>& fc);
    void heat(double t);
    void velocity_scale(double dt);
    void updateStats();

    double coulomb_energy() const;
    double kinetic_energy() const;
    double total_energy() const;

    double aspect_ratio() const;

    size_t number_of_ions() const { return ionVec_.size(); }
    const Ion_ptr_vector& get_ions() const { return ionVec_; }

    void update_position_histogram(ImageCollection&) const;
    void update_energy_histogram(IonHistogram_ptr) const;

    void saveStats(const std::string basePath, const double length_scale,
                   const double time_scale) const;
    void savePos(const std::string basePath, const double length_scale,
                   const double time_scale) const;

    void swap_first(const IonType& from, const IonType& to);

    IonCloud(const IonCloud&) = delete;
    const IonCloud& operator=(const IonCloud&) = delete;

 private:
    /** @brief Keep a reference to the parameters. */
    const CloudParams& cloudParams_;
    /** @brief Keep a reference to the parameters. */
    const SimParams& simParams_;
    const TrapParams& trapParams_;
    /** A list of pointers to the ion objects. */
    Ion_ptr_vector ionVec_;

    Vector3D get_cloud_centre() const;
    void move_centre(const Vector3D& v);
    static std::vector<Vector3D> get_lattice(size_t n);
    static int get_nearest_cube(int n);

    /** @brief CoulombForce needs direct access to the list of ions. */
    friend class CoulombForce;
};

typedef std::shared_ptr<IonCloud> IonCloud_ptr;

#endif  // INCLUDE_IONCLOUD_H_
