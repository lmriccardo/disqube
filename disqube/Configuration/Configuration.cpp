#include "Configuration.hpp"

void Configuration::DisqubeConfiguration::readIniFile(const std::string &filepath)
{
    std::filesystem::path path(filepath);

    // Check that the file exists
    if (!std::filesystem::exists(path))
    {
        std::cerr << "Input file: " << filepath << " does not exists" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Check that the input file is a regular file with ini extension
    if (!(
           std::filesystem::is_regular_file(filepath) 
        && path.extension().string() == ".ini"
    ))
    {
        std::cerr << "Input should be a .ini file" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Once we have done all the checks, now we can open the file for reading
    std::ifstream infile(std::filesystem::absolute(path).string());

    // Check that the file has correctly been opened
    if (!infile.is_open())
    {
        std::cerr << "Error opening the input file." << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Initialize the configuration datastructure
    PropertyGroup_ptr currGroup = nullptr;

    std::string currLine;
    std::regex headerPattern("^\\[{1}[a-zA-Z]+\\]{1}$");
    std::regex valuePattern("\\w+\\s*={1}\\s*\\w+");

    while (std::getline(infile, currLine))
    {
        // First we need to see if the current line matches the section header of a .ini 
        // file, which means the word included between two square brackets. The value 
        // will be the key of the group.
        if (std::regex_match(currLine, headerPattern))
        {
            // If the current group is null pointer it means no previous
            // group was created, otherwise we need to add the current
            // group to the configuration
            if (currGroup != nullptr)
            {
                addGroup(currGroup);
                currGroup = nullptr;
            }

            // Create the shared pointer to the property
            std::string propName = currLine.substr(1,currLine.length()-2);
            currGroup = std::make_shared<PropertyGroup>(propName);
            continue;
        }

        // Search for assignment values
        std::smatch matches;
        if (std::regex_search(currLine, matches, valuePattern))
        {
            std::string match = matches[0];
            std::vector<std::string> values = splitString(match, "=");
            if (currGroup == nullptr) continue;

            // Otherwise get the property name and value
            std::string name = values[0];
            std::string value = values[1];
            currGroup->addProperty(std::make_shared<Property>(name, value));
        }
    }
}

std::vector<std::string> Configuration::DisqubeConfiguration::splitString(
    const std::string &line, const std::string &delimiter
) {
    std::vector<std::string> tokens;
    std::regex re(delimiter);
    std::sregex_token_iterator it(line.begin(), line.end(), re, -1);
    std::sregex_token_iterator end;

    while (it != end) 
    {
        tokens.push_back(*it++);
    }

    return tokens;
}

void Configuration::DisqubeConfiguration::addGroup(PropertyGroup_ptr group)
{
    std::string name = group->getGroupName();
    _groups.insert(std::make_pair(name, group));
}

std::string Configuration::DisqubeConfiguration::getConfigurationValue(const std::string &gname, const std::string &pname)
{
    auto it = _groups.find(gname);
    if (it == _groups.end()) throw std::runtime_error("No such given key exists!");
    PropertyGroup_ptr grp = it->second;
    Property_ptr prp = grp->getProperty(pname);
    
    // Check if the cast is possible, in that case returns the
    // value otherwise nullptr
    return prp->getPropertyValue();
}
