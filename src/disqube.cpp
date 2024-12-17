#include <iostream>
#include <CommonLib/CLI/ArgumentParser.hpp>
#include <Qube/Qube.hpp>

namespace cli = Lib::CLI;
namespace conc = Lib::Concurrency;

Qube::Qube* qube = nullptr;

static void Stop( int );

int main(int argc, const char* argv[])
{
    signal(SIGINT, Stop);
    
    cli::ArgumentParser argparse(argv[0]);

    argparse.addBooleanArgument({"verbose", "v", "Enable verbose mode", false}, false);
    argparse.addBooleanArgument({"master", "", "Activate master flag", false}, false);
    argparse.addStringArgument({"config", "", "Configuration file", true});

    argparse.parse(argc, argv);

    bool masterFlag = argparse.getBoolean("master");
    std::string confFile = argparse.getString("config");

    conc::AbstractTimerable::prepareSignals();

    if (masterFlag)
    {
        qube = new Qube::QubeManager(confFile);
    }
    else
    {
        qube = new Qube::QubeWorker(confFile);
    }

    qube->run();

    Stop(0);

    return 0;
}

void Stop( int )
{
    if (qube != nullptr)
    {
        delete qube;
    }

    exit(0);
}