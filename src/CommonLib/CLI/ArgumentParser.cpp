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
    for (auto &arg : m_RequiredArgs)
    {
        if (arg != nullptr) delete arg; // Delete the pointer to the element
    }

    for (auto &arg : m_OptionalArgs)
    {
        if (arg != nullptr) delete arg; // Delete the pointer to the element
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

    // Add to the unordered map
    m_ArgtypeMap.insert(std::make_pair(arg->getArgumentName(), ArgumentType::BOOLEAN));
    m_ArgMap.insert(std::make_pair(arg->getArgumentName(), arg));
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

    // Add to the unordered map
    m_ArgtypeMap.insert(std::make_pair(arg->getArgumentName(), ArgumentType::STRING));
    m_ArgMap.insert(std::make_pair(arg->getArgumentName(), arg));
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

    // Add to the unordered map
    m_ArgtypeMap.insert(std::make_pair(arg->getArgumentName(), ArgumentType::INTEGER));
    m_ArgMap.insert(std::make_pair(arg->getArgumentName(), arg));
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

    // Add to the unordered map
    m_ArgtypeMap.insert(std::make_pair(arg->getArgumentName(), ArgumentType::DOUBLE));
    m_ArgMap.insert(std::make_pair(arg->getArgumentName(), arg));
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

void ArgumentParser::parse(int argc, const char **argv)
{
    if (argc < 2)
    {
        printUsage();
        exit(EXIT_FAILURE);
    }

    // First we need to assemble the entire string using input argv
    std::string input;
    for (int index = 1; index < argc; index++)
    {
        input += std::string(argv[index]);
        if (index < argc - 1)
            input += " ";
    }

    std::string pattern = combinePatterns(); // Take the pattern string
    std::regex cmdline_r(pattern);           // Create the regex
    std::smatch match;                       // Create the match results

    if (std::regex_match(input, match, cmdline_r))
    {
        handleMatchingGroups(match);
    }
    else
    {
        std::cerr << "Input arguments bad formatting" << std::endl;
        printUsage();
        throw std::runtime_error("");
    }
}

bool ArgumentParser::getBoolean(const std::string &name) const
{
    // Check that there are no errors then continue
    continueIfNoErrors(name, ArgumentType::BOOLEAN);

    // Take the corresponding argument
    CliArgumentInterface *arg = m_ArgMap.at(name);
    return ((BooleanArgument *)arg)->getValue();
}

std::string ArgumentParser::getString(const std::string &name) const
{
    // Check that there are no errors then continue
    continueIfNoErrors(name, ArgumentType::STRING);

    // Take the corresponding argument
    CliArgumentInterface *arg = m_ArgMap.at(name);
    return ((StringArgument *)arg)->getValue();
}

int ArgumentParser::getInteger(const std::string &name) const
{
    // Check that there are no errors then continue
    continueIfNoErrors(name, ArgumentType::INTEGER);

    // Take the corresponding argument
    CliArgumentInterface *arg = m_ArgMap.at(name);
    return ((IntegerArgument *)arg)->getValue();
}

double ArgumentParser::getDouble(const std::string &name) const
{
    // Check that there are no errors then continue
    continueIfNoErrors(name, ArgumentType::DOUBLE);

    // Take the corresponding argument
    CliArgumentInterface *arg = m_ArgMap.at(name);
    return ((DoubleArgument *)arg)->getValue();
}

void ArgumentParser::clean()
{
    std::unordered_map<std::string, CliArgumentInterface*>::iterator it;
    for (it = m_ArgMap.begin(); it != m_ArgMap.end(); it++)
    {
        it->second->clean();
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
        pattern += "\\s+" + option->getPatternMatch();
    }

    return pattern += "$";
}

void ArgumentParser::handleMatchingGroups(const std::smatch &match)
{
    // Print captured groups
    CliArgumentInterface *option;
    int opt_argument_idx = 0, pos_argument_idx = 0, curridx = 1;
    int nofargs, valueidx;

    while (curridx < match.size())
    {
        // Take the correct command line option or position argument
        if (opt_argument_idx < m_OptionalArgs.size())
        {
            option = m_OptionalArgs[opt_argument_idx];
            opt_argument_idx++;
        }
        else
        {
            option = m_RequiredArgs[pos_argument_idx];
            pos_argument_idx++;
        }

        // Take the argument type to understand how many arguments it requires
        auto type = m_ArgtypeMap[option->getArgumentName()];
        nofargs = (type != ArgumentType::BOOLEAN && !option->isRequired()) ? 2 : 1;

        // If there is no match for the current argument, continue
        if (!match[curridx].matched)
        {
            curridx += nofargs;
            continue;
        }

        // If the type is boolean than set the correct value
        if (type == ArgumentType::BOOLEAN)
        {
            ((BooleanArgument *)option)->setValue(); // We know that it is boolean
        }
        else
        {
            valueidx = (nofargs > 1) ? curridx + 1 : curridx;
            option->setValue(match[valueidx].str());
        }

        curridx += nofargs;
    }
}

void ArgumentParser::continueIfExists(const std::string &name) const
{
    // Check if the parameter name is in the argument map
    auto pos = m_ArgtypeMap.find(name);
    if (pos == m_ArgtypeMap.end())
    {
        printf("Argument %s does not exists.\n", name.c_str());
        throw std::runtime_error("Error");
    }
}

bool ArgumentParser::checkArgumentType(const std::string &name, const ArgumentType &type) const
{
    return m_ArgtypeMap.at(name) == type;
}

void ArgumentParser::continueIfNoErrors(const std::string &name, const ArgumentType &type) const
{
    continueIfExists(name); // Check if the input parameter exists

    // Check that the given argument matches the given type
    if (!checkArgumentType(name, type))
    {
        printf("Input argument %s does not match input type\n", name.c_str());
        throw std::runtime_error("Error");
    }
}
