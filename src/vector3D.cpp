/** 
 * @file vector3d.cpp
 * @brief Function definitions for vector arithmetic.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */
#include "include/vector3D.h"

// vector operations
inline double dot(const Vector3D& a, const Vector3D& b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline Vector3D cross(const Vector3D& a, const Vector3D& b)
{
    return Vector3D(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}

Vector3D& Vector3D::normalise() {
    // scale vector to unit length
    double length = this->norm();
    return (*this)/length;
}

double Vector3D::dist(const Vector3D& a, const Vector3D& b) {
    // Euclidean distance between points
    Vector3D r = a;
    r -= b;
    return r.norm();
 }

std::ostream& operator<<(std::ostream& os, const Vector3D& r) {
    os << '(' << r.x << ',' << r.y << ',' << r.z << ')';
    return os;
}
