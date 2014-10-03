//
//  stochastic_heat.h
//

#ifndef CCMD_Stochastic_heat_h
#define CCMD_Stochastic_heat_h

#include "vector3D.h"
#include "logger.h"
//#include <boost/random.hpp>
//
#include <random>

//using namespace boost;

class Stochastic_heat {
     // create a Mersenne twister random number generator
    static std::mt19937 generator;
    // select Gaussian probability distribution
    std::normal_distribution<double> norm_dist;
    // bind random number generator to distribution, forming a function
    //std::variate_generator<mt19937&, std::normal_distribution<> > normal;
    
    std::uniform_real_distribution<double> flat_dist;
    //std::variate_generator<mt19937&, uniform_real<> > flat;
    
    double kick_size;
public:
    Stochastic_heat(int seed)
        :norm_dist(0.0,1.0), flat_dist(0, 1), kick_size(0.01) {
    //: normal(generator, norm_dist), flat_dist(0,1), flat(generator, flat_dist)
        if (seed<0) {
            seed = (int)std::time(0);
        }
        generator.seed(static_cast<unsigned int>(seed));
            
    }
    Vector3D random_kick() 
       //{ return Vector3D( normal(), normal(), normal())*kick_size; }
       { return Vector3D(norm_dist(generator), norm_dist(generator), norm_dist(generator))*kick_size; }
    void set_kick_size(double d) { kick_size = d; }
    double get_kick_size() const { return kick_size; }
    
    bool kick_direction (const double p) {
        double num = flat_dist(generator);
        return p>num;}
    
    Stochastic_heat(const Stochastic_heat&) = delete;
    const Stochastic_heat& operator=(const Stochastic_heat&) = delete;
};

#endif

