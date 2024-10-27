#include "ConfigurationLoader.hpp"

void Configuration::ConfigurationLoader::readIniFile(const std::string &filepath)
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

    std::string currLine;
    while (std::getline(infile, currLine))
    {
        std::cout << currLine << std::endl;
    }
}
