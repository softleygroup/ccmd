//
//  stochastic_heat.h
//

#ifndef CCMD_Stochastic_heat_h
#define CCMD_Stochastic_heat_h

#include "vector3D.h"
#include <boost/random.hpp>

using namespace boost;

class Stochastic_heat {
    // create a Mersenne twister random number generator
    static mt19937 generator;
    // select Gaussian probability distribution
    normal_distribution<double> norm_dist;
    // bind random number generator to distribution, forming a function
    variate_generator<mt19937&, normal_distribution<> > normal;
    
    double kick_size;
public:
    Stochastic_heat() : normal(generator, norm_dist), kick_size(0.01) { }
    Vector3D random_kick() 
       { return Vector3D( normal(), normal(), normal())*kick_size; }
    void set_kick_size(double d) { kick_size = d; }
    double get_kick_size() const { return kick_size; }
};

#endif

