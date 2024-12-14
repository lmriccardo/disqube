#include "ArgumentParser.hpp"

using namespace Lib::CLI;

void ArgumentParser::prettify() const
{
    std::stringstream required_s, optional_s;
    for (const auto &option : m_RequiredArgs)
    {
        required_s << std::left << "    " << std::setw(10);
        required_s << option->getArgumentName();
        required_s << option->getArgumentDescription();
        required_s << std::endl;
    }

    for (const auto &option : m_OptionalArgs)
    {
        optional_s << std::left << "    " << std::setw(6);
        optional_s << ((option->hasShortName()) ? "-" + option->getArgumentShortName() + "," : " ");
        optional_s << std::setw(20) << "--" + option->getArgumentName()
                   << option->getArgumentDescription() << std::endl;
    }

    if (m_OptionalArgs.size() > 0)
    {
        std::cout << "These are the OPTIONS parameter" << std::endl;
        std::cout << optional_s.str();
    }

    std::cout << std::endl;

    if (m_RequiredArgs.size() > 0)
    {
        std::cout << "These are the required arguments" << std::endl;
        std::cout << required_s.str();
    }
}

ArgumentParser::~ArgumentParser()
{
    for (const auto &arg : m_RequiredArgs)
    {
        delete arg; // Delete the pointer to the element
    }

    for (const auto &arg : m_OptionalArgs)
    {
        delete arg; // Delete the pointer to the element
    }
}

void ArgumentParser::addBooleanArgument(const ParserArgument_t &args, bool default_value)
{
    CliArgumentInterface *arg = new BooleanArgument(args.name, args.short_name);
    arg->setRequired(args.required);
    arg->setHelpString(args.help_str);
    ((BooleanArgument *)arg)->setDefaultValue(default_value);

    // Add the argument into the vector
    if (args.required)
        m_RequiredArgs.push_back(arg);
    else
        m_OptionalArgs.push_back(arg);
}

void ArgumentParser::addStringArgument(const ParserArgument_t &args, const std::string &default_value)
{
    CliArgumentInterface *arg = new StringArgument(args.name, args.short_name);
    arg->setRequired(args.required);
    arg->setHelpString(args.help_str);
    ((StringArgument *)arg)->setDefaultValue(default_value);

    // Add the argument into the vector
    if (args.required)
        m_RequiredArgs.push_back(arg);
    else
        m_OptionalArgs.push_back(arg);
}

void ArgumentParser::addIntegerArgument(const ParserArgument_t &args, int default_value)
{
    CliArgumentInterface *arg = new IntegerArgument(args.name, args.short_name);
    arg->setRequired(args.required);
    arg->setHelpString(args.help_str);
    ((IntegerArgument *)arg)->setDefaultValue(default_value);

    // Add the argument into the vector
    if (args.required)
        m_RequiredArgs.push_back(arg);
    else
        m_OptionalArgs.push_back(arg);
}

void ArgumentParser::addDoubleArgument(const ParserArgument_t &args, double default_value)
{
    CliArgumentInterface *arg = new DoubleArgument(args.name, args.short_name);
    arg->setRequired(args.required);
    arg->setHelpString(args.help_str);
    ((DoubleArgument *)arg)->setDefaultValue(default_value);

    // Add the argument into the vector
    if (args.required)
        m_RequiredArgs.push_back(arg);
    else
        m_OptionalArgs.push_back(arg);
}

const std::string &Lib::CLI::ArgumentParser::getProgramName() const
{
    return m_Name;
}

void ArgumentParser::printUsage() const
{
    printf("Usage: %s ", getProgramName().c_str()); // First we need to print the program name

    // Then we need to check if there are parameters
    if (m_OptionalArgs.size() + m_RequiredArgs.size() < 1)
    {
        printf("\n");
        return;
    }

    // Check for any required and optional parameters
    if (m_OptionalArgs.size() > 0)
        printf("[OPTIONS]...");
    if (m_RequiredArgs.size() > 0)
    {
        for (const auto &option : m_RequiredArgs)
        {
            if (option->isRequired())
            {
                printf(" %s", option->getArgumentName_upper().c_str());
            }
        }
    }

    printf("\n\n");
    prettify();
}

void ArgumentParser::parse(int argc, char **argv)
{
    if (argc < 2)
    {
        printUsage();
        exit(EXIT_FAILURE);
    }

    // First we need to assemble the entire string using input argv
    std::string input, tmp;
    for (int index = 1; index < argc; index++)
    {
        input += std::string(argv[index]) + " ";
    }

    std::string pattern = combinePatterns(); // Take the pattern string
    std::regex cmdline_r(pattern);           // Create the regex
    std::smatch match;                       // Create the match results

    if (std::regex_match(input, match, cmdline_r))
    {
        // Print captured groups
        for (size_t i = 0; i < match.size(); ++i) {
            std::cout << "Group " << i << ": " << match[i].str() << std::endl;
        }
    } else {
        std::cerr << "Input arguments bad formatting" << std::endl;
        printUsage();
    }
}

std::string ArgumentParser::combinePatterns() const
{
    std::string pattern = "^"; // Initialize the output pattern

    // First put all the optional arguments
    for (const auto &option : m_OptionalArgs)
    {
        pattern += "\\s*" + option->getPatternMatch();
    }

    // Then all the required arguments
    for (const auto &option : m_RequiredArgs)
    {
        pattern += "\\s*" + option->getPatternMatch();
    }

    return pattern += "$";
}
