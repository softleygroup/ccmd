//
//  ccmd_sim.h
//

//
// These classes contain parameters which are used to define the
// molecular dynamics simulation.
//
// Improvments: use XML to store/load parameters
//              simulation parent class
//

#ifndef CCMD_ccmd_sim_h
#define CCMD_ccmd_sim_h

#include <list>
#include <string>

class Logger;

class TrapParams {
 public:
    explicit TrapParams(const std::string& file_name);

    // Radiofrequency parameters

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


    double tau;                 ///< Waveform duty cycle.
                                ///< reused as time constant for exponential decay cosine trap
    double deltaT;              ///< wait time until exponential decay for decaying trap
    std::string waveformFile;   ///< File containing waveform data.

    double freq_mult;           ///< Multiplier for second waveform

    TrapParams(const TrapParams&) = delete;
    const TrapParams& operator= (const TrapParams&) = delete;
};


class IntegrationParams {
 public:
    explicit IntegrationParams(const std::string& file_name);

    double time_step;           ///< Time interval, Units are 2/Omega = 1/(pi*f))
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


class Ion_type {
 public:
    Ion_type() = default;
    int number;                 ///< Number of these in the trap.
    std::string name;           ///< Name to call ion.
    std::string formula;        ///< Chemical formula.

    // Ion physical properties
    double mass;                ///< Mass of ion in atomic mass units.
    int charge;                 ///< Charge of ion in electron charge units.
    double beta;                ///< Velocity damping coefficient for laser cooling.
    double recoil;              ///< Magnitude of stochastic heating.
    int seed;                   ///< Seed for heating random number generator.
    double direction;           ///< Ratio of left-to-right cooling intensity.

    bool is_laser_cooled;
    bool is_heated;

    // Ion_type(Ion_type&&) = default;
    // Ion_type(const Ion_type& ) = delete;
    // const Ion_type& operator=(const IntegrationParams&) = delete;
    // Ion_type& operator=(IntegrationParams&&) = default;
};

// typedef boost::shared_ptr<Ion_type> Ion_type_ptr;


class CloudParams {
 public:
    explicit CloudParams(const std::string& file_name);

    /// List to hold an Ion_type for each ion type used.
    std::list<Ion_type> ionTypeList;
};

/*
class Swap_params {
    public:
        Swap_params(const std::string& file_name, const CloudParams& p);
        bool do_swap;
        Ion_type_ptr from;
        Ion_type_ptr to;
        double p;

        Swap_params(const Swap_params&) = delete;
        const Swap_params& operator=(const Swap_params&) = delete;
};
*/

class MicroscopeParams {
 public:
    explicit MicroscopeParams(const std::string& file_name);

    bool makeimage;                 // whether to run image creation or not
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

    /** Number of threads to use in coulomb_force calculation. Default 0. */
    int coulomb_threads;
    /** Seed for random number generator used by stochastic_heat. -1 chooses
     seed from system clock and will be different for every run. Default -1. */
    int random_seed;

 private:
    SimParams(const SimParams& ) = delete;
    const SimParams& operator=(const SimParams&) = delete;
};


#endif
