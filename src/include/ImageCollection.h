//
//  ImageCollection.h
//  ccmd
//
//  Created by Chris Rennick on 21/05/2013.
//
//

#ifndef __ccmd__ImageCollection__
#define __ccmd__ImageCollection__

#include <list>
#include <map>
#include <memory>
#include <string>
#include <thread>

#include "include/hist3D.h"

class Vector3D;
class MicroscopeParams;
class ImageWorker;

class ImageCollection {
 public:
    explicit ImageCollection(double binSize);

    void addIon(const std::string &name, const Vector3D &r);
    void writeFiles(const std::string &basePath, 
            const MicroscopeParams &p) const;

    ImageCollection(const ImageCollection&) = delete;
    const ImageCollection operator=(const ImageCollection&) = delete;
 private:
    typedef std::map<std::string, Hist3D_ptr> Collection;
    typedef std::shared_ptr<ImageWorker> ImageWorker_ptr;
    typedef std::list<ImageWorker_ptr> ThreadList;

    Collection collection;
    double binSize;

    friend class ImageWorker;
};

class ImageWorker {
 public:
    ImageWorker(const std::string &basePath, const std::string &name,
            Hist3D_ptr hist, const MicroscopeParams &p);
    void join();

 private:
    friend class ImageCollection;
    std::string basePath;
    std::thread m_Thread;
    std::string fileName;
    Hist3D_ptr pIonHist;
    const MicroscopeParams& params;

    void generateAndSave();
};



#endif /* defined(__ccmd__ImageCollection__) */
