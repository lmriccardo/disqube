#pragma once

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

        void setValue(const std::string& value) override 
        {
            m_Value = (value.compare("true") == 0);
            m_HasValue = true;
        }

        void setValue()
        {
            if (!m_HasValue) m_Value = !m_DefaultValue;
            m_HasValue = true;
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
                pattern = pattern + "(?:=|\\s+)(\\d+)";
                return "(?:" + pattern + ")?";
            }

            return "(\\d+)";
        }

        void setValue(const std::string& value) override 
        {
            m_Value = std::stoi(value);
            m_HasValue = true;
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
                pattern = pattern + "(?:=|\\s+)([\\w\\./_\\\\-]+)";
                return "(?:" + pattern + ")?";
            }

            return "([\\w\\./_\\\\-]+)";
        }

        void setValue(const std::string& value) override 
        {
            m_Value = value;
            m_HasValue = true;
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
                pattern = pattern + "(?:=|\\s+)(\\d+\\.\\d+)";
                return "(?:" + pattern + ")?";
            }

            return "(\\d+\\.{1}\\d+)";
        }

        void setValue(const std::string& value) override 
        {
            m_Value = std::stod(value);
            m_HasValue = true;
        }
    };
}