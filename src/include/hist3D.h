/**
 * @file cosine_trap.cpp
 * @brief Function definitions a cosine trap.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#ifndef INCLUDE_HIST3D_H
#define INCLUDE_HIST3D_H

#include <map>
#include <memory>
#include <vector>

#include "vector3D.h"

class HistPixel;

class Hist3D {
 public:
    explicit Hist3D(double bin_size);

    enum xyz{x = 0, y, z};        ///< Specifies an axis
    void update(const Vector3D& r);
    void minmax(const Hist3D::xyz&, int& minr, int& maxr) const;
    std::vector<HistPixel> getPlane(const Hist3D::xyz& , int r) const;
    void prune(double threshold_percent);
    void reset();

    Hist3D(const Hist3D&) = delete;
    const Hist3D& operator=(const Hist3D&) = delete;
 private:
    std::map<std::vector<int>, double> hist_;
    double bin_size_;
};

class HistPixel {
 public:
    int x;
    int y;
    double value;
};

typedef std::shared_ptr<Hist3D> Hist3D_ptr;

#endif  // INCLUDE_HIST3D_H
