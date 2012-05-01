//
//  ion_cloud.cpp
//  CCMD
//
//  Created by Martin Bell on 09/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ion_cloud.h"


#include "ccmd_sim.h"
#include "ion.h"
#include "vector3D.h"
#include "hist3D.h"

#include <functional>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
//#include <string>

//#include "eigs.h"

// To do:   fix aspect ratio
//          use std::for_each etc

using namespace std;

class Hist3D;
int get_nearest_cube(int n);
std::vector<Vector3D> get_lattice(size_t n);

struct compare_ions_by_mass {
    bool operator()(const Ion* lhs, const Ion* rhs) const 
    {
        return lhs->m() < rhs->m();
    }
};

struct position_ions {
    Ion* operator() (Ion* ion, const Vector3D& r) const
    {
        ion->set_position(r);
        return ion;
    }
};

Ion_cloud::Ion_cloud(const Ion_trap& ion_trap, const Cloud_params& params)
    :  trap(&ion_trap), cloud_params(&params)  
{

    // Loop over ion types to initialise ion cloud
    typedef map<Ion_type*,int>::const_iterator map_itr;
    for (map_itr p=cloud_params->ion_numbers.begin(); 
                 p!=cloud_params->ion_numbers.end();
               ++p) {
        Ion_type* type_ptr(p->first);
        int number_of_type(p->second);
        
        // Allocate ions
        // Construct ions with using *trap to ensure that changes to
        // ion trap parameters propagate to the ions
        try {
            for (int i=0; i<number_of_type; ++i) {
                if (type_ptr->is_laser_cooled) {
                    add_ion(new Lasercooled_ion(*trap, *type_ptr));
                } else {
                    add_ion(new Trapped_ion(*trap, *type_ptr));
                }
            }
        } catch (bad_alloc& ba) {
            ostringstream error_msg;
            error_msg << "bad_alloc in ion cloud constructor: " << ba.what();
            throw runtime_error( error_msg.str() );
        }
        
        // Sort ions by mass
        sort(ion_vec.begin(),ion_vec.end(), compare_ions_by_mass() );
        
        // Generate initial positions
        vector<Vector3D> lattice = get_lattice( number_of_ions() );
        
        // Move ions into position
        transform(ion_vec.begin(), ion_vec.end(),
                  lattice.begin(), ion_vec.begin(),
                  position_ions() );
        
        // Move cloud centre to the origin
        Vector3D to_origin = -get_cloud_centre( *this );
        move_centre(to_origin);
    }
}

void Ion_cloud::update_params()
{
    // Update types for ions
    for_each(ion_vec.begin(),
             ion_vec.end(), 
             mem_fun(&Ion::update_ion_type));
    
    // Update trap parameters used by ions
   for_each(ion_vec.begin(),
            ion_vec.end(), 
            mem_fun(&Ion::update_trap_force));
}

Ion_cloud::~Ion_cloud()
{
    typedef std::vector<Ion*>::iterator ion_itr;
    for (ion_itr ion=ion_vec.begin(); ion<ion_vec.end(); ++ion) {
        delete *ion;
    }
    ion_vec.clear();
}

int get_nearest_cube(int n)
{
    // Returns number which when cubed is closest to n 
	int min_cube = 1;
	int max_cube = 1;
	int i = 1;
	while (max_cube <=n) {
		min_cube = max_cube;
		++i;
		max_cube = i*i*i;
	}
    return (n-min_cube) < (max_cube-n) ? i-1 : i;

}

Vector3D Ion_cloud::get_cloud_centre(const Ion_cloud& ) const
{
    // 	Unweighted geometric centre of ion cloud
	Vector3D centre;
    double n_ions = ion_vec.size();
    
	for (int i=0; i<n_ions; ++i)
		centre += ion_vec[i]->r();
    
	centre /= n_ions;
	return centre;
}

void Ion_cloud::move_centre(const Vector3D& v)
{
	// Place cloud centre at origin
	for (int i=0; i<ion_vec.size(); ++i) {
		ion_vec[i]->move( v );
	}
}

std::ostream& operator<<(std::ostream& os, const Ion_cloud& ic)
{
    static bool first_call = true;
    
    if (first_call) {
        std::string header_space(7,' ');
        std::string header_line(33,'-');
        std::string header = " Ion         x       y       z";
    
        os << header_space << header << std::endl;
        os << header_space << header_line << std::endl;
    
        first_call = false;
    }
    
	os.setf(std::ios::fixed);
	os.precision(6);
	os << setiosflags( std::ios::right );
    
	enum XYZ{x,y,z};
	for (int i=0; i<ic.ion_vec.size(); ++i) {
		os << std::setw(10) << i+1 << '\t';
        
		os << std::setw(7) << ic.ion_vec[i]->r(x) << ' '
            << std::setw(7) << ic.ion_vec[i]->r(y) << ' '
            << std::setw(7) << ic.ion_vec[i]->r(z) << std::endl;
	}
    
	//os << header_space << header_line << std::endl;
	os << std::endl;
    
	return os;
}


void Ion_cloud::drift(double t)
{
    for (int i=0; i<ion_vec.size(); ++i) {
        ion_vec[i]->drift(t);
    }
}

void Ion_cloud::kick(double t)
{
    for (int i=0; i<ion_vec.size(); ++i) {
        ion_vec[i]->kick(t);;
    }
}

void Ion_cloud::kick(double t, const std::vector<Vector3D>& f)
{
    for (int i=0; i<ion_vec.size(); ++i) {
        ion_vec[i]->kick(t, f[i] );
    }
}
 
void Ion_cloud::velocity_scale(double dt)
{
    for (int i=0; i<ion_vec.size(); ++i) {
        ion_vec[i]->velocity_scale(dt);
    }
}

