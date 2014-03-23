//
//  logger.cpp
//  ccmd
//
//  Created by Chris Rennick on 22/03/2014.
//
//

#include "logger.h"

#include <ctime>
#include <sstream>


/**
 *  @brief Set log level and output file.
 *
 *  @param maxlevel A selection from the log level enumeration.
 *  @param filename Path to log output file.
 */
void Logger::initialise(const Level maxlevel, const std::string filename)
{
    fileStream_.open(filename.c_str());
    maxlevel_ = maxlevel;
    
    level_string_ = std::vector<std::string>(5, "");
    level_string_[error] = "Error";
    level_string_[warn] = "Warn";
    level_string_[info] = "Info";
    level_string_[debug] = "Debug";
    level_string_[loop] = "Loop";
}

Logger::~Logger()
{
    fileStream_.close();
}

/**
 *  @brief Log an event.
 *
 *  Write a log event to screen and file. This should be thread safe as we get 
 *  a boost::mutex lock before writing the log, and release it on return.
 *
 *  @param level Error level.
 *  @param message The log message.
 */
void Logger::log(const Level level, const std::string message)
{
    lock scopelock(mtx_);
    
    if (maxlevel_ == -1) {
        std::cout << "Logger not initialised - " << message;
        return;
    }
    if (!fileStream_) {
        std::cout << "Log file not open.\n" << message;
        return;
    }
    
    if (level<=maxlevel_) {
        std::time_t rawtime;
        std::time(&rawtime);
        std::string time_string = std::ctime(&rawtime);
        std::stringstream ss;
        ss << time_string.substr(0, time_string.size()-1);
        ss << " [" << level_string_[level] << "] ";
        ss << message;
        ss <<std::endl;
        std::cout << ss.str();
        fileStream_ << ss.str();
        fileStream_.flush();
    }
}
