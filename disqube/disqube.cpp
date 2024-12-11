#include <iostream>
#include <Qube/Qube.hpp>

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: disqube CONF-FILE" << std::endl;
        return 1;
    }

    std::string confFile = argv[1];
    // Qube::QubeInterface itf(true, confFile);
    // itf.qubeDiscovering();

    Qube::QubeManager qube(confFile);
    qube.run();

    return 0;
}