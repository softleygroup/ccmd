//
//  microscope_image.cpp
//  CCMD
//
//  Created by Martin Bell on 22/04/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "ccmd_image.h"
#include "hist3D.h"

Microscope_params::Microscope_params() 
{
    pixels_to_distance = 1;
    w0 = 5.0*pixels_to_distance;
    z0 = 50.0/pixels_to_distance;
    zmin = 0;
    zmax = 0;
}

Microscope_image::Microscope_image(int num_rows, int num_cols, const Hist3D& hist)
    : CCMD_image(num_rows,num_cols), hist_ptr(&hist)
{
    hist.minmax(Hist3D::x, params.zmin, params.zmax);
    plane_now = params.zmin;
}

void Microscope_image::draw()
{
    std::vector<histPixel> pixels;
   
    pixels = hist_ptr->getPlane(Hist3D::x, plane_now);
        
    // scales and moves pixels to image centre
    for (int i=0; i<pixels.size(); ++i) {
        pixels[i].x *= params.pixels_to_distance;
        pixels[i].y *= params.pixels_to_distance;
        pixels[i].x += rows/2;
        pixels[i].y += cols/2;
    }
        
    // updates a new image for current plane
    CCMD_image image_plane(rows,cols);
    image_plane.set_pixel(pixels);
        
    // get blur parameters
    int dz = abs(plane_now);
    
    double blur_radius = params.w0/sqrt(2.0)*(1.0 + dz/params.z0);
    int blur_pixels = 10*blur_radius + 10;
    Gauss_kernel blur_plane(blur_pixels,blur_radius);
    image_plane.gaussian_blur( blur_plane );
        
    // Add image from current plane
    *this += image_plane;

    // Move on to the next plane
    ++plane_now;
}

bool Microscope_image::is_finished() {
    return plane_now == params.zmax;
}

float Microscope_image::get_progress() {
    return (plane_now-params.zmin)*100.0/(params.zmax-params.zmin+1);
}
