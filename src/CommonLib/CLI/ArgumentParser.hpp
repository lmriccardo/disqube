#ifndef _ARGUMENTPARSER_H
#define _ARGUMENTPARSER_H

#include <vector>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <CommonLib/CLI/CliArguments.hpp>

namespace Lib::CLI
{
    class ArgumentParser
    {
    public:
        struct ParserArgument_t
        {
            std::string name;       // The name of the argument
            std::string short_name; // The short name of the argument
            std::string help_str;   // The help string of the argument
            bool required;          // If the argument is required or not
        };

    private:
        enum class ArgumentType
        {
            BOOLEAN, // A boolean command line argument
            STRING,  // A string command line argument
            INTEGER, // An integer command line argument
            DOUBLE   // A double command line argument
        };

        std::vector<CliArgumentInterface *> m_RequiredArgs; // Command line arguments
        std::vector<CliArgumentInterface *> m_OptionalArgs; // Optional command line arguments

        std::unordered_map<std::string, ArgumentType> m_ArgtypeMap;       // Maps command line name to their type
        std::unordered_map<std::string, CliArgumentInterface *> m_ArgMap; // Maps command line name to their type

        std::string m_Name; // The name of the executable

        void prettify() const;                               // Pretty printing of command line options
        std::string combinePatterns() const;                 // Combine the patterns of all arguments
        void handleMatchingGroups(const std::smatch &match); // Handles matching groups

        void continueIfExists(const std::string &name) const;
        bool checkArgumentType(const std::string &name, const ArgumentType &type) const;
        void continueIfNoErrors(const std::string &name, const ArgumentType &type) const;

    public:
        ArgumentParser(const char *name) : m_Name(name) {};
        ArgumentParser(const std::string& name) : m_Name(name) {};
        ~ArgumentParser();

        void addBooleanArgument(const ParserArgument_t &args, bool default_value = false);
        void addStringArgument(const ParserArgument_t &args, const std::string &default_value = "");
        void addIntegerArgument(const ParserArgument_t &args, int default_value = 0);
        void addDoubleArgument(const ParserArgument_t &args, double default_value = 0.0);

        const std::string &getProgramName() const;
        void printUsage() const;
        void parse(int argc, const char **argv);

        bool getBoolean(const std::string &name) const;
        std::string getString(const std::string &name) const;
        int getInteger(const std::string &name) const;
        double getDouble(const std::string &name) const;
        void clean();

        ArgumentParser(const ArgumentParser& other) = delete; // Delete the copy constructor
        ArgumentParser& operator=(const ArgumentParser& other) = delete; // Delete also the operator =
    };
}

#endif