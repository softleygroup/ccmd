/** 
 * @file ion.h
 * @brief Declaration of all Ion base and derived classes.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */
#ifndef INCLUDE_ION_H_
#define INCLUDE_ION_H_

#include <memory>
#include <string>

#include "ccmdsim.h"
#include "ionhistogram.h"
#include "iontrap.h"
#include "stats.h"
#include "stochastic_heat.h"

class Vector3D;
class IonHistogram;
template <class T> class Stats;

class Ion {
 public:
    explicit Ion(const IonType& type, const LaserParams& lp);
    virtual ~Ion() {}
    // Shift ion position.
    void move(const Vector3D &move_va) { pos_ += move_va; }

    // Base class functions
    void drift(double dt);
    void recordKE(IonHistogram_ptr ionHistogram, const TrapParams& trapParams) const;
    void updateStats();
    void update_from(const IonType& from);

    // These should only be called once on initialising the ion;
    void set_position(const Vector3D &r) { pos_ = r; }
    void set_velocity(const Vector3D &v) { vel_ = v; }
    void set_ElecState(const int &ES)    { ElecState = ES;}

    // velocity modifying functions
    // Subclasses must provide their own force calculation
    virtual void kick(double dt) = 0;
    virtual void kick(double dt, const Vector3D &f);
    virtual void velocity_scale(double dt) {}
    virtual void heat(double dt) {}

    // accessor functions
    const IonType& get_type()               const {return ionType_; }
    std::string name()                      const {return ionType_.name;}
    std::string formula()                   const {return ionType_.formula;}
    const Vector3D& get_pos()               const {return pos_;}
    const Vector3D& get_vel()               const {return vel_;}
    const int& get_state()                  const {return ElecState;}
    double get_mass()                       const {return ionType_.mass;}
    double get_charge()                     const {return ionType_.charge;}
    const Stats<Vector3D> get_posStats()    const {return posStats_;}
    const Stats<double> get_velStats()      const {return velStats_;}

    Ion(const Ion&) = delete;
    const Ion& operator=(const Ion&) = delete;

 protected:
    const IonType& ionType_;
    const LaserParams& lp_;
    Vector3D pos_;
    Vector3D vel_;
    int ElecState;		   ///< Electronic energy level, 0 == Ground State, 1 == Excited State, (2 == Dark State)
 
    // Store statistics for this ion
    Stats<Vector3D> posStats_;
    Stats<double> velStats_;
};


class TrappedIon : public Ion {
 public:
    TrappedIon(const IonTrap_ptr trap, const IonType& type, const LaserParams& lp);
    ~TrappedIon() {}

    virtual void kick(double dt);
    virtual void velocity_scale(double dt) {}
    
    TrappedIon(const TrappedIon&) = delete;
    const TrappedIon& operator=(const TrappedIon&) = delete;

 private:
    const IonTrap_ptr trap_;
};


class LaserCooledIon : public TrappedIon {
 public:
    LaserCooledIon(const IonTrap_ptr ion_trap, const TrapParams& trap_params, const IonType& type, const SimParams& sp, const LaserParams& lp);
    ~LaserCooledIon() {}

    void kick(double dt);
    void velocity_scale(double dt);
    void heat(double dt);
	Vector3D Emit(double dt);
	Vector3D Absorb(double dt);

    LaserCooledIon(const LaserCooledIon&) = delete;
    const LaserCooledIon& operator=(const LaserCooledIon&) = delete;
 private:
	double fscatt(double LaserDirection);
	Vector3D isoEmit();
    Stochastic_heat heater_;
    Vector3D get_friction() const;
    const TrapParams& trap_params; 
};

/// Ion pointer type.
typedef std::shared_ptr<Ion> Ion_ptr;

#endif  // INCLUDE_ION_H_
