//
//  IonHistogram.cpp
//  ccmd
//
//  Created by Chris Rennick on 20/05/2013.
//
//

#include "include/IonHistogram.h"

#include <cmath>
#include <fstream>
#include <memory>


/**
 *  @class IonHistogram
 *  @brief Stores a histogram of kinetic energies, separated by ion type.
 *
 *  A new energy is added and indexed by a name. Saving the histogram to a file
 *  uses this name in the file name.
 */

/**
 *  @brief Initialise a histogram with the given bin width.
 *
 *  @param width Bin width.
 */
IonHistogram::IonHistogram(const double width) : histMap()
{
    this->binWidth = width;
}

IonHistogram::~IonHistogram()
{
    histMap.clear();
}


/**
 *  @brief Append a new value to this histogram.
 *  
 *  Find the histogram with a matching name, convert the energy to a bin number
 *  and increment the value in this bin.
 *
 *  @param name     Name of the histogram to append.
 *  @param energy   Value to append to histogram.
 */
void IonHistogram::addIon(const std::string& name, const double& energy)
{
    Histogram_ptr pTheHist;
    if (histMap.count(name))
    {
        pTheHist = histMap[name];
    } else {
        pTheHist = std::make_shared<Histogram>();
        histMap[name] = pTheHist;
    }
    
    int bin_num = int(std::floor(energy/binWidth));
    ++(*pTheHist)[bin_num];
}


/**
 *  @brief Write all histograms to separate files in the given path.
 *
 *  Each file is named with the string used to identify the ion type, followed
 *  by the string "_hist.dat". Files are saved in the given path.
 *
 *  @param basePath Directory in which to save files.
 */
void IonHistogram::writeFiles (const std::string& basePath)
{
    std::string fileEnding = "_hist.dat";
    std::string fileName;
    double binCount;
    Histogram_ptr pTheHist;
    int cumulate;
    int maxBin;

    for (auto& it : histMap) {
        fileName = basePath + it.first + fileEnding;
        std::ofstream fileStream(fileName.c_str());
        pTheHist = it.second;
        
        // Determine the cumulative total and the maximum bin number.
        cumulate = 0;
        maxBin = 0;
        for (auto& histIt : *pTheHist) {
            maxBin = std::max(maxBin, histIt.first);
            cumulate += histIt.second;
        }
        
        // Iterate over all bin numbers, output zero for unused bins.
        for (int i = 0; i < maxBin; i++)
        {
            if(pTheHist->count(i))
                binCount = (pTheHist->at(i));
            else
                binCount = 0;
            
            fileStream << binWidth * i << "\t" << binCount << std::endl;
        }
        fileStream.close();
    }
}

