# CCMD (Coulomb Crystal Molecular Dynamics)

The CCMD (Coulomb Crystal Molecular Dynamics) code provides a framework for 
performing molecular dynamics simulations of trapped ion clouds. 

Requires the Boost library for parsing the input file and generating images.

## Compiling

### Linux:

Running make in src/ compliles the executable in bin/linux

### Mac OS X:

XCode project file in build/macosx 

## Running

The ccmd executable expects one command-line argument: the directory in which to 
run. This directory needs to hold the input file named `trap.info`; all output 
files will be written to this directory, overwriting any previous files. This is 
done so that the output files are automatically documented by the input file 
that generated them.

An example input file is in the directory `config`.

## Input file format


All the parameters are stored in a text input file following the Boost info
file format (see documentation at the [boost website][info_doc]), which is
parsed as a property tree. The base branches are `trap` containing ion trap
parameters, `integrator` containing integrator parameters, `ionnumber`
containing the number of each type of ion in the simulation and `iontype`
which defines the physical properties of the ions.

Note: the names in the input file are not case sensitive.

### Example input file

    trap {
        vrf     300.0
        vend    2.0
        eta     0.244
        r0      3.5e-3
        z0      2.75e-3
        freq    3.85e6
        type    {
            name    digital
            tau     0.1
        }
    }
    integrator {
        stepsPerPeriod 20
        respasteps  50       ; Respa inner loop steps
        coolperiods 2000
        histperiods   200
    }
    image {
        makeimage   true
        scale       1; 2.5     ; Image scaling in pixels per micron
        blur        5.0     ; Blur radius in microns
        dof         50.0    ; Depth of field in microns
        nz          640     ; Number of pixels in z axis
        nx          480     ; Number of pixels in x axis
    }
    simulation {
        threads     0
        seed        -1
    }
    ionnumbers {
        Ca      100
        CaF     50
    }
    iontype {
        CaF {
            name        CalciumFluoride
            mass        59.0
            charge      1
        }
        Ca {
            name        Calcium
            mass        40.0
            charge      1
            lasercooled true
            beta        0.6
            heating     true
            recoil      0.01
            direction   0.5
        }
    }


### Trap parameters

In the block `trap`

Parameter   | Description
------------|------------------------------------------------------------
`vrf`      | Peak-to-peak RF voltage
`vend`     | End-cap voltage
`eta`      | Geometric non-ideality factor
`r0`       | Distance between quadrupole rod centres
`z0`       | Distance between endcaps
`freq`     | RF frequency
`type`     | (_specifies trap type sub-branch_)
`name`     | Trap type name (cosine / digital)
`tau`      | (digital only) waveform duty cycle


### Cloud parameters

Ions are described as follows: a branch named `ionnumbers` lists one ion
formula followed by the number of this ion in the simulation. We then look
within the `iontype` branch for a sub-branch with the same name as each
formula given in the `ionnumbers` branch. The physical properties of each
ion in use is then loaded.

The definition is arranged in two parts with the number defined separately
to allow the definition of ion physical properties to be easily copied and
pasted between input files. There is no penalty for having more ions defined
than used, the function only looks for definitions matching those listed in
the `ionnumbers` branch.

# Avaliable parameters #
## iontype ##

The `lasercooled` and `heating` parameters are optional, and are taken
as `false` if not specified (e.g. for CaF in the example below.) the other
parameters marked optional require heating or cooling and are ignored if
these are false.


Parameter      | Description
---------------|------------------------------------------------------------
`name`         |   Full name of the ion, spaces will be stripped
`mass`         |   Molecular mass in AMU
`charge`       |   Charge in units of fundamental charge (+1, +2, ...)
`lasercooled`  |   Is this ion lasercooled (true/false) (**optional**)
`direction`    |   Weighting of laser cooling along -+z (0-1) (**optional**)
`beta`         |   Laser cooling parameter  (**optional**)
`heating`      |   Is the ion heated by photon recoil?  (true/false)
               |   (**optional**)
`seed`         |   Seed for random number generator, use -1 to generate
               |   automatically, or an integer to remove all randomness.
`recoil`       |   Heating recoil factor (**optional**)


### Integration parameters

Parameter        | Description
-----------------|---------------------------------------------------------------
`stepsPerPeriod` | Number of integrator steps to take in one RF period.
`respasteps`     | Number of fast trap force steps to take between each slow
                 | Coulomb force calculation. `respasteps = 1` is equivalent to
                 | velocity Verlet algorithm.
`coolperiods`    | Number of RF periods for equilibration (no data collected)
`histperiods`    | Number of RF periods to propagate while collecting data.


### Image generation

Parameter   | Description
------------|---------------------------------------------------------------
`makeimage` | Boolean. Image will be generated if `true` or present.
`scale`     | Image scaling in pixels per micron
`blur`      | Blur radius in microns
`dof`       | Depth of field in microns
`nz`        | Number of pixels in z axis
`nx`        | Number of pixels in x axis


### Simulation parameters

Parameter   | Description
------------|---------------------------------------------------------------
`threads`   | Number of threads to use in Coulomb force calculation.
            | The total number of running threads will be this number plus one.
            | Zero performs all calculations in a single thread.
`seed`      | Seed for random number generator. Set to -1 to pick seed from
            | system clock.
