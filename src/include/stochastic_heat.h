//
//  stochastic_heat.h
//

#ifndef CCMD_Stochastic_heat_h
#define CCMD_Stochastic_heat_h

#include "vector3D.h"
#include "logger.h"
#include <boost/random.hpp>

using namespace boost;

class Stochastic_heat {
     // create a Mersenne twister random number generator
    static mt19937 generator;
    // select Gaussian probability distribution
    normal_distribution<double> norm_dist;
    // bind random number generator to distribution, forming a function
    variate_generator<mt19937&, normal_distribution<> > normal;
    
    uniform_real<double> flat_dist;
    variate_generator<mt19937&, uniform_real<> > flat;
    
    double kick_size;
public:
    Stochastic_heat(int seed)
    : normal(generator, norm_dist), flat_dist(0,1),
    flat(generator, flat_dist), kick_size(0.01) {
        if (seed<0) {
            seed = std::time(0);
        }
        generator.seed(static_cast<unsigned int>(seed));
            
    }
    Vector3D random_kick() 
       { return Vector3D( normal(), normal(), normal())*kick_size; }
    void set_kick_size(double d) { kick_size = d; }
    double get_kick_size() const { return kick_size; }
    
    bool kick_direction (const double p) {
        double num = flat();
        return p>num;}
};

#endif

