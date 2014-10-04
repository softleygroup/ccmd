/**
 * @file ionhistogram.cpp
 * @brief Function definitions to generate and store histograms of ion properties.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */
#include "include/ionhistogram.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <memory>
#include <string>


/**
 *  @class IonHistogram
 *  @brief Stores a histogram of kinetic energies, separated by ion type.
 *
 *  A new energy is added and indexed by a name. When saving the histogram to a
 *  file this is used as the file name.
 */

/**
 *  @brief Initialise a histogram with the given bin width.
 *
 *  @param width Bin width.
 */
IonHistogram::IonHistogram(const double width) : hist_map_(), bin_width_(width) {
}

IonHistogram::~IonHistogram() {
    hist_map_.clear();
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
void IonHistogram::addIon(const std::string& name, const double& energy) {
    Histogram_ptr pTheHist;
    if (hist_map_.count(name)) {
        pTheHist = hist_map_[name];
    } else {
        pTheHist = std::make_shared<Histogram>();
        hist_map_[name] = pTheHist;
    }

    int bin_num = static_cast<int>(std::floor(energy/bin_width_));
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
void IonHistogram::writeFiles(const std::string& basePath) {
    std::string fileEnding = "_hist.dat";
    std::string fileName;
    double binCount;
    Histogram_ptr pTheHist;
    int cumulate;
    int maxBin;

    for (auto& it : hist_map_) {
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
        for (int i = 0; i < maxBin; i++) {
            if (pTheHist->count(i))
                binCount = (pTheHist->at(i));
            else
                binCount = 0;

            fileStream << bin_width_ * i << "\t" << binCount << std::endl;
        }
        fileStream.close();
    }
}

