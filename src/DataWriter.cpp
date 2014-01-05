//
//  DataWriter.cpp
//  ccmd
//
//  Created by Chris Rennick on 06/12/2013.
//
//

#include "DataWriter.h"

#include <list>
//template<class T>

/**
 *  @class DataWriter
 *  @brief Output a delimited data line to a file.
 *
 *  This class will take a std::list of double precision data and write it to
 *  a given file name. The class takes care of opening the file stream the first
 *  time a new file name is encoutered and maintaing the stream to save
 *  re-opening each time new data is written. All open streams are closed in the
 *  class destructor.
 */

/**
 *  @brief Create a new DataWriter object that uses the given string as a delimeter.
 *
 *  When writing one row of data to a file the string stored in  \c delim is 
 *  inserted between each number.
 *
 *  @param delim    Column delimeter to use.
 */
DataWriter::DataWriter(const std::string &delim)
: delim(delim)
{
}

/**
 *  @brief Before the object is destroyed, close all open file streams.
 */
DataWriter::~DataWriter()
{
    // Close all out output streams.
	for (StreamList::iterator it=fileStreams.begin(); it!=fileStreams.end(); ++it) {
	it->second->close();
	}
	fileStreams.clear();
}


/**
 *  @brief Write one line of data to the file.
 *
 *  The parameter fileName contains the path to the file to write. If this does
 *  not exist in fileStreams, a new stream is opened and a pointer stored in the
 *  fileStreams map, using the path string as a key. If the fileName already 
 *  exists as a key in the fileStreams map, get the pointer to the stream.
 *
 *  The list of numbers in rowData forms one row of data in the file. Each 
 *  number is written to the file, separated by the delimeter stored in delim. 
 *  After the final number, a new line character is written.
 *
 *  @param fileName Path to write data to.
 *  @param rowData  List of values to write as one line of data.
 */
void DataWriter::writeRow(const std::string& fileName, 
							const std::list<double>& rowData)
{
    // If we can't find the fileName in the map, the stream hasn't been opened yet.
    if (!fileStreams.count(fileName)) {
        fStreamPt stream(new std::ofstream(fileName.c_str(), std::ofstream::out));
        fileStreams[fileName] = stream;
    }
    // Retrieve a pointer to the output file stream.
    fStreamPt out = fileStreams[fileName];
    for (std::list<double>::const_iterator it=rowData.begin(); it!=rowData.end(); ++it) {
        // Output the delimeter if we're not at the start of the line.
        if (it!=rowData.begin())
            (*out) << delim;

        (*out) << (*it);
    }
    (*out) << "\n";
}

