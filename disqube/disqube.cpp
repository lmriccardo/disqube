#include <iostream>
#include <Qube/QubeInterface.hpp>

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: disqube CONF-FILE" << std::endl;
        return 1;
    }

    std::string confFile = argv[1];
    Qube::QubeInterface itf(true, confFile);
    itf.qubeDiscovering();

    return 0;
}