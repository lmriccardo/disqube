#include <Configuration/Configuration.hpp>
#include "Test.hpp"
#include <iostream>

using DisqubeConfiguration = Configuration::DisqubeConfiguration;
using namespace Test;

int main()
{
    std::string filename = "/mnt/c/Users/ricca/Desktop/distributedcpp/conf/disqube.ini";
    DisqubeConfiguration loader(filename);
    std::string value = loader.getConfigurationValue("Environment", "NUMBER_OF_NODES");
    std::cout << std::stoi(value) << std::endl;

    return 0;
}