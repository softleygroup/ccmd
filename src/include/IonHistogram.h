//
//  IonHistogram.h
//  ccmd
//
//  Created by Chris Rennick on 20/05/2013.
//
//

#ifndef __ccmd__IonHistogram__
#define __ccmd__IonHistogram__

#include <map>
#include <memory>

class IonHistogram {
public:
    IonHistogram(const double width);
    ~IonHistogram();
    void addIon(const std::string& name, const double& energy);
    void writeFiles (const std::string& basePath);
    
    IonHistogram(IonHistogram&) = delete;
    const IonHistogram& operator=(const IonHistogram&) = delete;
private:
    /// A histogram with bin index number and bin count.
    typedef std::map<int, int> Histogram;
    /// Pointer to a histogram
    typedef std::shared_ptr<Histogram> Histogram_ptr;
    /// Map holding histograms indexed by a name string.
    typedef std::map<std::string, Histogram_ptr> HistMap;
    
    double binWidth;    ///< Width of each bin.
    HistMap histMap;    ///< The histogram map.
};

typedef std::shared_ptr<IonHistogram> IonHistogram_ptr;

#endif /* defined(__ccmd__IonHistogram__) */
