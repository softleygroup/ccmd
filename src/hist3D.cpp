/** 
 * @file hist3D.cpp
 * @brief Function definitions for three-dimensional histogram.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#include "include/hist3D.h"

#include <cmath>
#include <limits>
#include <stdexcept>

Hist3D::Hist3D(double bin_size) : bin_size_(bin_size) {}

void Hist3D::update(const Vector3D& r) {
    std::vector<int> bin_vec(3,0);
    for (int i=0; i<3; ++i)
            bin_vec[i] = int(std::floor(r[i]/bin_size_));
    ++hist[bin_vec];
}


struct Compare_by_axis {
    Compare_by_axis(const Hist3D::xyz& r) : T(r) {}
    typedef std::map<std::vector<int>,double> Map_type;
    
    bool operator() (const Map_type::value_type& lhs, 
                     const Map_type::value_type& rhs) const
        {
            return lhs.first.at(T) < rhs.first.at(T);
        }
private:
    Hist3D::xyz T;
};

void Hist3D::minmax(const Hist3D::xyz& r, int& minr, int& maxr) const {
    // gets maximum and minimum values from map's key vector
    // overwrites minr and maxr
    if (hist.empty()) {
        minr = 0;
        maxr = 0;
        return;
    }
    
    typedef std::map<std::vector<int>,double>::const_iterator hist_it;
    Compare_by_axis compare_r(r);
    hist_it max_ptr = max_element(hist.begin(), hist.end(), compare_r );
    
    maxr = max_ptr->first.at(r);
    if ( max_ptr != hist.end() ) maxr = max_ptr->first.at(r);
    
    hist_it min_ptr = min_element(hist.begin(), hist.end(), compare_r );
    if ( min_ptr != hist.end() ) minr = min_ptr->first.at(r);
    
    return;
}

std::vector<histPixel> Hist3D::getPlane(const Hist3D::xyz& r, int index) const {
    // returns vector of histPixels found in plane specified by axis r
    
    std::vector<histPixel> pixels;
    typedef std::map<std::vector<int>,double>::const_iterator hist_it;
    
    Hist3D::xyz plane_x = Hist3D::x;
    Hist3D::xyz plane_y = Hist3D::y;

    switch (r) {
        case Hist3D::x : 
            plane_x = Hist3D::y;
            plane_y = Hist3D::z;
            break;
        case Hist3D::y : 
            plane_x = Hist3D::x;
            plane_y = Hist3D::z;
            break;    
        case Hist3D::z : 
            plane_x = Hist3D::x;
            plane_y = Hist3D::y;
    }
    
    for (hist_it p = hist.begin(); p != hist.end(); ++p) {
        if (p->first.at(r) == index) {
            histPixel foundPixel;
            foundPixel.x = p->first.at(plane_x);
            foundPixel.y = p->first.at(plane_y);
            foundPixel.value = p->second;
            pixels.push_back(foundPixel);
        }
    }
    
    
    return pixels;
}


void Hist3D::prune(double threshold) {
    typedef std::map<std::vector<int>,double>::const_iterator const_hist_it;
    typedef std::map<std::vector<int>,double>::iterator hist_it;

    // find the maximum value in the histogram
    double max_val = 0;
    for (const_hist_it p = hist.begin(); p != hist.end(); ++p) {
        max_val = p->second > max_val ? p->second : max_val;
    }
    
    // erase bins that fall below threshold
    hist_it p = hist.begin();
    while ( p != hist.end() ) {
        if (p->second < threshold*max_val) {
            // erasing element does not invalidate iterator for map,
            // except for element being erased
            hist.erase(p++);
        }
        else {
            ++p;
        }
    }
    
    return;
}

void Hist3D::reset() {
    hist.clear();    
}


    

