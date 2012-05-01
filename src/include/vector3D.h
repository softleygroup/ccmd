//
//  vector3D.h
//  CCMD
//
//  Created by Martin Bell on 09/02/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CCMD_Vector3D_h
#define CCMD_Vector3D_h

#include <iostream>
#include <cmath>

struct Vector3D {
public:
    double x,y,z;

    // Constructors
    Vector3D() : x(0.0), y(0.0), z(0.0) {}
    Vector3D(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {}
    Vector3D(const Vector3D& a) { x=a.x; y=a.y; z=a.z; }

    // Copy assignment
    Vector3D& operator=(const Vector3D& a) 
    {
        x = a.x; y = a.y; z = a.z;
        return *this;
    }
    
    // Index Access
    double operator[] (int index) const 
    {
        switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return 0.0;    // Modify to throw exception
        }
    }
    
    // Unary operator
    friend Vector3D operator-(const Vector3D& a) 
    { 
        return Vector3D(-a.x,-a.y,-a.z);
        //x = -x; y = -y; z = -z;
        //return (*this);
    }
    
    // Binary operators
    friend Vector3D operator+(const Vector3D& a, const Vector3D& b) 
                    { return Vector3D(a.x+b.x,a.y+b.y,a.z+b.z); }
    
    friend Vector3D operator-(const Vector3D& a, const Vector3D& b) 
                    { return Vector3D(a.x-b.x,a.y-b.y,a.z-b.z); }
    
    friend Vector3D operator*(const Vector3D& a, const Vector3D& b) 
                    { return Vector3D(a.x*b.x,a.y*b.y,a.z*b.z); }       // Element-wise multiply

    friend Vector3D operator/(const Vector3D& a, const Vector3D& b) 
        { return Vector3D(a.x/b.x,a.y/b.y,a.z/b.z); }                   // Element-wise divide

    friend Vector3D operator*(const Vector3D& a, double d) 
        { return Vector3D(a.x*d,a.y*d,a.z*d); }                         // Multiply by constant
    
    friend Vector3D operator/(const Vector3D& a, double d) 
        { return Vector3D(a.x/d,a.y/d,a.z/d); }                         // Division by constant
    
    Vector3D add(const Vector3D& a)  { return (*this)+a; }
    Vector3D subtract(const Vector3D& a) { return (*this)-a; }
    Vector3D multiply(const Vector3D& a) { return (*this)*a; }
    Vector3D divide(const Vector3D& a) { return (*this)/a; }
    
    Vector3D& operator+=(const Vector3D& a) 
    {
        x += a.x; y += a.y; z += a.z;
        return *this;
    }

    Vector3D& operator-=(const Vector3D& a) 
    {
        x -= a.x; y -= a.y; z -= a.z;
        return *this;
    }
    
    Vector3D& operator*=(const Vector3D& a) 
    {
        x *= a.x; y *= a.y; z *= a.z;
        return *this;
    }

    Vector3D& operator*=(double d) 
    {
        x *= d; y *= d; z *= d;
        return *this;
    }
    
    Vector3D& operator/=(const Vector3D& a) 
    {
        x /= a.x; y /= a.y; z /= a.z;
        return *this;
    }

    Vector3D& operator/=(double d) 
    {
        x /= d; y /= d; z /= d;
        return *this;
    }

    Vector3D& operator/(double d) 
    {
        x /= d; y /= d; z /= d; 
        return *this;
    }
    
    // Comparison 
    friend bool operator==(const Vector3D& a,const Vector3D& b) 
    {
        return (a.x==b.x && a.y==b.y && a.z==b.z);
    }
    
    friend bool operator!=(const Vector3D& a,const Vector3D& b) 
    {
        return (a.x!=b.x || a.y!=b.y || a.z!=b.z);
    }

    friend bool operator<(const Vector3D& a,const Vector3D& b) 
    {
        return (a.norm_sq() < b.norm_sq());
    }

    friend bool operator>(const Vector3D& a,const Vector3D& b) 
    {
        return (a.norm_sq() > b.norm_sq());
    }
    
    // Vector operations
    static double dot(const Vector3D& a, const Vector3D& b) 
    {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    } 

    static Vector3D cross(const Vector3D& a, const Vector3D& b) 
    {
        return Vector3D(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
    }

    double norm() const { return sqrt( norm_sq() ); }

    double norm_sq() const { return x*x + y*y + z*z; }

    
    Vector3D& normalise();
    
    static double dist(const Vector3D& a, const Vector3D& b); 
    
    friend std::ostream& operator<<(std::ostream& os, const Vector3D& r); 

};

#endif
