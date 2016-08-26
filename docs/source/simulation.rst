The CCMD Simulation
===================

This ion crystal simulation is intended to be flexible and straightforward to
extend, adding new integrators or crystal property reporting.

## Load parameters

The parameters are stored in a file called `trap.info` in the run path. This
contains all simulation parameters, and is described in :file:ccmdsim.cpp. All
parameters are loaded and stored in classes::
        TrapParams trap_params(info_file);
        CloudParams cloud_params(info_file);
        IntegrationParams integration_params(info_file);
        MicroscopeParams microscope_params(info_file);
        SimParams sim_params(info_file);

## Initialse trap and ion cloud

The correct trap class is instantiated

## Attach listeners and integrate

The progress of the simulation is monitored by objects that inherit from the
:class:IntegratorListener


The CCMD simulation is controlled by the :function:main function 

# Simulation Parameters

## Ion Trap

## Ion Cloud

## Integrator

An :class:`IntegratorListener` provides a mechanism to run a function at
regular intervals during the integration. This function can record ion
positions or energies, or modify the ion cloud. 

## Listeners


