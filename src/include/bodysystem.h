/*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */
 
 #ifndef __BODYSYSTEM_H__
#define __BODYSYSTEM_H__

enum NBodyConfig
{
    NBODY_CONFIG_RANDOM,
    NBODY_CONFIG_SHELL,
    NBODY_CONFIG_EXPAND,
    NBODY_NUM_CONFIGS
};

enum BodyArray 
{
    BODYSYSTEM_POSITION,
    BODYSYSTEM_VELOCITY,
};

// BodySystem abstract base class
template <typename T>
class BodySystem
{
public: // methods
    BodySystem(int numBodies) {}
    virtual ~BodySystem() {}

    virtual void update(T deltaTime) = 0;

    virtual void setSoftening(T softening) = 0;
    virtual void setDamping(T damping) = 0;

    virtual T* getArray(BodyArray array) = 0;
    virtual void   setArray(BodyArray array, const T* data) = 0;
 
    virtual unsigned int getCurrentReadBuffer() const = 0;

    virtual unsigned int getNumBodies() const = 0; 

    virtual void   synchronizeThreads() const {};

protected: // methods
    BodySystem() {} // default constructor

    virtual void _initialize(int numBodies) = 0;
    virtual void _finalize() = 0;
};

#endif // __BODYSYSTEM_H__
