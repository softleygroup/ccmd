/**
 * @file ccmdimage.h
 * @brief Class declaration for an Image
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#ifndef INCLUDE_IMAGE_H_
#define INCLUDE_IMAGE_H_

#include <string>
#include <vector>

#include "ccmdsim.h"
#include "hist3D.h"

class HistPixel;
class Gauss_kernel;

//
// This class provides a simple 2D image based on array of doubles
//
// Apart from pixel manipulation, principal function is gaussian_blur
// which performs a 2D convolution on the image using a Gaussian kernel.
//
class Image {
 public:
    Image(int num_rows, int num_cols);
    Image(int num_rows, int num_cols, const Hist3D& hist);
    virtual ~Image() { cleanup(); }

    Image(const Image& image) {
        // deep copy of pixel data
        rows = image.rows;
        cols = image.cols;
        allocate_image();
        // copies pixel array
        for (int i = 0; i < this->rows; ++i) {
            for (int j = 0; j < this->cols; ++j) {
                this->pixels[i][j] = image.pixels[i][j];
            }
        }
    }

    Image& operator+=(const Image& image_in) {
        // adds pixel arrays
        for (int i = 0; i < this->rows; ++i) {
            for (int j = 0; j < this->cols; ++j) {
                this->pixels[i][j] += image_in.pixels[i][j];
            }
        }
        return *this;
    }

    int get_rows() const { return rows; }
    int get_cols() const { return cols; }
    double get_pixel(int x, int y) const;

    void set_pixel(const std::vector<HistPixel>& pixels);
    void set_pixel(const HistPixel& pixel);
    void set_pixel(int x, int y, double pixel_val);

    void gaussian_blur(const Gauss_kernel& blurrer);

    // makes maximum intensity == 1
    void normalise();

    void ouput_to_file(std::string file_name);

 private:
    // zero-padded one-dimensional convolution of u and v
    void conv_1D(double u[], int m, double v[], int n);

    void transpose();

    // allocates memory for pixel data on heap
    void allocate_image() {
        pixels = new double* [rows];
        for (int i = 0; i < rows; ++i) {
            pixels[i] = new double[cols];
            for (int j = 0; j < cols; ++j) {
                // initialises a blank image
                pixels[i][j] = 0.0;
            }
        }
    }

    // deallocates memory for pixel data from heap
    void cleanup() {
        for (int i = 0; i < rows; ++i) {
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
// 1D gaussian kernel for blurring. Implemented as a 1D Image.
//
class Gauss_kernel : public Image {
 public:
    Gauss_kernel(int num_pixels, double sigma);
 private:
    double s;           // standard deviation in pixels
    double gaussian(double x, double mu, double sigma);
};


//
// This class uses the interface from Image to produce
// a simulated microscope image for comparison with experimental
// data.
//
class Microscope_image : public Image {
 public:
    Microscope_image(const Hist3D_ptr hist, const MicroscopeParams& p);
    void draw();
    bool is_finished();
    float get_progress();

 private:
    const Hist3D_ptr hist_ptr;
    const MicroscopeParams& params;
    int plane_now;
    int zmin;
    int zmax;
};

#endif  // INCLUDE_IMAGE_H_
