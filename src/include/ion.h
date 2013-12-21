#ifndef CCMD_ion_h
#define CCMD_ion_h

#include <boost/shared_ptr.hpp>

#include "stochastic_heat.h"
#include "Stats.h"
#include "ccmd_sim.h"

class Vector3D;
class Ion_trap;
class IonHistogram;
template <class T> class Stats;


class Ion {
public:
    Ion(const Ion_type& type);
    virtual ~Ion() {}
    // shifts ion position
    void move(const Vector3D& move_va) { pos += move_va; }
    
    // free flight 
    void drift(const double dt);

    // velocity modifying functions
    // Subclasses must provide their own force calculation
    virtual void kick(double dt) = 0;
    virtual void kick(double dt, const Vector3D& f);
    virtual void velocity_scale(double dt) {}
    virtual void heat(double dt) {}

    // These should only be called once on initialising the ion;
    void set_position(const Vector3D& r) { pos = r; }
    void set_velocity(const Vector3D& v) { vel = v; }

    void recordKE(IonHistogram& ionHistogram) const;
    
    // accessor functions
    const Ion_type& get_type() const {return *ion_type; }
    std::string name() const {return ion_type->name;}
    std::string formula() const {return ion_type->formula;}
    const Vector3D& get_pos() const {return pos;}
    const Vector3D& get_vel() const {return vel;}
    const double get_mass() const {return mass;}
    const double get_charge() const {return charge;}
    
    void updateStats();
    void resetStats();
protected:
    const Ion_type* ion_type;
    Vector3D pos;
    Vector3D vel;
    double mass;
    int charge;
    
    
    // Ion statistics
    Stats<Vector3D> posStats;
    Stats<Vector3D> velStats;
    
//    friend class Ion_cloud;
};


class Trapped_ion : public Ion {
public:
    virtual void kick(double dt);
    virtual void velocity_scale(double dt) {}
//    void update_trap_force();
protected:    
    Trapped_ion(const Ion_trap& ion_trap,const Ion_type& type); 
    ~Trapped_ion() {}
private:
    const Ion_trap* trap;
//    Vector3D trap_omega;
//    double plasma_omega; 
//    double a_trap;
//    double q_trap;

    friend class Trapped_ion_cloud;
    friend class Ion_cloud;
};

//
// Laser-cooled ion class. Heating term arising from photon
// recoil is implemented as a Langevin process with a Gaussian
// momentum distribution.
//
class Lasercooled_ion : public Trapped_ion {
public:
    void kick(double dt);
    void velocity_scale(double dt);
    void heat(double dt);
protected:
    Lasercooled_ion(const Ion_trap& ion_trap,const Ion_type& type); 
    ~Lasercooled_ion() {}
private:
    double beta;
    
    // photon recoil heating
    Stochastic_heat heater;

    Vector3D get_friction() const;

    friend class Lasercooled_ion_cloud;
    friend class Ion_cloud;
};

/// Ion pointer type.
typedef boost::shared_ptr<Ion> ion_ptr;

#endif

