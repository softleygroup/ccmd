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
#include "vector3D.h"
#include <iostream>

class histPixel;

class Hist3D {

    std::map<std::vector<int>,double> hist;
    static double bin_size;
    
    void bin(const std::vector<int>& v) { ++hist[v]; }
    static Vector3D bin_to_Vector3D(const std::vector<int>);
    static std::vector<int> Vector3D_to_bin(const Vector3D& v);

    friend std::ostream& operator<<(std::ostream& os, const Hist3D& h);
    void get_vector_bounds(const std::vector<int>& , 
                           std::vector<int>& , 
                           std::vector<int>& ) const;
    
public:
    enum xyz{x=0,y,z};

    void update(const Vector3D& r);
    void set_bin_size(double size);
    size_t get_number_bins() const { return hist.size(); }
    void output_image(std::ostream& os) const;
    void output_hist(std::ostream& os) const;
    void output_scaled_hist(std::ostream& os, double scale_factor) const;
        
    void minmax(const Hist3D::xyz&, int& minr, int& maxr) const;
    std::vector<histPixel> getPlane(const Hist3D::xyz& , int r) const;
    
    void prune(double threshold_percent);

};

struct histPixel {
    int x;
    int y;
    double value;
};

#endif
