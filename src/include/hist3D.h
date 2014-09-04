//
//  Hist3D.h
//  CCMD
//
//  Created by Martin Bell on 23/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CCMD_Hist3D_h
#define CCMD_Hist3D_h

#include <map>
#include <memory>
#include <vector>

#include "vector3D.h"

class histPixel;

class Hist3D {    
public:
    explicit Hist3D(double bin_size);
    enum xyz{x=0,y,z};                  // specifies an axis

    void update(const Vector3D& r);     // adds point to histogram, increments bin
    
    
    size_t get_number_bins() const { return hist.size(); }
    
    // finds bounds of histogram along specified axis
    void minmax(const Hist3D::xyz&, int& minr, int& maxr) const;
    
    // returns all pixels that lie in the plane perpendicular to x,y or z,
    // which passes through the bin specified by r
    std::vector<histPixel> getPlane(const Hist3D::xyz& , int r) const;
    
    // removes histogram bins below a threshold (given as percentage of maximum)
    void prune(double threshold_percent);
    
    // clears histrogram data
    void reset();

    double bin_size_;
    
    Hist3D(const Hist3D&) = delete;
    const Hist3D& operator=(const Hist3D&) = delete;
private:
    std::map<std::vector<int>,double> hist;
    
                           std::vector<int>& , 
                           std::vector<int>& ) const;

};

class histPixel {
    public:
    int x;
    int y;
    double value;
};

typedef std::shared_ptr<Hist3D> Hist3D_ptr;

#endif
