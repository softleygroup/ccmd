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
	DataWriter();
	~DataWriter();
	void writeRow(const std::string& fileName, const std::list<double>& rowData);
private:
    typedef boost::shared_ptr<std::ofstream> fStreamPt;
    typedef std::map<std::string, fStreamPt> StreamList;
    StreamList fileStreams;
    std::string delim;
};

#endif /* defined(__ccmd__DataWriter__) */
