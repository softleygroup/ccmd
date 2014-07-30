//
//  ccmd_image.h
//

//
// This class provides allows a simulated microscope image to be produced
// from a 3D histrogram of the time-averaged ion positions
//

#ifndef ccmd_image_ccmd_image_h
#define ccmd_image_ccmd_image_h

#include "ccmd_sim.h"

#include <iostream>
#include <vector>

class Hist3D;
class histPixel;
class Gauss_kernel;

// 
// This class provides a simple 2D image based on array of doubles
//
// Apart from pixel manipulation, principal function is gaussian_blur 
// which performs a 2D convolution on the image using a Gaussian kernel.
//
class CCMD_image {
public:
    CCMD_image(int num_rows, int num_cols) 
        : rows(num_rows), cols(num_cols) 
        { 
            allocate_image(); 
        }
    
    CCMD_image(int num_rows, int num_cols, const Hist3D& hist); 
    virtual ~CCMD_image () { cleanup(); }
    
    CCMD_image(const CCMD_image& image)
    {
        // deep copy of pixel data
        rows = image.rows;
        cols = image.cols;
        allocate_image();
        // copies pixel array
        for (int i=0; i<this->rows; ++i) {
            for (int j=0; j<this->cols; ++j) {
                this->pixels[i][j] = image.pixels[i][j];
            }
        }
    }
    
    CCMD_image& operator+=(const CCMD_image& image_in)
    {
        // adds pixel arrays
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
    
    // makes maximum intensity == 1
    void normalise();
    
    void ouput_to_file(std::string file_name);
    friend std::ostream& operator<<(std::ostream& os, const CCMD_image& im);

private:
    //zero-padded one-dimensional convolution of u and v    
    void conv_1D(double u[], int m, double v[], int n);

    void transpose();
    
    // allocates memory for pixel data on heap
    void allocate_image()
    {
        pixels = new double* [rows];
        for (int i=0; i<rows; ++i) {
            pixels[i] = new double[cols];
            for (int j=0; j<cols; ++j) {
                // initialises a blank image
                pixels[i][j] = 0.0;
            }
        }
    }
    
    // deallocates memory for pixel data from heap
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

//
// 1D gaussian kernel for blurring. Implemented as a 1D CCMD_image.
//
class Gauss_kernel : public CCMD_image
{
public:
    Gauss_kernel(int num_pixels, double sigma); 
private:
    double s;           // standard deviation in pixels
    double gaussian(double x, double mu, double sigma);
};

////
//// Stores microscope model parameters
////
//// see: A. D. Gingell, D.Phil thesis, University of Oxford
////      Chapter 3
////
//struct MicroscopeParams {
//    MicroscopeParams();
//    
//    double pixels_to_distance;      // conversion from microns to pixels
//    double w0;                      // blur size
//    double z0;                      // depth of field
//    int zmin;                       // start plane
//    int zmax;                       // end plane 
//};


//
// This class uses the interface from CCMD_image to produce
// a simulated microscope image for comparison with experimental
// data.
//
class Microscope_image : public CCMD_image {
public:
    Microscope_image(const Hist3D& hist, const MicroscopeParams& p);

    // generates image by adding successive blurred planes 
    // using microscope model
    void draw();    
    
    // have all the planes been added to image?
    bool is_finished();
    
    // percentage progress in image calculation
    float get_progress();
    
private:
    const Hist3D* hist_ptr;
    const MicroscopeParams& params;    
    int plane_now;
    int zmin;
    int zmax;
};


#endif
