#ifndef _CLI_ARGUMENTS_H
#define _CLI_ARGUMENTS_H

#include <string>
#include <CommonLib/CLI/TemplateCliArgument.hpp>

namespace Lib::CLI
{
    class BooleanArgument : public TemplateCliArgument<bool>
    {
    public:
        BooleanArgument(const std::string &name) : TemplateCliArgument<bool>(name) {};
        BooleanArgument(const std::string &name, const std::string &sname)
            : TemplateCliArgument<bool>(name, sname) {};

        std::string getPatternMatch() override 
        {
            std::string pattern = TemplateCliArgument<bool>::getPatternMatch();
            return pattern + "?";
        }
    };

    class IntegerArgument : public TemplateCliArgument<int>
    {
    public:
        IntegerArgument(const std::string &name) : TemplateCliArgument<int>(name) {};
        IntegerArgument(const std::string &name, const std::string &sname)
            : TemplateCliArgument<int>(name, sname) {};

        std::string getPatternMatch() override 
        {
            if (!isRequired())
            {
                std::string pattern = TemplateCliArgument<int>::getPatternMatch();
                pattern = pattern + "(=|\\s+)\\d+";
                return "(" + pattern + ")?";
            }

            return "\\d+";
        }
    };

    class StringArgument : public TemplateCliArgument<std::string>
    {
    public:
        StringArgument(const std::string &name) : TemplateCliArgument<std::string>(name) {};
        StringArgument(const std::string &name, const std::string &sname)
            : TemplateCliArgument<std::string>(name, sname) {};

        std::string getPatternMatch() override 
        {
            if (!isRequired())
            {
                std::string pattern = TemplateCliArgument<std::string>::getPatternMatch();
                pattern = pattern + "(=|\\s+)[a-zA-Z0-9]+";
                return "(" + pattern + ")?";
            }

            return "([[:print:]]+)";
        }
    };

    class DoubleArgument : public TemplateCliArgument<double>
    {
    public:
        DoubleArgument(const std::string &name) : TemplateCliArgument<double>(name) {};
        DoubleArgument(const std::string &name, const std::string &sname)
            : TemplateCliArgument<double>(name, sname) {};

        std::string getPatternMatch() override 
        {
            if (!isRequired())
            {
                std::string pattern = TemplateCliArgument<double>::getPatternMatch();
                pattern = pattern + "(=|\\s+)\\d+\\.{1}\\d+";
                return "(" + pattern + ")?";
            }

            return "\\d+\\.{1}\\d+";
        }
    };
}

#endif