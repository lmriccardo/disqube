#include <Configuration/ConfigurationLoader.hpp>
#include "Test.hpp"
#include <iostream>


using ConfigurationLoader = Configuration::ConfigurationLoader;
using namespace Test;

int main()
{
    std::string filename = "../conf/disqube.ini";
    ConfigurationLoader loader(filename);

    return 0;
}