void Ion_cloud::heat(double t)
{
    for (int i=0; i<ion_vec.size(); ++i) {
        ion_vec[i]->heat(t);;
    }
}

double Ion_cloud::kinetic_energy() const
{
    double e = 0;
    double m;
    for (int i=0; i<ion_vec.size(); ++i) {
        m = ion_vec[i]->mass;
        e += 0.5*m*( ion_vec[i]->vel.norm_sq() );
    }
    return e;
}

double Ion_cloud::coulomb_energy() const
{
    Vector3D r1,r2;
    int q1,q2;
    double e = 0;
    double r;
    
    for (int i=0; i<ion_vec.size(); ++i) {
        r1 = ion_vec[i]->pos;
        q1 = ion_vec[i]->charge;
        for (int j=i+1; j<ion_vec.size(); ++j) {
            r2 = ion_vec[j]->pos;
            q2 = ion_vec[j]->charge;
            
            if (r1 != r2) {
                r = Vector3D::dist(r1,r2);
            } else {
                std::cout << i << ' ' << r1 << ' ' << j << ' ' << r2 << '\n';
                std::abort();
            }
        e += q1*q2/r;
        }
    }
    return e;
}

void Ion_cloud::update_position_histogram(Hist3D& h) const
{
    for (int i=0; i<ion_vec.size(); ++i) {
        if (ion_vec[i]->visible) {
            Vector3D posn = ion_vec[i]->r();
            Vector3D rotated_pos;
            rotated_pos.x = (posn.x+posn.y)/sqrt(2.0);
            rotated_pos.y = (posn.x-posn.y)/sqrt(2.0);
            rotated_pos.z = posn.z;
            h.update( rotated_pos );
            //h.update( ion_vec[i]->pos );
         }
    }
    return;
}

void Ion_cloud::scale(double scale_factor)
{
    for (int i=0; i<ion_vec.size(); ++i) {
        ion_vec[i]->pos = ion_vec[i]->pos*scale_factor;
    }
}

std::vector<Vector3D> get_lattice(size_t n)
{
    int side = ceil( pow(n,1.0/3.0) );
    std::vector<Vector3D> lattice;

	Vector3D grid_pos;
    double scale = 2.0;
    
    Vector3D to_centre = Vector3D(0.5,0.5,0.5)*side*scale;
	// Move ions to cubic lattice sites 
	for (int i=0; i<pow(side,3.0); ++i) {
		grid_pos.x =               i%side;
		grid_pos.y =        (i/side)%side;
		grid_pos.z = (i/(side*side))%side;
        
        grid_pos *= scale;
        grid_pos -= to_centre;
        lattice.push_back(grid_pos);
    }
    std::sort(lattice.begin(), lattice.end());
    lattice.resize(n);
    
    return lattice;
}

double Ion_cloud::aspect_ratio() const
{
    using namespace std;
    
    
    double x_max = 0.0;
    double y_max = 0.0;
    double z_max = 0.0;

    for (int i=0; i<ion_vec.size(); ++i) {
        x_max = max(x_max, abs(ion_vec[i]->pos.x) );
        y_max = max(y_max, abs(ion_vec[i]->pos.y) );
        z_max = max(z_max, abs(ion_vec[i]->pos.z) );

    };
    return z_max/max(x_max,y_max); 
    
/*
    double w[3];
    double x,y,z = 0.0;
    
    // Calculate moment of inertia tensor
    double I[9] = {0,0,0,0,0,0,0,0,0};
    for (int i=0; i<ion_vec.size(); ++i) {
        x = ion_vec[i]->pos.x;
        y = ion_vec[i]->pos.y;
        z = ion_vec[i]->pos.z;
        I[0] += y*y + z*z; 
        I[1] += -x*y; 
        I[2] += -x*z; 
        I[4] += x*x + z*z; 
        I[5] += -y*z; 
        I[8] += x*x + y*y;
    };
    I[3] = I[1];
    I[6] = I[2];
    I[7] = I[5];
    
    get_eigenvalues(I,w,3);
    
    //for (int i=0; i<3; ++i) std::cout << w[i] << std::endl;
    
    return w[2]/w[0];
*/
}

Vector3D Ion_cloud::ion_position(size_t ion_index) const
{
    if (ion_index < ion_vec.size())
        return ion_vec[ion_index]->r();
    else
        throw;
}

Vector3D Ion_cloud::ion_velocity(size_t ion_index) const
{
    if (ion_index < ion_vec.size())
        return ion_vec[ion_index]->v();
    else
        throw;
}

int Ion_cloud::ion_charge(size_t ion_index) const
{
    return ion_vec[ion_index]->q();
}

double Ion_cloud::ion_mass(size_t ion_index) const
{
    return ion_vec[ion_index]->m();
}

std::string Ion_cloud::ion_name(size_t ion_index) const
{
    return ion_vec[ion_index]->name();
}

std::string Ion_cloud::ion_formula(size_t ion_index) const
{
    return ion_vec[ion_index]->formula();
}

std::string Ion_cloud::ion_color(size_t ion_index) const
{
    return ion_vec[ion_index]->color();
}

void Ion_cloud::set_ion_position(size_t ion_index, const Vector3D& r)
{
    if (ion_index < ion_vec.size()) {
        ion_vec[ion_index]->set_position(r);
        return;
    }
    else
        throw;
}

void Ion_cloud::set_ion_velocity(size_t ion_index, const Vector3D& v)
{
    if (ion_index < ion_vec.size()) {
        ion_vec[ion_index]->set_velocity(v);
        return;
    }
    else
        throw;
}


