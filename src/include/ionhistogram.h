/**
 * @file ionhistogram.h
 * @brief Function declarations for IonHistogram class.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#ifndef INCLUDE_IONHISTOGRAM_H_
#define INCLUDE_IONHISTOGRAM_H_

#include <map>
#include <memory>
#include <string>

class IonHistogram {
 public:
    explicit IonHistogram(const double width);
    ~IonHistogram();
    void addIon(const std::string& name, const double& energy);
    void writeFiles(const std::string& basePath);

    IonHistogram(IonHistogram&) = delete;
    const IonHistogram& operator=(const IonHistogram&) = delete;
 private:
    /// A histogram with bin index number and bin count.
    typedef std::map<int, int> Histogram;
    /// Pointer to a histogram
    typedef std::shared_ptr<Histogram> Histogram_ptr;
    /// Map holding histograms indexed by a name string.
    typedef std::map<std::string, Histogram_ptr> HistMap;

    double bin_width_;    ///< Width of each bin.
    HistMap hist_map_;    ///< The histogram map.
};

typedef std::shared_ptr<IonHistogram> IonHistogram_ptr;

#endif  // INCLUDE_IONHISTOGRAM_H_
