/** @file imagecollection.h
 *
 * @brief Class declaration for ImageCollection
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#ifndef INCLUDE_IMAGECOLLECTION_H_
#define INCLUDE_IMAGECOLLECTION_H_

#include <list>
#include <map>
#include <memory>
#include <string>

#include "hist3D.h"

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

    Collection collection_;
    double binsize_;
};


#endif  // INCLUDE_IMAGECOLLECTION_H_
