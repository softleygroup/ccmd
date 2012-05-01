//
//  Vector3D.cpp
//  CCMD
//
//  Created by Martin Bell on 10/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "vector3D.h"

// Vector operations
inline double dot(const Vector3D& a, const Vector3D& b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}


inline Vector3D cross(const Vector3D& a, const Vector3D& b)
{
    return Vector3D(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}


// Scale vector to unit length
Vector3D& Vector3D::normalise() {
    double length = this->norm();
    return (*this)/length;
}

double Vector3D::dist(const Vector3D& a, const Vector3D& b) {
    Vector3D r = a;
    r -= b;
    return r.norm();
 }

std::ostream& operator<<(std::ostream& os, const Vector3D& r) {
    os << '(' << r.x << ',' << r.y << ',' << r.z << ')';
    return os;
}
