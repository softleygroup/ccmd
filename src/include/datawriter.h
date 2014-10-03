/** @file datawriter.h
 *
 * @brief Class declaration for writing delimited data to a file.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#ifndef INCLUDE_DATAWRITER_H_
#define INCLUDE_DATAWRITER_H_

#include <fstream>
#include <list>
#include <map>
#include <memory>
#include <string>

class DataWriter {
 public:
     explicit DataWriter(const std::string &delim);
     ~DataWriter();
     void writeRow(const std::string& fileName,
             const std::list<double>& rowData);
     void writeComment(const std::string& fileName,
             const std::string& commentText);

     DataWriter(const DataWriter&) = delete;
     const DataWriter& operator=(const DataWriter&) = delete;
 private:
     /// A pointer to a file stream.
     typedef std::shared_ptr<std::ofstream> StreamPt;
     /// A map holding pointers to a file stream using the file path as a key.
     typedef std::map<std::string, StreamPt> StreamList;
     /// A map holding pointers to a file stream using the file path as a key.
     StreamList streamlist_;
     /// The delimeter string that will be inserted between each number.
     std::string delim_;
     /// A string inserted at the beginning of a comment line
     std::string comment_leader_;

     StreamPt getStream(const std::string& fileName);
};

#endif  // INCLUDE_DATAWRITER_H_
