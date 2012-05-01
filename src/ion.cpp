
#include "ion.h"
#include "ccmd_sim.h"

Ion::Ion(const Ion_type& type) 
    : ion_type(&type) 
{
    update_ion_type();
}

void Ion::update_ion_type()
{
    mass = ion_type->mass;
    charge = ion_type->charge;
    visible = ion_type->visible;
}


void Ion::drift(double dt)
{
    pos += vel*dt;
}

inline void Ion::kick(double dt,const Vector3D& f)
{
    double time_over_mass = dt/mass;
    vel += f*time_over_mass;
}

// *** Modify or remove these functions ***
double Ion::r(char coord) const
{
	switch (coord) {
		case 'x': return pos[0];
		case 'y': return pos[1];
		case 'z': return pos[2];
		default: 
			// FUTURE: Throw an exception
			return 999.9;
	}
} 

double Ion::v(char coord) const
{
	switch (coord) {
		case 'x': return vel[0];
		case 'y': return vel[1];
		case 'z': return vel[2];
		default: 
			// FUTURE: Throw an exception
			return 999.9;
	}
} 

std::string Ion::name() const 
{ 
    return ion_type->name; 
}

std::string Ion::formula() const 
{ 
    return ion_type->formula; 
}

std::string Ion::color() const 
{ 
    return ion_type->color; 
}

