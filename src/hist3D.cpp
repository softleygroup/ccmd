//
//  hist3D.cpp
//

#include "hist3D.h"

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <limits>

using namespace std;

double Hist3D::bin_size = 1.0;

void Hist3D::update(const Vector3D& r)
{
    bin( Vector3D_to_bin(r) );
}

void Hist3D::set_bin_size(double size)
{ 
    // throws runtime_error if there is already histogram data defined
    // with a different bin size
    if (!hist.empty() && bin_size != size) {
        throw runtime_error("Hist3D bin size already set for existing data");
    }
    bin_size = size;
}

std::ostream& operator<<(std::ostream& os, const Hist3D& h)
{
    // outputs histogram in format: x y z density  
    typedef std::map<std::vector<int>,double>::const_iterator hist_it;
    for (hist_it p=h.hist.begin(); p != h.hist.end(); ++p) {
        Vector3D r = Hist3D::bin_to_Vector3D( p->first );
        os << r.x << ' ' << r.y << ' ' << r.z << ' ' << p->second << '\n';
    }
    return os; 
}

Vector3D Hist3D::bin_to_Vector3D(const std::vector<int> bin_vec)
{
    Vector3D v = Vector3D(bin_vec[0], bin_vec[1], bin_vec[2]);
    return v*bin_size;
}

std::vector<int> Hist3D::Vector3D_to_bin(const Vector3D& v)
{
    std::vector<int> bin_vec(3,0);
    for (int i=0; i<3; ++i)
        bin_vec[i] = std::floor(v[i]/bin_size + 0.5);
    return bin_vec;
}

void Hist3D::output_image(std::ostream& os) const
{
    typedef std::map<std::vector<int>,double>::const_iterator hist_it;
    
    // Get image pixel bounds
    std::vector<int> min_vec(3,999);
    std::vector<int> max_vec(3,-999);
    // Add in exception for max size image
    
    for (hist_it p = hist.begin(); p != hist.end(); ++p) {
        get_vector_bounds( p->first, min_vec, max_vec);
    }

    enum XYZ{x=0,y,z};
    for (int i=min_vec[x]; i<max_vec[x]+1; ++i) {
        for (int j=min_vec[y]; j<max_vec[y]+1; ++j) {
            
            std::vector<int> pixel_vec(3);
            pixel_vec[x] = i;
            pixel_vec[y] = j;
            double pixel_count = 0.0;
            
            for (int k=min_vec[z]; k<max_vec[z]+1; ++k) {
                pixel_vec[z] = k;
                if (this->hist.count(pixel_vec)) {
                    pixel_count += this->hist.at(pixel_vec);
                }
            }
            
            os << i+1 << '\t' << j+1 << '\t' << pixel_count << '\n';
        }
    }
        
}

void Hist3D::get_vector_bounds(const vector<int>& v, 
                               vector<int>& min_store, vector<int>& max_store) const
{
    for (int i=0; i<v.size(); ++i) {
        min_store[i] = min(v[i], min_store[i]);
        max_store[i] = max(v[i], max_store[i]);
    }
}

void Hist3D::output_scaled_hist(std::ostream& os, double scale_factor) const
{
    typedef std::map<std::vector<int>,double>::const_iterator hist_it;
    for (hist_it p=this->hist.begin(); p != this->hist.end(); ++p) {
        Vector3D r = Hist3D::bin_to_Vector3D( p->first );
        r *= scale_factor;
        os << r.x << ' ' << r.y << ' ' << r.z << ' ' << p->second << '\n';
    }
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

void Hist3D::minmax(const Hist3D::xyz& r, int& minr, int& maxr) const
{
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

std::vector<histPixel> Hist3D::getPlane(const Hist3D::xyz& r, int index) const
{
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


void Hist3D::prune(double threshold)
{
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

void Hist3D::reset() 
{
    hist.clear();    
}


    

