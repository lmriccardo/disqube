#ifndef _DISQUBE_LOGGER_H
#define _DISQUBE_LOGGER_H

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <vector>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <memory>

#define RED     "\033[31m" // Defines the red color for formatting the log
#define GREEN   "\033[32m" // Defines the green color for formatting the log
#define YELLOW  "\033[33m" // Defines the yellow color for formatting the log
#define BLUE    "\033[34m" // Defines the blue color for formatting the log
#define MAGENTA "\033[35m" // Defines the magenta color for formatting the log
#define CYAN    "\033[36m" // Defines the cyan color for job posting formatting
#define NOCOLO  "\033[0m"  // Resets the collor

namespace Qube::Logging
{
    // Enumeration of all possible logging type
    enum LoggingType
    {
        INFO        = 0, // Lower level of log messages, generic flow of informations
        WARNING     = 1, // Mid level of log messages, for possible but not fatality errors
        ERROR       = 2, // Higer level of log messages, critical errors
        JOB_POSTING = 3  // Newly created job sent to another node
    };

    class DisqubeLogger
    {
        private:
            int                   _Id;        // Logging Id for the current run
            bool                  _logOnFile; // Enable/Disable logging on file
            std::filesystem::path _logFolder; // Root logging folder
            std::string           _logFile;   // Optional logging file
            
            static std::vector<std::string> levels;

            std::string colorize(const std::string& word, LoggingType lvl, bool onFile);
            void log(const std::string& msg, LoggingType lvl);

        public:
            DisqubeLogger(int id, bool logOnFile, const std::string& folder);
            ~DisqubeLogger() = default;

            void info(const std::string& msg);
            void warning(const std::string& msg);
            void error(const std::string& msg);
            void jobPosting(const std::string& msg);
    };

    typedef std::shared_ptr<DisqubeLogger> DisqubeLogger_ptr;
}

#endif