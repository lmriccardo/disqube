#include "Test.hpp"
#include <CommonLib/CLI/ArgumentParser.hpp>

using namespace Test;
namespace cli = Lib::CLI;

int main(int argc, char **argv)
{
    cli::ArgumentParser argparse(argv[0]);

    // for (int i = 0; i < argc; i++)
    // {
    //     char* arg = argv[i];
    //     printf("%s\n", arg);
    // }

    argparse.addBooleanArgument({"verbose", "v", "Enable verbose mode", false}, false);
    argparse.addStringArgument({"interface", "i", "Selected network interface to use", false});
    argparse.addIntegerArgument({"max-size", "", "The maximum reception queue", false}, 100);
    argparse.addStringArgument({"conf", "", "Path to the configuration file", true});
    argparse.printPatterns();

    argparse.parse(argc, argv);

    return 0;
}