#pragma once

#include <string>

namespace Lib::CLI
{
    class CliArgumentInterface
    {
    public:
        virtual ~CliArgumentInterface() = default;
        virtual void setRequired(bool value) = 0;
        virtual bool isRequired() const = 0;
        virtual void setHelpString(const std::string &) = 0;
        virtual const std::string& getArgumentName() const = 0;
        virtual const std::string& getArgumentName_upper() const = 0;
        virtual const std::string& getArgumentShortName() const = 0;
        virtual const std::string& getArgumentDescription() const = 0;
        virtual std::string getPatternMatch() = 0;
        virtual bool hasShortName() const = 0;
        virtual bool hasValue() const = 0;
        virtual void setValue(const std::string& value) = 0;
        virtual void clean() = 0;
    };
}