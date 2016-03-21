//
//  stochastic_heat.h
//

#ifndef CCMD_Stochastic_heat_h
#define CCMD_Stochastic_heat_h

//#include "dc.h"
#include "vector3D.h"
#include "logger.h"

#include <random>
#include <ctime>
#include <cstdlib>

//Sample Code for usage of mtrnd
//MT::MersenneTwist mtrnd;
//mtrnd.init_genrand(5489UL); //initialize the Mersenne Twister.
//int n = static_cast<int>(floor(mtrnd.genrand_res53()*Nmax)); 

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
    
	
	Vector3D random_sphere_vector() {
		
		const double pi = 3.14159265359;
//	    int tid = omp_get_thread_num()
        double rnumu = flat_dist(generator);
        double rnumv = flat_dist(generator);
      
		double theta = 2 * pi * rnumu;
		double phi = acos((2 * rnumv) -1);
		double rndsphx = cos(theta) * sin(phi);
		double rndsphy = sin(theta) * sin(phi);
		double rndsphz = cos(phi);
		Vector3D spherevector(rndsphx,rndsphy,rndsphz);
		return spherevector;
	}
    
    bool testfscatt(double fscatt){
//        int tid = omp_get_thread_num()
        return (flat_dist(generator) < fscatt);       
    }
};

#endif

