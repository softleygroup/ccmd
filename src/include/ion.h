#ifndef CCMD_ion_h
#define CCMD_ion_h

#include "stochastic_heat.h"

class Vector3D;
class Ion_trap;
class Ion_type;

// Base class providing interface for ion types
class Ion {
public:
    Ion(const Ion_type& type); 

    virtual ~Ion() {}
	
    virtual void update_ion_type();
    virtual void update_trap_force() {}

    // shifts ion position
    void move(const Vector3D& move_va) { pos += move_va; }
    
    // free flight 
    void drift(double dt);

    // velocity modifying functions
    virtual void kick(double dt) {}
    virtual void kick(double dt, const Vector3D& f);
    virtual void velocity_scale(double dt) {}
    virtual void heat(double dt) {}

    void set_position(const Vector3D& r) { pos = r; }
    void set_velocity(const Vector3D& v) { vel = v; }

    // accessor functions
    Vector3D r() const { return pos; } 
    double r(const char ch) const; 
    double r(int i) const { return pos[i]; } 
    
    Vector3D v() const { return vel; } 
    double v(char coord) const;
    double v(int i) const { return vel[i]; } 
    
    double m() const {return mass;}
    int q() const {return charge;}
    std::string name() const;
    std::string formula() const;
    std::string color() const;

    const Ion_type& get_type() const {return *ion_type; }
    
protected:  
    const Ion_type* ion_type;
    Vector3D pos;
    Vector3D vel;
    double mass;
    int charge;
    bool visible;
    
private:
    Vector3D newion_pos();
    Vector3D newion_vel();

    friend class Ion_cloud;
};

//
// Trapped-but-not-laser-cooled ion class. This class is used to model
// sympathetically-cooled ions
//
class Trapped_ion : public Ion {
public:
    virtual void kick(double dt);
    virtual void velocity_scale(double dt) {}
    void update_trap_force();
protected:    
    Trapped_ion(const Ion_trap& ion_trap,const Ion_type& type); 
    ~Trapped_ion() {}
    
    virtual void update_ion_type();
private:
    const Ion_trap* trap;
    Vector3D trap_omega;
    double plasma_omega; 
    double a_trap;
    double q_trap;

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

    virtual void update_ion_type();
private:
    double beta;
    
    // photon recoil heating
    Stochastic_heat heater;

    Vector3D get_friction() const;

    friend class Lasercooled_ion_cloud;
    friend class Ion_cloud;
};


#endif

