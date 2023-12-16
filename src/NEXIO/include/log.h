// log.h

#ifndef NEXIO_INCLUDE_LOGGER_H
#define NEXIO_INCLUDE_LOGGER_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>


class nexio_logger {
public:
    enum level {
        info,
        warning,
        error,
        debug
    };

    nexio_logger();  // Default constructor
    nexio_logger(const std::string& logFilePath);  // Constructor with log file path
    ~nexio_logger();

    void setLogLevel(level lvl);
    void log(level lvl, const std::string& message, const std::string& fileIdentifier = "default");
    void addLogFile(const std::string& identifier, const std::string& filePath);

private:
    level logLevel;  // Current log level
    std::map<std::string, std::ofstream> logFiles;  // Map of log files

    const std::string defaultLogFilePath = "/wzz/nexio.log";  // Default log file path
    const std::string keyLogFilePath = "/wzz/nexio_key.log";  // Key log file path
    const std::string valLogFilePath = "/wzz/nexio_val.log";  // Value log file path
};


extern nexio_logger logger;

#endif // NEXIO_INCLUDE_LOGGER_H
