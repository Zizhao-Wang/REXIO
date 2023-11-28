#include "include/log.h"
#include <cstring>
#include <sstream>

nexio_logger logger;

nexio_logger::nexio_logger() : logLevel(info) {
    // Open default log files
    addLogFile("default", defaultLogFilePath);
    addLogFile("key", keyLogFilePath);
    addLogFile("val", valLogFilePath);
}

nexio_logger::nexio_logger(const std::string& logFilePath) : logLevel(info) {
    addLogFile("default", logFilePath);
}

nexio_logger::~nexio_logger() {
    for (auto& logFile : logFiles) {
        if (logFile.second.is_open()) {
            logFile.second.close();
        }
    }
}

void nexio_logger::setLogLevel(level lvl) {
    logLevel = lvl;
}

void nexio_logger::log(level lvl, const std::string& message, const std::string& fileIdentifier) {
    if (lvl >= logLevel) {
        std::ostringstream oss;
        oss << "[NEXIO";
        switch (lvl) {
            case info:    oss << " INFO] "; break;
            case warning: oss << " WARNING] "; break;
            case error:   oss << " ERROR] "; break;
            case debug:   oss << " DEBUG] "; break;
        }
        oss << message;

        // Write to specified log file
        if (logFiles.find(fileIdentifier) != logFiles.end() && logFiles[fileIdentifier].is_open()) {
            logFiles[fileIdentifier] << oss.str() << std::endl;
        } else {
            std::cerr << "Log file with identifier '" << fileIdentifier << "' not found or not open." << std::endl;
        }
    }
}

void nexio_logger::addLogFile(const std::string& identifier, const std::string& filePath) {
    logFiles[identifier] = std::ofstream(filePath, std::ios::trunc);
    if (!logFiles[identifier].is_open()) {
        std::cerr << "Failed to open log file: " << filePath << std::endl;
    }
}
