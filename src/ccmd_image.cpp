//
//  ccmd_image.cpp
//  ccmd_image
//
//  Created by Martin Bell on 02/04/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ccmd_image.h"
#include "hist3D.h"
#include "cmath"

#include <iostream>
#include <fstream>

CCMD_image::CCMD_image(int num_rows, int num_cols, const Hist3D& hist) 
    : rows(num_rows), cols(num_cols) 
{ 
    allocate_image(); 
    
    double pixels_to_distance = 1;     // Conversion microns to pixels
    double w0 = 5.0*pixels_to_distance;
    double z0 = 50.0/pixels_to_distance;
    
    int zmin = 0;
    int zmax = 0;
    hist.minmax(Hist3D::x, zmin,zmax);
    std::vector<histPixel> pixels;
    
    std::cout << "Number of image planes = " << zmax-zmin+1 << "\n\n";
    std::cout << "Generating image";

    //zmax = 50;
    //zmin = -zmax;
    
    for (int z=zmin; z<zmax+1; ++z) {
        std::cout << std::flush << ".";
        pixels = hist.getPlane(Hist3D::x, z);

        // Scale and move pixels to image centre
        for (int i=0; i<pixels.size(); ++i) {
            pixels[i].x *= pixels_to_distance;
            pixels[i].y *= pixels_to_distance;
            pixels[i].x += num_rows/2;
            pixels[i].y += num_cols/2;
        }
        
        // Update a new image for current plane
        CCMD_image image_plane(num_rows,num_cols);
        image_plane.set_pixel(pixels);
        
        // Get blur parameters
        int dz = abs(z);
        double blur_radius = w0/sqrt(2.0)*(1.0 + dz/z0);
        int blur_pixels = 10*blur_radius + 10;
        
        Gauss_kernel blur_plane(blur_pixels,blur_radius);
        image_plane.gaussian_blur( blur_plane );
        
        // Add image from current plane
        *this += image_plane;
    }    
    this->normalise();
}


double CCMD_image::get_pixel(int x, int y) const
{
    // Return zero if pixel coordinates out of range
    if (x > rows || x < 1 || y > cols || y < 1) return 0.0;
    return pixels[x-1][y-1];
}

void CCMD_image::set_pixel(int x, int y, double pixel_val) 
{
    // No change if pixel coordinates out of range
    if (x > rows || x < 1 || y > cols || y < 1) return;
    pixels[x-1][y-1] = pixel_val;
}

void CCMD_image::set_pixel(const std::vector<histPixel>& pixels)
{
    for (int i=0; i<pixels.size(); ++i) {
        this->set_pixel(pixels[i]);
    }
}


Gauss_kernel::Gauss_kernel(int num_pixels, double sigma)
    : CCMD_image(1,num_pixels), s(sigma) 
{
    double kernel_sum = 0.0;
    // As kernel is symmetric, use only a single column 
    for (int i=0; i<cols; ++i) {
        double x = i - cols/2;
        pixels[0][i] = gaussian(x,0.0,s);
        kernel_sum += pixels[0][i];
    }
    
    // Square sum for 2D normalisation
    kernel_sum *= kernel_sum;
    
    // Normalise kernel
    for (int i=0; i<cols; ++i) {
        pixels[0][i] /= kernel_sum;
    }
    return;
}

double Gauss_kernel::gaussian(double x, double mu, double sigma)
{
    // Unnormalised 1D Gaussian function
    return exp( -(x-mu)*(x-mu)/(2.0*sigma*sigma) );
}

void CCMD_image::gaussian_blur(const Gauss_kernel& blurrer)
{
    double* blur_ptr = blurrer.pixels[0];
    // Apply filter to each row
    for (int i=0; i<rows; ++i) {
        double* pixel_ptr = pixels[i];
        conv_1D(pixel_ptr, cols, blur_ptr, blurrer.cols);
    }
    
    this->transpose();
    // Apply filter to each column
    for (int i=0; i<rows; ++i) {
        double* pixel_ptr = pixels[i];
        conv_1D(pixel_ptr, cols, blur_ptr, blurrer.cols);
    }
    this->transpose();
    
    return;
}

void CCMD_image::conv_1D(double u[], int m, double v[], int n)
{
    // zero-padded one-dimensional convolution of u and v
    // u,v of length m,n respectively
    // central part (of length m) of (u * v) returned in u
    
    double w[m+n-1];
    
    for (int k=1; k<m+n; ++k) {
        w[k-1] = 0;
        int jmin = std::max(1,k+1-n);
        int jmax = std::min(k,m);
        for (int j=jmin; j<jmax+1; ++j) {
            w[k-1] += u[j-1]*v[k-j];
        }
    }
    
    // Copy convolution into input array
    for (int j=0; j<m; ++j) {
        u[j] = w[j+n/2];
    }
    return;    
}

void CCMD_image::transpose()
{
    CCMD_image image_in( *this );
    
    // Delete pixel array
    this->cleanup();
    // Swap dimensions
    this->rows = image_in.cols;
    this->cols = image_in.rows;
    // Reallocate pixel array
    this->allocate_image();
    
    // Copy pixel information
    for (int i=0; i<this->rows; ++i) {
        for (int j=0; j<this->cols; ++j) {
            this->pixels[i][j] = image_in.pixels[j][i];
        }
    }
}
    
std::ostream& operator<<(std::ostream& os, const CCMD_image& im)
{
    /*
    for (int i=0; i<im.rows; ++i) {
        for (int j=0; j<im.cols; ++j) {
            if (im.pixels[i][j] > 0.0) {
                os << 'o';
            } else {
                os << '.';
            }
        }
        os << std::endl;
    }
    */     
    
    for (int i=1; i<im.rows+1; ++i) {
        for (int j=1; j<im.cols+1; ++j) {
            os << i << ' ' << j << ' ' << im.get_pixel(i,j) << std::endl;
        }
    }    
    
    return os;
}

void CCMD_image::set_pixel(const histPixel& pixel) { 
    set_pixel(pixel.x,pixel.y,pixel.value); 
}

void CCMD_image::ouput_to_file(std::string file_name) const {
    std::ofstream image_out;
    image_out.open( file_name.c_str() );
    if ( !image_out ) {
        std::cout << "Can't open output file" << '\n';
    }
    image_out << *this;
    std::cout << "Image output to file: " << file_name << '\n';
}

void CCMD_image::normalise()
{
    // Find maximum
    double maxval = 0.0;
    for (int i=0; i<rows; ++i) {
        for (int j=0; j<cols; ++j) {
            double pixval = get_pixel(i,j);
            maxval = maxval > pixval ? maxval : pixval;
        }
    }
    
    for (int i=0; i<rows; ++i) {
        for (int j=0; j<cols; ++j) {
            double newpixval = get_pixel(i,j)/maxval;
            set_pixel(i,j, newpixval);
        }
    }
}

