//
//  DataWriter.h
//  ccmd
//
//  Created by Chris Rennick on 06/12/2013.
//
//

#ifndef __ccmd__DataWriter__
#define __ccmd__DataWriter__

#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <string>

#include <boost/shared_ptr.hpp>

class DataWriter
{
public:
	DataWriter(const std::string &delim);
	~DataWriter();
	void writeRow(const std::string& fileName, const std::list<double>& rowData);
private:
    /// A pointer to a file stream.
    typedef boost::shared_ptr<std::ofstream> fStreamPt;
    /// A map holding pointers to a file stream using the file path as a key.
    typedef std::map<std::string, fStreamPt> StreamList;
    /// A map holding pointers to a file stream using the file path as a key.
    StreamList fileStreams;
    /// The delimeter string that will be inserted between each number.
    std::string delim;
};

#endif /* defined(__ccmd__DataWriter__) */
