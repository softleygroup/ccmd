//
//  ImageCollection.cpp
//  ccmd
//
//  Created by Chris Rennick on 21/05/2013.
//
//

#include "include/ImageCollection.h"

#include <list>
#include <memory>
#include <thread>
#include <string>

#include "include/ccmd_image.h"
#include "include/ccmd_sim.h"
#include "include/logger.h"
#include "include/hist3D.h"
#include "include/vector3D.h"

ImageCollection::ImageCollection(double binSize) : collection() {
    this->binSize = binSize;
}


void ImageCollection::addIon(const std::string &name, const Vector3D &r) {
    Hist3D_ptr pIonHist;
    if (collection.count(name)) {
        pIonHist = collection[name];
    } else {
        pIonHist = std::make_shared<Hist3D>(binSize);
        collection[name] = pIonHist;
    }
    pIonHist->update(r);
}

void ImageCollection::writeFiles(const std::string &basePath,
        const MicroscopeParams &p) const {
    ThreadList threadList;
    for (auto& it : collection) {
        ImageWorker_ptr w = std::make_shared<ImageWorker>(basePath,
                it.first, it.second, p);
        threadList.push_back(w);
    }
    for (auto it_thread : threadList) {
        it_thread->join();
    }
}

ImageWorker::ImageWorker(
                         const std::string &basePath,
                         const std::string &name,
                         Hist3D_ptr hist,
                         const MicroscopeParams &p)
:basePath(basePath), fileName(name), pIonHist(hist), params(p) {
    m_Thread = std::thread(&ImageWorker::generateAndSave, this);
}

void ImageWorker::join() {
    m_Thread.join();
}

void ImageWorker::generateAndSave() {
    Logger& log = Logger::getInstance();
    std::string fileEnding = "_image.png";
    log.log(Logger::info, "Generating image: " + fileName);
    Microscope_image image(pIonHist, params);
    while (!image.is_finished()) {
        image.draw();
    }
    image.ouput_to_file(basePath + fileName + fileEnding);
    log.log(Logger::info, "Done generating image: " + fileName);
}
