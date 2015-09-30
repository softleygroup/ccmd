/**
 * @file hist3d.cpp
 * @brief Function definitions for three-dimensional histogram.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

/**
 * @class Hist3D
 * @brief Store a three-dimensional histogram.
 *
 * The exposure time for an ion image is simulated by averaging the positions
 * of each ion over many time steps. This class generates a histogram by
 * selecting the bin a position vector lies in and incrementing the count of
 * this bin.
 *
 */

#include "include/hist3D.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <vector>

#include "include/logger.h"

/**
 * @brief Construct a new Hist3D with a given bin size.
 */
Hist3D::Hist3D(double bin_size) : bin_size_(bin_size) {}

/**
 * @brief Add a new position vector to the histogram.
 *
 * Create the integer bin coordinates from the given position vector, then
 * increment the count of this bin.
 *
 * @parameter r Vector3D to add to the histogram.
 */
void Hist3D::update(const Vector3D& r) {
    std::vector<int> bin_vec(3, 0);
    for (int i = 0; i < 3; ++i)
            bin_vec[i] = static_cast<int>(std::floor(r[i]/bin_size_));
    ++hist_[bin_vec];
}


struct Compare_by_axis {
    explicit Compare_by_axis(const Hist3D::xyz& r) : T(r) {}
    typedef std::map<std::vector<int>, double> Map_type;

    bool operator() (const Map_type::value_type& lhs,
                     const Map_type::value_type& rhs) const {
            return lhs.first.at(T) < rhs.first.at(T);
        }
 private:
    Hist3D::xyz T;
};

/**
 * @brief Get the minimum and maximum bin index along the axis given by \r.
 *
 * @param r The axis to consider.
 * @param minr Reference to return maxumum bin index.
 * @param minr Reference to return minimum bin index.
 * @return Maximum and minimum by reference.
 */
void Hist3D::minmax(const Hist3D::xyz& r, int& minr, int& maxr) const {
    // gets maximum and minimum values from map's key vector
    // overwrites minr and maxr
    if (hist_.empty()) {
        minr = 0;
        maxr = 0;
        return;
    }

    typedef std::map<std::vector<int>, double>::const_iterator hist_it;
    Compare_by_axis compare_r(r);
    hist_it max_ptr = max_element(hist_.begin(), hist_.end(), compare_r);

    maxr = max_ptr->first.at(r);
    if ( max_ptr != hist_.end() ) maxr = max_ptr->first.at(r);

    hist_it min_ptr = min_element(hist_.begin(), hist_.end(), compare_r);
    if ( min_ptr != hist_.end() ) minr = min_ptr->first.at(r);

    return;
}

/**
 * @brief Get a vector of pixels lying in the plane normal to \r.
 *
 * @param r Axis normal to plane
 * @param index Bin number along axis \r specifying layer depth.
 * @return Vector of bin values for occupied bins in layer. 
 */
std::vector<HistPixel> Hist3D::getPlane(const Hist3D::xyz& r, int index) const {
    // returns vector of histPixels found in plane specified by axis r

    std::vector<HistPixel> pixels;
    typedef std::map<std::vector<int>, double>::const_iterator hist_it;

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

    for (auto& p : hist_) {
        if (p.first.at(r) == index) {
            HistPixel foundPixel;
            foundPixel.x = p.first.at(plane_x);
            foundPixel.y = p.first.at(plane_y);
            foundPixel.value = p.second;
            pixels.push_back(foundPixel);
        }
    }
    return pixels;
}

/**
 * @brief Prune the histogram by removing bins with small counts.
 *
 * Threshold is a fraction between 0 and 1 representing the fraction of the
 * maximum bin occupation. Bins with lower count are removed from the map.
 * Threshold out of range is ignored, and no pruning is done.
 *
 * @param threshold Fraction of largest bin to set minimum prune cut-off.
 */
void Hist3D::prune(double threshold) {
    typedef std::map<std::vector<int>, double>::const_iterator const_hist_it;
    typedef std::map<std::vector<int>, double>::iterator hist_it;

    if (threshold < 0 || threshold > 1) {
        Logger& log = Logger::getInstance();
        log.error("Prune threshold out of range. Not pruning.");
        return;
    }

    // find the maximum value in the histogram
    double max_val = 0;
    for (auto &p : hist_) {
        max_val = p.second > max_val ? p.second : max_val;
    }
    // erase bins that fall below threshold
    hist_it p = hist_.begin();
    while ( p != hist_.end() ) {
        if (p->second < threshold*max_val) {
            // erasing element does not invalidate iterator for map,
            // except for element being erased
            hist_.erase(p++);
        } else {
            ++p;
        }
    }
    return;
}

/**
 * @brief Clear the histogram by removing all emements.
 */
void Hist3D::reset() {
    hist_.clear();
}
