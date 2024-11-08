#include <Configuration/Configuration.hpp>
#include "Test.hpp"
#include <iostream>

using DisqubeConfiguration = Configuration::DisqubeConfiguration;
using namespace Test;

int main()
{
    std::string filename = "/mnt/c/Users/ricca/Desktop/distributedcpp/conf/disqube.ini";
    DisqubeConfiguration loader(filename);
    unsigned int value = loader.getNumOfQubes();
    std::cout << value << std::endl;

    return 0;
}