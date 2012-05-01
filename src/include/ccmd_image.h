//
//  ccmd_image.h
//  ccmd_image
//
//  Created by Martin Bell on 02/04/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ccmd_image_ccmd_image_h
#define ccmd_image_ccmd_image_h

#include <iostream>
#include <vector>

class Hist3D;
class histPixel;
class Gauss_kernel;

class CCMD_image {
public:
    CCMD_image(int num_rows, int num_cols) 
        : rows(num_rows), cols(num_cols) 
        { allocate_image(); }
    
    CCMD_image(int num_rows, int num_cols, const Hist3D& hist); 
    
    ~CCMD_image () 
        { cleanup(); }
    
    // Copy constructor
    CCMD_image(const CCMD_image& image)
    {
        rows = image.rows;
        cols = image.cols;
        allocate_image();
        // Copy pixel array
        for (int i=0; i<this->rows; ++i) {
            for (int j=0; j<this->cols; ++j) {
                this->pixels[i][j] = image.pixels[i][j];
            }
        }
    }
    
    CCMD_image& operator+=(const CCMD_image& image_in)
    {
        // Add pixel arrays
        for (int i=0; i<this->rows; ++i) {
            for (int j=0; j<this->cols; ++j) {
                this->pixels[i][j] += image_in.pixels[i][j];
            }
        }
        return *this;
    }
    
    int get_rows() const { return rows; }
    int get_cols() const { return cols; }
    double get_pixel(int x, int y) const;
    
    void set_pixel(const std::vector<histPixel>& pixels);
    void set_pixel(const histPixel& pixel);
    void set_pixel(int x, int y, double pixel_val);
    
    void gaussian_blur(const Gauss_kernel& blurrer);
    
    void normalise();
    
    void ouput_to_file(std::string file_name) const;
    
    friend std::ostream& operator<<(std::ostream& os, const CCMD_image& im);

private:
    
    void transpose();
    void conv_1D(double u[], int m, double v[], int n);
    void allocate_image()
    {
        pixels = new double* [rows];
        for (int i=0; i<rows; ++i) {
            pixels[i] = new double[cols];
            for (int j=0; j<cols; ++j) {
                // Initialise a blank image
                pixels[i][j] = 0.0;
            }
        }
    }
    
    void cleanup() 
    {
        for (int i=0; i<rows; ++i) {
            delete [] pixels[i];
        }
        delete [] pixels;
    }
    
protected:
    int rows;           // number of rows
    int cols;           // number of columns
    double **pixels;    // 2D array  
};

// 1D gaussian kernel for blurring
class Gauss_kernel : public CCMD_image
{
public:
    Gauss_kernel(int num_pixels, double sigma); 
private:
    double s;           // Standard deviation in pixels
    double gaussian(double x, double mu, double sigma);
};

struct Microscope_params {
    Microscope_params();
    
    double pixels_to_distance;      // Conversion microns to pixels
    double w0;                      // Blur size
    double z0;                      // Depth of field
    int zmin;                       // Start plane
    int zmax;                       // End plane 
};

class Microscope_image : public CCMD_image {
public:
    Microscope_image(int num_rows, int num_cols, const Hist3D& hist);

    void draw();             // Generate image by adding planes     
    bool is_finished();      // Have all planes been added to image?
    float get_progress();

private:
    const Hist3D* hist_ptr;
    Microscope_params params;    
    int plane_now;
};


#endif
