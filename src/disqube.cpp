#include <iostream>
#include <CommonLib/CLI/ArgumentParser.hpp>
#include <Qube/Qube.hpp>

namespace cli = Lib::CLI;
namespace conc = Lib::Concurrency;

int main(int argc, const char* argv[])
{
    cli::ArgumentParser argparse(argv[0]);

    argparse.addBooleanArgument({"verbose", "v", "Enable verbose mode", false}, false);
    argparse.addBooleanArgument({"master", "", "Activate master flag", false}, false);
    argparse.addStringArgument({"config", "", "Configuration file", true});

    argparse.parse(argc, argv);

    bool masterFlag = argparse.getBoolean("master");
    std::string confFile = argparse.getString("config");

    conc::AbstractTimerable::prepareSignals();

    Qube::Qube* qube = nullptr;

    if (masterFlag)
    {
        qube = new Qube::QubeManager(confFile);
    }
    else
    {
        qube = new Qube::QubeWorker(confFile);
    }

    qube->run();

    return 0;
}