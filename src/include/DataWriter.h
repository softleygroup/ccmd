//
//  DataWriter.h
//  ccmd
//
//  Created by Chris Rennick on 06/12/2013.
//
//

#ifndef __ccmd__DataWriter__
#define __ccmd__DataWriter__

#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <string>

#include <boost/shared_ptr.hpp>

class DataWriter
{
public:
	DataWriter(const std::string &delim);
	~DataWriter();
	void writeRow(const std::string& fileName, const std::list<double>& rowData);
	void writeComment(const std::string& fileName, const std::string& commentText);
    
    DataWriter(const DataWriter&) = delete;
    const DataWriter& operator=(const DataWriter&) = delete;
private:
    /// A pointer to a file stream.
    typedef std::shared_ptr<std::ofstream> fStreamPt;
    /// A map holding pointers to a file stream using the file path as a key.
    typedef std::map<std::string, fStreamPt> StreamList;
    /// A map holding pointers to a file stream using the file path as a key.
    StreamList fileStreams;
    /// The delimeter string that will be inserted between each number.
    std::string delim;
    /// A string inserted at the beginning of a comment line
    std::string commentLeader;
    
    fStreamPt getStream (const std::string& fileName);
};

#endif /* defined(__ccmd__DataWriter__) */
