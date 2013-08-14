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

//
//  This code has been modified for use with the CCMD program
//  (Martin Bell, April 2012)
// 

// Modified to remove GL references

#include <cutil_inline.h>    // includes cuda.h and cuda_runtime_api.h

#include <assert.h>
#include <math.h>
#include <memory.h>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

template <typename T>
void integrateNbodySystem(DeviceData<T>* deviceData,
                          unsigned int currentRead,
                          float deltaTime,
                          unsigned int numBodies, unsigned int numDevices,
                          int p, int q);

template<typename T>
BodySystemCUDA<T>::BodySystemCUDA(unsigned int numBodies, unsigned int numDevices, unsigned int p, 
                                  unsigned int q)
: m_numBodies(numBodies),
  m_numDevices(numDevices),
  m_bInitialized(false),
  m_currentRead(0),
  m_currentWrite(1),
  m_p(p),
  m_q(q)
{
    m_hPos[0] = m_hPos[1] = 0;
    m_hVel = 0;
 
    m_deviceData = 0;

    _initialize(numBodies);
    setSoftening(1e-12f);
    setDamping(1.0f);   
}

template<typename T>
BodySystemCUDA<T>::~BodySystemCUDA()
{
    _finalize();
    m_numBodies = 0;
}

template<typename T>
void BodySystemCUDA<T>::_initialize(int numBodies)
{
    assert(!m_bInitialized);

    m_numBodies = numBodies;

    unsigned int memSize = sizeof(T) * 4 * numBodies;

    m_deviceData = new DeviceData<T>[m_numDevices];

    // divide up the workload amongst Devices
    float *weights = new float[m_numDevices];
    int *numSms = new int[m_numDevices];
    float total = 0;

    for (unsigned int i = 0; i < m_numDevices; i++)
    {
        cudaDeviceProp props;
        cutilSafeCall(cudaGetDeviceProperties(&props, i));
        
        // Choose the weight based on the Compute Capability
        // We estimate that a CC2.0 SM is about 4.0x faster than a CC 1.x SM for
        // this application (since a 15-SM GF100 is about 2X faster than a 30-SM GT200).
        numSms[i] = props.multiProcessorCount;
        weights[i] = numSms[i] * (props.major >= 2 ? 4.f : 1.f);
        total += weights[i];
        
    }

    unsigned int offset = 0;
    unsigned int remaining = m_numBodies;
    for (unsigned int i = 0; i < m_numDevices; i++)
    {
        unsigned int count = (int)((weights[i] / total) * m_numBodies);
        unsigned int round = numSms[i] * 256;
        count = round * ((count + round - 1) / round);
        if (count > remaining) count = remaining;
        remaining -= count;
        m_deviceData[i].offset = offset;
        m_deviceData[i].numBodies = count;
        offset += count;
        if ((i == m_numDevices - 1) && (offset < m_numBodies-1))
            m_deviceData[i].numBodies += m_numBodies - offset;
    }

    delete [] weights;
    delete [] numSms;

    m_hPos[0] = new T[m_numBodies*4];
    m_hVel = new T[m_numBodies*4];

    memset(m_hPos[0], 0, memSize);
    memset(m_hVel, 0, memSize);

    cutilSafeCall(cudaEventCreate(&m_deviceData[0].event));

    cutilSafeCall(cudaMalloc((void **)&m_deviceData[0].dPos[0], memSize));
    cutilSafeCall(cudaMalloc((void **)&m_deviceData[0].dPos[1], memSize));

    cutilSafeCall(cudaMalloc((void **)&m_deviceData[0].dVel, memSize));   

    m_bInitialized = true;
}

template<typename T>
void BodySystemCUDA<T>::_finalize()
{
    assert(m_bInitialized);

    delete [] m_hPos[0];
    delete [] m_hPos[1];
    delete [] m_hVel;

    cutilSafeCall(cudaFree((void**)m_deviceData[0].dVel));

    cutilSafeCall(cudaFree((void**)m_deviceData[0].dPos[0]));
    cutilSafeCall(cudaFree((void**)m_deviceData[0].dPos[1]));

    delete [] m_deviceData;
}

template<typename T>
void BodySystemCUDA<T>::setSoftening(T softening)
{
    T softeningSq = softening*softening;

    for (unsigned int i = 0; i < m_numDevices; i++)
    {
      cutilSafeCall(cudaMemcpyToSymbol( 
          (sizeof(T) > 4) ? "softeningSquared_fp64" : "softeningSquared", 
                                         &softeningSq, 
                                         sizeof(T), 0, 
                                         cudaMemcpyHostToDevice)); 
    }
}


template<typename T>
void BodySystemCUDA<T>::setDamping(T damping)
{
    m_damping = damping;
}

template<typename T>
void BodySystemCUDA<T>::update(T deltaTime)
{
    integrateNbodySystem<T>(m_deviceData, m_currentRead, 
                            (float)deltaTime, m_numBodies, m_numDevices,
                            m_p, m_q);
    std::swap(m_currentRead, m_currentWrite);
}

template<typename T>
T* BodySystemCUDA<T>::getArray(BodyArray array)
{
    assert(m_bInitialized);
 
    T* hdata = 0;
    T* ddata = 0;

    int currentReadHost = 0;

    switch (array)
    {
    default:
    case BODYSYSTEM_POSITION:
        hdata = m_hPos[currentReadHost];
        ddata = m_deviceData[0].dPos[m_currentRead];
        break;
    case BODYSYSTEM_VELOCITY:
        hdata = m_hVel;
        ddata = m_deviceData[0].dVel;
        break;
    }
    cutilSafeCall(cudaMemcpy(hdata, ddata, m_numBodies*4*sizeof(T), cudaMemcpyDeviceToHost));

    return hdata;
}

template<typename T>
void BodySystemCUDA<T>::setArray(BodyArray array, const T* data)
{
    assert(m_bInitialized);

    m_currentRead = 0;
    m_currentWrite = 1;
 
    switch (array)
    {
    default:
    case BODYSYSTEM_POSITION:
        cutilSafeCall(cudaMemcpy(m_deviceData[0].dPos[m_currentRead], data, 
                                 m_numBodies * 4 * sizeof(T), 
                                 cudaMemcpyHostToDevice));
        break;
    case BODYSYSTEM_VELOCITY:
        cutilSafeCall(cudaMemcpy(m_deviceData[0].dVel, data, m_numBodies * 4 * sizeof(T), 
                      cudaMemcpyHostToDevice));
        break;
    }       
}
