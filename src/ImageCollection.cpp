//
//  ImageCollection.cpp
//  ccmd
//
//  Created by Chris Rennick on 21/05/2013.
//
//

#include "ImageCollection.h"

#include "ccmd_image.h"
#include "ccmd_sim.h"
#include "hist3D.h"
#include "vector3D.h"

#include <list>

#include <boost/thread.hpp>

/**
 */

ImageCollection::ImageCollection(const double binSize) : collection()
{
    this->binSize = binSize;
}

ImageCollection::~ImageCollection()
{
    for (Collection::iterator it=collection.begin(); it!=collection.end(); ++it) {
        delete it->second;
        collection.erase(it);
    }
}

void ImageCollection::addIon(const std::string &name, const Vector3D &r)
{
    Hist3D* pIonHist;
    if (collection.count(name))
    {
        pIonHist = collection[name];
    } else {
        pIonHist = new Hist3D;
        pIonHist->set_bin_size(binSize);
        collection[name] = pIonHist;
    }
    
    pIonHist->update(r);
}

void ImageCollection::writeFiles(std::string const& basePath, Microscope_params& p) const
{
    typedef std::list<ImageWorker*> ThreadList;
    ThreadList threadList;
    
    for (Collection::const_iterator it=collection.begin();
         it!=collection.end(); ++it)
    {
        ImageWorker *w = new ImageWorker(basePath, it, p);
        threadList.push_back(w);
    }
    
    for (ThreadList::iterator it_thread=threadList.begin(); it_thread!=threadList.end(); ++it_thread) {
        (*it_thread)->join();
        delete *it_thread;
    }
}

ImageWorker::ImageWorker(
                         std::string const& basePath,
                         ImageCollection::Collection::const_iterator const& it,
                         Microscope_params& p)
:basePath(basePath), fileName(it->first), pIonHist(it->second), params(p)
{
    m_Thread = boost::thread(&ImageWorker::generateAndSave, this);
}

void ImageWorker::join()
{
    m_Thread.join();
}

void ImageWorker::generateAndSave() {
    std::string fileEnding = "_image.png";
    Microscope_image* image;
    
    std::cout << "Generating image: " << fileName << std::endl;
    image = new Microscope_image((*pIonHist), params);
    while (!image->is_finished())
    {
        image->draw();
        printProgBar( static_cast<int>(image->get_progress()) );
    }
    std::cout << std::endl;
    image->ouput_to_file(basePath + fileName + fileEnding);
    delete image;
}


