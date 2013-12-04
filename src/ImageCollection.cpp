//
//  ImageCollection.cpp
//  ccmd
//
//  Created by Chris Rennick on 21/05/2013.
//
//

#include "ImageCollection.h"
#include "hist3D.h"
#include "vector3D.h"
#include "ccmd_image.h"

#include <thread>
#include <list>


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

const void ImageCollection::workerGenerateImage(Collection::const_iterator it, const std::string &basePath)
{
    std::string fileEnding = "_image.dat";
    
    std::string fileName;
    Hist3D* pIonHist;
    Microscope_image* image;
    
    fileName=it->first;
    pIonHist=it->second;
    std::cout << "Generating image: " << fileName << std::endl;
    image = new Microscope_image(640, 640, (*pIonHist));
    while (!image->is_finished())
    {
        image->draw();
        printProgBar( static_cast<int>(image->get_progress()) );
    }
    std::cout << std::endl;
    image->ouput_to_file(basePath + fileName + fileEnding);
    
    delete image;

}


const void ImageCollection::writeFiles(const std::string &basePath)
{
    typedef std::list<std::thread*> ThreadList;
    ThreadList threadList;
    
    for (Collection::const_iterator it=collection.begin();
         it!=collection.end(); ++it)
    {
        std::thread* worker = new std::thread(&ImageCollection::workerGenerateImage, this, it, basePath);
        threadList.push_back(worker);
    }
    
    for (ThreadList::iterator it_thread=threadList.begin(); it_thread!=threadList.end(); ++it_thread) {
        (*it_thread)->join();
        delete (*it_thread);
    }
}

