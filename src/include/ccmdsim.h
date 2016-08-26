/**
 * @file ccmdsim.d
 * @brief Declaration of simulation parameter classes.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#ifndef INCLUDE_CCMDSIM_H_
#define INCLUDE_CCMDSIM_H_

#include <list>
#include <string>

class Logger;

class TrapParams {
 public:
    explicit TrapParams(const std::string& file_name);
    /// Enumeration of avaliable trap types.
    enum Waveform {cosine, digital, waveform, cosine_decay, twofreq};
    Waveform wave;              ///< Type of RF waveform applied to trap
    double freq;                ///< Trap RF frequency            (Hz)

    // Trap voltage parameters
    double v_rf;                ///< Amplitude of RF voltage      (V)
    double v_end;               ///< Endcap electrode voltage     (V)

    // Trap geometry parameters
    double eta;                 ///< Trap geometry parameter
    double r0;                  ///< Electrode radius             (m)
    double z0;                  ///< Central electrode length     (m)

    int     seed;

    /** Waveform duty cycle. Reused as time constant for exponential decay
     * cosine trap
     */
    double tau;
    double deltaT;     ///< wait time until exponential decay for decaying trap
    std::string waveformFile;   ///< File containing waveform data.

    double freq_mult;           ///< Multiplier for second waveform

    double length_scale;    ///< Derived simulation length scale.
    double time_scale;      ///< Derived simulation time scale.
    double energy_scale;    ///< Derived simulation energy scale.

    TrapParams(const TrapParams&) = delete;
    const TrapParams& operator= (const TrapParams&) = delete;
};


class IntegrationParams {
 public:
    explicit IntegrationParams(const std::string& file_name);

    double time_step;          ///< Time interval, Units are 2/Omega = 1/(pi*f))
    int steps_per_period;       ///<< Number of time steps taken in 1 RF period.

    // respa_steps == RESPA algorithm inner loop number
    //
    // respa_steps == 1 is equivalent to a Velocity Verlet
    // or Leapfrog algorithm
    //
    // See: M. Tuckerman, B. J. Berne and G. J. Martyna
    //      J. Chem. Phys. 92, 1990 (1992)
    //

    /// Number of RESPA inner loop steps between Coulomb force updates
    int respa_steps;
    int cool_steps;          ///< Number of timesteps for cooling stage.
    int hist_steps;          ///< Number of steps to collect statistics.

    IntegrationParams(const IntegrationParams&) = delete;
    const IntegrationParams& operator=(const IntegrationParams&) = delete;
};


class IonType {
 public:
    IonType() = default;
    int number;            ///< Number of these in the trap.
    std::string name;      ///< Name to call ion.
    std::string formula;   ///< Chemical formula.

    // Ion physical properties
    double mass;           ///< Mass of ion in atomic mass units.
    int charge;            ///< Charge of ion in electron charge units.
    double beta;           ///< Velocity damping coefficient for laser cooling.
    double recoil;         ///< Magnitude of stochastic heating.
    int seed;              ///< Seed for heating random number generator.
    double direction;      ///< Ratio of left-to-right cooling intensity.

    bool is_laser_cooled;
    bool is_heated;

    float A21;			   ///< Einstein A21 coefficient
    
    // IonType(IonType&&) = default;
    // IonType(const IonType& ) = delete;
    // const IonType& operator=(const IntegrationParams&) = delete;
    // IonType& operator=(IntegrationParams&&) = default;
};


class CloudParams {
 public:
    explicit CloudParams(const std::string& file_name);

    /// List to hold an IonType for each ion type used.
    std::list<IonType> ion_type_list;
};

/*
class SwapParams {
    public:
        SwapParams(const std::string& file_name, const CloudParams& p);
        bool do_swap;
        IonType_ptr from;
        IonType_ptr to;
        double p;

        SwapParams(const SwapParams&) = delete;
        const SwapParams& operator=(const SwapParams&) = delete;
};
*/

class MicroscopeParams {
 public:
    explicit MicroscopeParams(const std::string& file_name);

    bool make_image;                 // whether to run image creation or not
    double pixels_to_distance;      // conversion from microns to pixels
    double w0;                      // blur size
    double z0;                      // depth of field
    int nz;                      // Number of pixels in z direction
    int nx;                      // Number of pixels in x direction
//    int zmin;                       // start plane
//    int zmax;                       // end plane

    MicroscopeParams(const MicroscopeParams&) = delete;
    const MicroscopeParams& operator=(const MicroscopeParams&) = delete;
};


class SimParams {
 public:
    explicit SimParams(const std::string& file_name);

    /** Number of threads to use in CoulombForce calculation. Default 0. */
    int coulomb_threads;
    /** Seed for random number generator used by stochastic_heat. -1 chooses
     seed from system clock and will be different for every run. Default -1. */
    int random_seed;

 private:
    SimParams(const SimParams& ) = delete;
    const SimParams& operator=(const SimParams&) = delete;
};

class LaserParams {
 public:
	explicit LaserParams(const std::string& file_name);

	// Wavelength of laser
	float wavelength;
	// Detuning of laser
	float delta;
	// I/Isat
	float IdIsat;
	
	private:
    LaserParams(const LaserParams& ) = delete;
    const LaserParams& operator=(const LaserParams&) = delete;
};

#endif  // INCLUDE_CCMDSIM_H_
