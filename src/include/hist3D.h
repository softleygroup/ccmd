//
//  Hist3D.h
//  CCMD
//
//  Created by Martin Bell on 23/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CCMD_Hist3D_h
#define CCMD_Hist3D_h

#include <vector>
#include <map>
#include <iostream>
#include "vector3D.h"

class histPixel;

class Hist3D {    
public:
    
    enum xyz{x=0,y,z};                  // specifies an axis

    void update(const Vector3D& r);     // adds point to histogram, increments bin
    
    void set_bin_size(double size);     // sets histogram resolution
    
    size_t get_number_bins() const { return hist.size(); }
    
    // outputs image by projecting along z-axis 
    // as: row (x), column(y), summed density
    void output_image(std::ostream& os) const;
    
    // outputs histogram in the format
    //void output_hist(std::ostream& os) const;
    
    // outputs histogram with scaled coordinates
    void output_scaled_hist(std::ostream& os, double scale_factor) const;
    
    // finds bounds of histogram along specified axis
    void minmax(const Hist3D::xyz&, int& minr, int& maxr) const;
    
    // returns all pixels that lie in the plane perpendicular to x,y or z,
    // which passes through the bin specified by r
    std::vector<histPixel> getPlane(const Hist3D::xyz& , int r) const;
    
    // removes histogram bins below a threshold (given as percentage of maximum)
    void prune(double threshold_percent);
    
    // clears histrogram data
    void reset();

    static double bin_size;
private:
    std::map<std::vector<int>,double> hist;

    
    void bin(const std::vector<int>& v) { ++hist[v]; }
    static Vector3D bin_to_Vector3D(const std::vector<int>);
    static std::vector<int> Vector3D_to_bin(const Vector3D& v);
    
    friend std::ostream& operator<<(std::ostream& os, const Hist3D& h);
    void get_vector_bounds(const std::vector<int>& , 
                           std::vector<int>& , 
                           std::vector<int>& ) const;

};

class histPixel {
    public:
    int x;
    int y;
    double value;
};

#endif
