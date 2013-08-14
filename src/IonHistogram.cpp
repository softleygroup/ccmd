//
//  IonHistogram.cpp
//  ccmd
//
//  Created by Chris Rennick on 20/05/2013.
//
//

#include "IonHistogram.h"
#include <cmath>
#include <fstream>

IonHistogram::IonHistogram(const double width) : histMap()
{
    this->binWidth = width;
}

IonHistogram::~IonHistogram()
{
    for (HistMap::iterator it=histMap.begin(); it!=histMap.end(); ++it)
    {
        delete it->second;
        histMap.erase(it);
    }
}

void IonHistogram::addIon(const std::string& name, const double& energy)
{
    Histogram* pTheHist;
    if (histMap.count(name))
    {
        pTheHist = histMap[name];
    } else {
        pTheHist = new Histogram;
        histMap[name] = pTheHist;
    }
    
    int bin_num = std::floor(energy/binWidth);
    ++(*pTheHist)[bin_num];
}

void IonHistogram::writeFiles (const std::string& basePath)
{
    std::string fileEnding = "_hist.dat";
    std::string fileName;
    double binCount;
    Histogram* pTheHist;
    int cumulate;
    int maxBin;
    
    for (HistMap::iterator it=histMap.begin(); it!=histMap.end(); ++it)
    {
        fileName = basePath + it->first + fileEnding;
        std::ofstream fileStream(fileName.c_str());
        pTheHist = it->second;
        
        cumulate = 0;
        maxBin = 0;
        for (Histogram::iterator histIt=pTheHist->begin(); histIt!=pTheHist->end(); ++histIt)
        {
            maxBin = std::max(maxBin, histIt->first);
            cumulate += histIt->second;
        }
        
        for (int i=0; i<maxBin; i++)
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