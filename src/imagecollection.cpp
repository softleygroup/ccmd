/** @file imagecollection.cpp
 *
 * @brief Function definitions for ImageCollection
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#include "include/imagecollection.h"

#include <list>
#include <memory>
#include <string>

#include "include/image.h"
#include "include/ccmdsim.h"
#include "include/logger.h"
#include "include/hist3D.h"
#include "include/vector3D.h"

/** @class ImageCollection
 *
 * @brief Maintain a list of ion position histograms, one for each trapped ion
 * type. Generate one image for each ion type.
 *
 * A set of Hist3D are stored in a map using the ion name as a key. Ion
 * positions are inserted into the appropriate histogram by name. If the
 * histogram does not already exist a new one is made.
 *
 * Calling the writeFiles function outputs a microscope image for each ion type
 * into a png file with the same name.
 */

/** @brief Initialise the collection_ map and store the bin size.
 */
ImageCollection::ImageCollection(double binsize)
    : collection_(), binsize_(binsize) {
}

/** @brief Add an ion position to the appropriate histogram.
 *
 * A pointer to the Hist3D is pulled from the collection_ map by name. If this
 * does not already exist, a new shared pointer is made and inserted into the
 * map. The Hist3d::update funcion is called to insert the position vector into
 * the histogram.
 *
 * @param name Long name of the ion.
 * @param r Ion position.
 */
void ImageCollection::addIon(const std::string &name, const Vector3D &r) {
    Hist3D_ptr hist;
    if (collection_.count(name)) {
        // Hist3D exists, use it.
        hist = collection_[name];
    } else {
        // Hist3D does not exist. Create a new one and insert into map
        hist = std::make_shared<Hist3D>(binsize_);
        collection_[name] = hist;
    }
    // Call the Hist3D function to insert a position into the array.
    hist ->update(r);
}

/** @brief Output all histograms as microscope images.
 *
 * A new thread is started to write each ion type.
 *
 * @param basePath Path and common start to image file name.
 * @param p Microscope imaging parameters to pass on.
 */
void ImageCollection::writeFiles(const std::string &basePath,
        const MicroscopeParams &p) const {
    Logger& log = Logger::getInstance();
    std::string fileEnding = "_image.png";
    for (auto& it : collection_) {
        log.info("Generating image: " + it.first);
        Microscope_image image(it.second, p);
        while (!image.is_finished()) {
            image.draw();
        }
        image.ouput_to_file(basePath + it.first + fileEnding);
        log.info("Done generating image: " + it.first);
    }
}

