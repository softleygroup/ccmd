/**
 * @file logger.h
 * @brief Declaration of a simple logging class.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */
#ifndef INCLUDE_LOGGER_H_
#define INCLUDE_LOGGER_H_

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

class Logger {
 public:
    /**
     * @brief log output levels in increasing verbosity.
     *  Name        |   Usage
     *  ------------|----------------
     *  \c loop     | Logging events in the iteration loop. Very verbose!!
     *  \c debug    | Debugging information that is not normally needed.
     *  \c info     | Normal level, simulation information and output.
     *  \c warn     | Warning that a default parameter is used as input file is out of range
     *  \c error    | Critical error, simulation will stop after this.
     */
    enum Level {ERROR, WARN, INFO, DEBUG, LOOP};
    ~Logger();

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void initialise(const Level maxlevel, const std::string filename);
    void log(const Level level, const std::string message);

    // Shortcuts
    void loop(const std::string message) { log(LOOP, message); }
    void debug(const std::string message) {log(DEBUG, message); }
    void info(const std::string message) { log(INFO, message); }
    void warn(const std::string message) { log(WARN, message); }
    void error(const std::string message) { log(ERROR, message); }

 private:
    // Don't implement
    Logger() : maxlevel_(-1) {}
    Logger(Logger const&);
    void operator=(Logger const&);

    std::ofstream fileStream_;               ///< Log file stream.
    int maxlevel_;                           ///< Verbosity set in initialise.
    std::vector<std::string> level_string_;  ///< Names of each level.
};



#endif  // INCLUDE_LOGGER_H_
