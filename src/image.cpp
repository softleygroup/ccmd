/** 
 * @file image.cpp
 * @brief Function definitions for Image.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#include "include/image.h"

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "include/hist3D.h"
#include "include/logger.h"

/**
 *  @class Image
 *  @brief Generates an image from the data in Hist3D.
 *
 *  Transforms the 3d histogram of ion positions into a microscope image by
 *  taking each layer of the histogram, blurring it based on the perpendicular
 *  distance between the layer and focal point, then accumulating the blurred
 *  layers into a final image. The intermediate image is stored as an array of
 *  pixel values, then converted to a png for saving.
*/


/**
 *  @brief Construct a new image of the given size.
 *
 *  @param num_rows Number of pixel rows in image.
 *  @param num_cols Number of pixel columns in image.
 *  @param hist The Hist3D object that will provide data for this image.
 */
Image::Image(int num_rows, int num_cols, const Hist3D& hist)
    : rows(num_rows), cols(num_cols) {
    allocate_image();
}


/**
 *  @brief Construct a new image of the given size.
 *
 *  @param num_rows Number of pixel rows in image.
 *  @param num_cols Number of pixel columns in image.
 *  @param hist The Hist3D object that will provide data for this image.
 */
Image::Image(int num_rows, int num_cols)
    : rows(num_rows), cols(num_cols) {
        allocate_image();
}

/**
 *  @brief Return the pixel value at the given coordinates, zero if out of
 *  range.
 *  @return Double precision pixel value.
 */
double Image::get_pixel(int x, int y) const {
    // returns zero if pixel coordinates out of range
    if (x > rows || x < 1 || y > cols || y < 1) return 0.0;
    return pixels[x-1][y-1];
}


/**
 *  @brief Set the pixel value at the given coordinates, do nothing if out of
 *  range.
 */
void Image::set_pixel(int x, int y, double pixel_val) {
    // no change if pixel coordinates out of range
    if (x > rows || x < 1 || y > cols || y < 1) return;
    pixels[x-1][y-1] = pixel_val;
}

void Image::set_pixel(const std::vector<HistPixel>& pixels) {
     for (int i = 0; i < pixels.size(); ++i) {
         this->set_pixel(pixels[i]);
     }
}

void Image::set_pixel(const HistPixel& pixel) {
    set_pixel(pixel.x, pixel.y, pixel.value);
}

Gauss_kernel::Gauss_kernel(int num_pixels, double sigma)
    : Image(1, num_pixels), s(sigma) {
    double kernel_sum = 0.0;
    // as kernel is separable/symmetric, use only a single column
    for (int i = 0; i < cols; ++i) {
        double x = i - cols/2;
        pixels[0][i] = gaussian(x, 0.0, s);
        kernel_sum += pixels[0][i];
    }

    // square sum for 2D normalisation
    kernel_sum *= kernel_sum;

    // normalise kernel
    for (int i = 0; i < cols; ++i) {
        pixels[0][i] /= kernel_sum;
    }
    return;
}

double Gauss_kernel::gaussian(double x, double mu, double sigma) {
    // unnormalised 1D Gaussian function
    return exp( -(x-mu)*(x-mu)/(2.0*sigma*sigma) );
}

void Image::gaussian_blur(const Gauss_kernel& blurrer) {
    double* blur_ptr = blurrer.pixels[0];
    // apply filter to each row
    for (int i = 0; i < rows; ++i) {
        double* pixel_ptr = pixels[i];
        conv_1D(pixel_ptr, cols, blur_ptr, blurrer.cols);
    }

    this->transpose();
    // apply filter to each column
    for (int i = 0; i < rows; ++i) {
        double* pixel_ptr = pixels[i];
        conv_1D(pixel_ptr, cols, blur_ptr, blurrer.cols);
    }
    this->transpose();

    return;
}

/**
 * @brief zero-padded one-dimensional convolution of u and v.
 *
    u,v of length m,n respectively
    central part (of length m) of (u * v) returned in u
 */
void Image::conv_1D(double u[], int m, double v[], int n) {
    double w[m+n-1];

    // convolution integral as discrete sum
    for (int k = 1; k < m+n; ++k) {
        w[k-1] = 0;
        int jmin = std::max(1, k+1-n);
        int jmax = std::min(k, m);
        for (int j = jmin; j < jmax+1; ++j) {
            w[k-1] += u[j-1]*v[k-j];
        }
    }

    // copies convolution into input array
    for (int j = 0; j < m; ++j) {
        u[j] = w[j+n/2];
    }
    return;
}

void Image::transpose() {
    Image image_in(*this);

    // delete pixel array
    this->cleanup();
    // swap dimensions
    this->rows = image_in.cols;
    this->cols = image_in.rows;
    // reallocate pixel array
    this->allocate_image();

    // copy pixel information
    for (int i = 0; i < this->rows; ++i) {
        for (int j = 0; j < this->cols; ++j) {
            this->pixels[i][j] = image_in.pixels[j][i];
        }
    }
}

/**
 * @brief Write normalised pixel values to an 8 bit greyscale image.
 *
 * Normalises image first, which replaces all pixel values. Call this function
 * after any other processing has been completed.
 */
void Image::ouput_to_file(std::string file_name) {
    Logger& log = Logger::getInstance();
    normalise();
    boost::gil::gray8_image_t img(rows+1, cols+1);
    boost::gil::gray8_view_t view = boost::gil::view(img);

    for (int i = 1; i < rows+1; ++i) {
        for (int j = 1; j < cols+1; ++j) {
            view(i, j) = boost::gil::gray8_pixel_t((uint8_t)(
                                                get_pixel(i, j)*254));
        }
    }
    boost::gil::png_write_view(file_name, view);
}


/**
 * @brief Normalise all pixel values to double precision in range (0-1)
 */
void Image::normalise() {
    // Find brightest pixel in image.
    double maxval = 0.0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            double pixval = get_pixel(i, j);
            maxval = maxval > pixval ? maxval : pixval;
        }
    }

    // Scale max brightness to 1
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            double newpixval = get_pixel(i, j)/maxval;
            set_pixel(i, j, newpixval);
        }
    }
}
