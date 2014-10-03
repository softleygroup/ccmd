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
#include <thread>
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
    // Build a list of threads, one for each file type.
    ThreadList threadList;
    for (auto& it : collection_) {
        ImageWorker_ptr w = std::make_shared<ImageWorker>(basePath,
                it.first, it.second, p);
        threadList.push_back(w);
    }
    // Wait for all threads to complete before returning.
    for (auto it_thread : threadList) {
        it_thread->join();
    }
}

/** @class ImageWorker
 * @brief A self-starting thread class that calls image drawing functions.
 */

/** @brief Construct a new thread worker and start task immediately.
 *
 * @param basePath Common beginning to file name.
 * @param name Long name of ion.
 * @param hist Pointer to Hist3D data for this ion.
 * @param p Set of MicroscopeParams.
 */
ImageWorker::ImageWorker(
                         const std::string &basePath,
                         const std::string &name,
                         Hist3D_ptr hist,
                         const MicroscopeParams &p)
:base_path_(basePath), filename_(name), hist_(hist), params_(p) {
    thread_ = std::thread(&ImageWorker::generateAndSave, this);
}

/** @brief Rejoin thread.
 */
void ImageWorker::join() {
    thread_.join();
}

/** @brief Create a new Microscope_image and start writing data to it. 
 */
void ImageWorker::generateAndSave() {
    Logger& log = Logger::getInstance();
    std::string fileEnding = "_image.png";
    log.log(Logger::info, "Generating image: " + filename_);
    Microscope_image image(hist_, params_);
    while (!image.is_finished()) {
        image.draw();
    }
    image.ouput_to_file(base_path_ + filename_ + fileEnding);
    log.log(Logger::info, "Done generating image: " + filename_);
}
