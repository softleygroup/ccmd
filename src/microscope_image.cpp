//
//  microscope_image.cpp
//  CCMD
//
//  Created by Martin Bell on 22/04/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


#include <vector>

#include "include/image.h"
#include "include/ccmdsim.h"
#include "include/hist3D.h"


Microscope_image::Microscope_image(const Hist3D_ptr hist,
        const MicroscopeParams& p)
    : Image(p.nx, p.nz), hist_ptr(hist), zmin(0), zmax(0), params(p) {
    hist->minmax(Hist3D::x, zmin, zmax);
    plane_now = zmin;
}

void Microscope_image::draw() {
    std::vector<HistPixel> pixels;
    pixels = hist_ptr->getPlane(Hist3D::x, plane_now);
    // scales and moves pixels to image centre
    for (int i = 0; i < pixels.size(); ++i) {
//        pixels[i].x *= params.pixels_to_distance * hist_ptr->bin_size;
//        pixels[i].y *= params.pixels_to_distance * hist_ptr->bin_size;
        pixels[i].x += rows/2;
        pixels[i].y += cols/2;
    }
    // updates a new image for current plane
    Image image_plane(rows, cols);
    image_plane.set_pixel(pixels);
    // get blur parameters
    int dz = abs(plane_now);
    double blur_radius = params.w0/sqrt(2.0)*(1.0 + dz/params.z0);
    int blur_pixels = 10*blur_radius + 10;
    Gauss_kernel blur_plane(blur_pixels, blur_radius);
    image_plane.gaussian_blur(blur_plane);
    // Add image from current plane
    *this += image_plane;

    // Move on to the next plane
    ++plane_now;
}

bool Microscope_image::is_finished() {
    return plane_now == zmax;
}

float Microscope_image::get_progress() {
    return (plane_now-zmin)*100.0/(zmax-zmin+1);
}
