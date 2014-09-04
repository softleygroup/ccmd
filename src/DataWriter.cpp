/** 
 * @file DataWriter.cpp
 * @brief Function definitions for writing delimited data to a file.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#include "include/DataWriter.h"

#include <list>
#include <string>

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
    : delim(delim), commentLeader("# ") {
    }

/**
 *  @brief Before the object is destroyed, close all open file streams.
 */
DataWriter::~DataWriter() {
    // Close all out output streams.
    for (auto it : fileStreams) {
        it.second->close();
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
        const std::list<double>& rowData) {
    DataWriter::fStreamPt out = getStream(fileName);
    for (auto it : rowData) {
        // Output the delimeter if we're not at the start of the line.
        if (it != rowData.front()) {
            (*out) << delim;
        }
        (*out) << it;
    }
    (*out) << ",\n";
}


/**
 *  @brief Write one line of comment text to the file.
 *
 *  This will write the text to a file, preceded by the comment character and 
 *  followed by a new line. This is useful
 *  for writing comments at the top of ouptut files, such as additional 
 *  information or column headers.
 *
 *  @param fileName Path to write data to.
 *  @param text Text to write to file
 */
void DataWriter::writeComment(const std::string& fileName,
        const std::string& commentText) {
    DataWriter::fStreamPt out = getStream(fileName);
    (*out) << commentLeader;
    (*out) << commentText;
    (*out) << std::endl;
}

/**
 *  @brief Get the stored file stream pointer, or create a new one.
 *
 *  @param fileName The file name.
 *  @return Pointer to a file stream.
 */
DataWriter::fStreamPt DataWriter::getStream(const std::string& fileName) {
    // If we can't find the fileName in the map, the stream hasn't been opened
    // yet.
    if (!fileStreams.count(fileName)) {
        fStreamPt stream(new std::ofstream(
                    fileName.c_str(), std::ofstream::out));
        fileStreams[fileName] = stream;
    }
    // Retrieve a pointer to the output file stream.
    fStreamPt out = fileStreams[fileName];
    return out;
}
