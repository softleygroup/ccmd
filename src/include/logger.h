//
//  logger.h
//  ccmd
//
//  Created by Chris Rennick on 22/03/2014.
//
//

#ifndef __ccmd__logger__
#define __ccmd__logger__

#include <boost/thread/mutex.hpp>
#include <fstream>
#include <string>
#include <vector>

/**
 *  @class Logger
 *  @brief Singleton class for logging timestamped information to a file.
 *
 *  Any class requiring logging should get a reference to the current logger by
 *  calling getInstance(). Before the first logging event, set the output level
 *  and file name using the initialise function.
 *
 *  Logging should be thread safe as a boost::mutex log is set before writing
 *  and released immediately after.
 *
 *  Messages are logged with a timestamp, the level and the message:
 *
 *      Sun Mar 23 09:54:40 2014 [Info] Making a cosine trap.
 *
 *  # Levels #
 *
 *  The named logging levels are listed from most to least verbose.
 *
 *  Name        |   Usage
 *  ------------|----------------
 *  \c loop     | Logging events in the iteration loop. Very verbose!!
 *  \c debug    | Debugging information that is not normally needed.
 *  \c info     | Normal level, simulation information and output.
 *  \c warn     | Warning that a default parameter is used as input file is out of range
 *  \c error    | Critical error or exception, simulation will stop after this.
 *
 *  # Example #
 *
 *      Logger& log = Logger::getInstance();            // Get a Logger instance
 *      log.initialise(Logger::info, "log.txt");        // Set level and file
 *      log.log(Logger::error, "An error occured.");    // Write an event
 */

class Logger
{
public:
    /**
     * @brief log output levels in increasing verbosity.
     *  Name        |   Usage
     *  ------------|----------------
     *  \c loop     | Logging events in the iteration loop. Very verbose!!
     *  \c debug    | Debugging information that is not normally needed.
     *  \c info     | Normal level, simulation information and output.
     *  \c warn     | Warning that a default parameter is used as input file is out of range
     *  \c error    | Critical error or exception, simulation will stop after this.
     */
    enum Level {error, warn, info, debug, loop};
    ~Logger();
    
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    
    void initialise(const Level maxlevel, const std::string filename);
    void log(const Level level, const std::string message);
private:
    // Don't implement
    Logger() : maxlevel_(-1) {}
    Logger(Logger const&);
    void operator=(Logger const&);
    
    std::ofstream fileStream_; ///< Log file stream.
    int maxlevel_; ///< Verbosity set in initialise.
    std::vector<std::string> level_string_; ///< Names of each level.
    boost::mutex mtx_; ///< Mutex lock.
};

/**
 *  @class lock
 *  @brief Holds a reference to a mutex lock and ensures the lock is released on exception.
 *
 *  Releasing the lock in the destructor ensures the lock is released when this
 *  object goes out of scope, either through completing the function that needed
 *  a lock, or after an exception.
 */
class lock
{
public:
    lock(boost::mutex &mtx) : mtx_(mtx)
    {
        mtx.lock();
    }
    ~lock()
    {
        mtx_.unlock();
    }
private:
    boost::mutex& mtx_;
};


#endif /* defined(__ccmd__logger__) */
