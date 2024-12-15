#ifndef _ABSTRACT_CLI_ARGUMENT_H
#define _ABSTRACT_CLI_ARGUMENT_H

#include <string>
#include <stdio.h>
#include <sstream>
#include <CommonLib/CLI/CliArgumentInterface.hpp>

namespace Lib::CLI
{
    template <typename T>
    class TemplateCliArgument : public CliArgumentInterface
    {
    protected:
        std::string m_Name;      // The name of the parameter
        std::string m_NameUpper; // The name of the parameter (upper case)
        std::string m_ShortName; // The parameter shorted name
        std::string m_Help;      // The help string
        T m_DefaultValue;        // The default value of the parameter
        T m_Value;               // The final value

        bool m_Required = false; // If the parameter is required or not
        bool m_HasValue = false; // If the argument has also the value (not just the default one)

    public:
        TemplateCliArgument(const std::string &name) : m_Name(name), m_ShortName("")
        {
            std::transform(m_Name.begin(), m_Name.end(), m_NameUpper.begin(),
                [](unsigned char c) { return std::toupper(c); });
        };

        TemplateCliArgument(const std::string &name, const std::string &sname)
            : m_Name(name), m_ShortName(sname)
        {
            std::transform(m_Name.begin(), m_Name.end(), m_NameUpper.begin(),
                [](unsigned char c) { return std::toupper(c); });
        };

        virtual ~TemplateCliArgument() = default;

        void setDefaultValue(const T &default_val) { m_DefaultValue = default_val; }
        void setRequired(bool value) override { m_Required = value; }
        bool isRequired() const override { return m_Required; }
        void setHelpString(const std::string& help) override { m_Help = help; }
        const std::string& getArgumentName() const override { return m_Name; }
        const std::string& getArgumentName_upper() const override { return m_NameUpper; }
        const std::string& getArgumentShortName() const override { return m_ShortName; }
        const std::string& getArgumentDescription() const override { return m_Help; }
        bool hasShortName() const override { return m_ShortName.size() > 0; }
        bool hasValue() const override { return m_HasValue; }

        T getValue() const
        {
            if (!hasValue()) return m_DefaultValue;
            return m_Value;
        }

        virtual std::string getPatternMatch()
        {
            if (isRequired()) return "";

            std::string pattern;
            pattern = "--" + getArgumentName();

            // If the argument does not have the short name
            if (!hasShortName()) return "(" + pattern + ")";
            
            // If the argument has also the short name we need to include it
            return "(-" + getArgumentShortName() + "|" + pattern + ")";
        }

        void clean() override
        {
            m_HasValue = false;
        }
    };
}

#endif