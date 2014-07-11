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
#include <boost/shared_ptr.hpp>

class IonHistogram {
public:
    IonHistogram(const double width);
    ~IonHistogram();
    void addIon(const std::string& name, const double& energy);
    void writeFiles (const std::string& basePath);
private:
    /// A histogram with bin index number and bin count.
    typedef std::map<int, int> Histogram;
    /// Pointer to a histogram
    typedef boost::shared_ptr<Histogram> Hist_ptr;
    /// Map holding histograms indexed by a name string.
    typedef std::map<std::string, Hist_ptr> HistMap;
    
    double binWidth;    ///< Width of each bin.
    HistMap histMap;    ///< The histogram map.
};

#endif /* defined(__ccmd__IonHistogram__) */
