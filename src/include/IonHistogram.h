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

class IonHistogram {
public:
    IonHistogram(const double width);
    ~IonHistogram();
    void addIon(const std::string& name, const double& energy);
    void writeFiles (const std::string& basePath);
private:
    typedef std::map<int, int> Histogram;
    typedef std::map<std::string, Histogram*> HistMap;
    typedef std::pair<std::string, Histogram*> HistPair;
    
    double binWidth;
    HistMap histMap;
};

#endif /* defined(__ccmd__IonHistogram__) */
