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

DataWriter::DataWriter(const std::string &delim)
: delim(delim)
{
}


DataWriter::DataWriter()
: delim("\n")
{
}


DataWriter::~DataWriter()
{
    // Close all out output streams.
	for (StreamList::iterator it=fileStreams.begin(); it!=fileStreams.end(); ++it) {
	it->second->close();
	}
	fileStreams.clear();
}


void DataWriter::writeRow(const std::string& fileName, 
							const std::list<double>& rowData)
{
    // If we can't find the fileName in the map, the stream hasn't been opened yet.
    if (!fileStreams.count(fileName)) {
        fStreamPt stream(new std::ofstream(fileName.c_str(), std::ofstream::out));
        fileStreams[fileName] = stream;
    }
    // Get a pointer to the output file stream.
    fStreamPt out = fileStreams[fileName];
    for (std::list<double>::const_iterator it=rowData.begin(); it!=rowData.end(); ++it) {
        // Output the delimeter if we're not at the start of the line.
        if (it!=rowData.begin())
            (*out) << delim;

        (*out) << (*it);
    }
    (*out) << "\n";
}

