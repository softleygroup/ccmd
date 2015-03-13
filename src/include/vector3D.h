/** 
 * @file vector3d.h
 * @brief Declaration of a 3 dimensional vector class.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */
#ifndef INCLUDE_VECTOR3D_H_
#define INCLUDE_VECTOR3D_H_

#include <cmath>
#include <iostream>

class Vector3D {
 public:
    double x, y, z;

    // constructors
    Vector3D() : x(0.0), y(0.0), z(0.0) {}
    Vector3D(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {}
    Vector3D(const Vector3D& a) : x(a.x), y(a.y), z(a.z) {}

    // copy assignment
    Vector3D& operator=(const Vector3D& a) {
        x = a.x; y = a.y; z = a.z;
        return *this;
    }

    // index Access
    double operator[] (int index) const {
        switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return 0.0;    // Modify to throw exception
        }
    }

    // unary operator
    friend Vector3D operator-(const Vector3D& a) {
        return Vector3D(-a.x,-a.y,-a.z); }

    // binary operators
    friend Vector3D operator+(const Vector3D& a, const Vector3D& b) {
        return Vector3D(a.x+b.x,a.y+b.y,a.z+b.z); }

    friend Vector3D operator-(const Vector3D& a, const Vector3D& b) {
        return Vector3D(a.x-b.x,a.y-b.y,a.z-b.z); }

    // element-wise multiply
    friend Vector3D operator*(const Vector3D& a, const Vector3D& b) {
        return Vector3D(a.x*b.x,a.y*b.y,a.z*b.z); }

    // element-wise divide
    friend Vector3D operator/(const Vector3D& a, const Vector3D& b) {
        return Vector3D(a.x/b.x,a.y/b.y,a.z/b.z); }

    //multiply by constant
    friend Vector3D operator*(const Vector3D& a, double d) {
        return Vector3D(a.x*d,a.y*d,a.z*d); }

    //division by constant
    friend Vector3D operator/(const Vector3D& a, double d) {
        return Vector3D(a.x/d,a.y/d,a.z/d); }

    Vector3D add(const Vector3D& a)  { return (*this)+a; }
    Vector3D subtract(const Vector3D& a) { return (*this)-a; }
    Vector3D multiply(const Vector3D& a) { return (*this)*a; }
    Vector3D divide(const Vector3D& a) { return (*this)/a; }

    Vector3D& operator+=(const Vector3D& a) {
        x += a.x; y += a.y; z += a.z;
        return *this;
    }

    Vector3D& operator-=(const Vector3D& a) {
        x -= a.x; y -= a.y; z -= a.z;
        return *this;
    }

    Vector3D& operator*=(const Vector3D& a) {
        x *= a.x; y *= a.y; z *= a.z;
        return *this;
    }

    Vector3D& operator*=(double d) {
        x *= d; y *= d; z *= d;
        return *this;
    }

    Vector3D& operator/=(const Vector3D& a) {
        x /= a.x; y /= a.y; z /= a.z;
        return *this;
    }

    Vector3D& operator/=(double d) {
        x /= d; y /= d; z /= d;
        return *this;
    }

    Vector3D& operator/(double d) {
        x /= d; y /= d; z /= d;
        return *this;
    }

    // comparison operators
    friend bool operator==(const Vector3D& a,const Vector3D& b) {
        return (a.x==b.x && a.y==b.y && a.z==b.z);
    }

    friend bool operator!=(const Vector3D& a,const Vector3D& b) {
        return (a.x!=b.x || a.y!=b.y || a.z!=b.z);
    }

    friend bool operator<(const Vector3D& a,const Vector3D& b) {
        return (a.norm_sq() < b.norm_sq());
    }

    friend bool operator>(const Vector3D& a,const Vector3D& b) {
        return (a.norm_sq() > b.norm_sq());
    }

    // vector operations
    static double dot(const Vector3D& a, const Vector3D& b) {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    static Vector3D cross(const Vector3D& a, const Vector3D& b) {
        return Vector3D(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
    }

    double norm() const { return sqrt( norm_sq() ); }

    double norm_sq() const { return x*x + y*y + z*z; }

    // Rotate 45 degrees along z axis to align trap axes with lab axes.
    Vector3D rotate45() const{ 
        double sqrt2 = 1.414213562;
        double newx = (x+y)/sqrt2;
        double newy = (x-y)/sqrt2;
        return Vector3D(newx, newy, z);
    }

    // make vector of unit length
    Vector3D& normalise();

    static double dist(const Vector3D& a, const Vector3D& b);

    friend std::ostream& operator<<(std::ostream& os, const Vector3D& r);

};

#endif  // INCLUDE_VECTOR3D_H_
