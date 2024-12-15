#include "Test.hpp"
#include <CommonLib/CLI/ArgumentParser.hpp>

using namespace Test;
namespace cli = Lib::CLI;

void test1(cli::ArgumentParser& argparse)
{
    std::cout << "[TEST 1/3] Command: argparse-test --argument1 -a2 ciao ../conf : ";
    const char *elements[] = {"argparse-test", "--argument1", "-a2", "ciao", "../conf"};

    argparse.parse(5, elements); // Parse all the arguments

    // Take the values and test the result
    bool arg1        = argparse.getBoolean("argument1");
    std::string arg2 = argparse.getString("argument2");
    std::string arg4 = argparse.getString("argument4");

    assert_eq<bool>(arg1, true);
    assert_eq_str(arg2.c_str(), "ciao", 4);
    assert_eq_str(arg4.c_str(), "../conf", 7);

    argparse.clean();
    std::cout << "Passed" << std::endl;
}

void test2(cli::ArgumentParser& argparse)
{
    std::cout << "[TEST 2/3] Command: argparse-test --argument3=200 -a5 2.7 ../conf : ";
    const char *elements[] = {"argparse-test", "--argument3=200", "-a5", "2.7", "../conf"};

    argparse.parse(5, elements); // Parse all the arguments

    // Take the values and test the result
    int arg3         = argparse.getInteger("argument3");
    double arg5      = argparse.getDouble("argument5");
    std::string arg4 = argparse.getString("argument4");

    assert_eq<int>(arg3, 200);
    assert_eq<double>(arg5, 2.7);
    assert_eq_str(arg4.c_str(), "../conf", 7);

    argparse.clean();
    std::cout << "Passed" << std::endl;
}

void test3(cli::ArgumentParser& argparse)
{
    std::cout << "[TEST 3/3] Command: argparse-test --argument3=200 -a5 2.7 : ";
    try
    {
        const char *elements[] = {"argparse-test", "--argument3=200", "-a5", "2.7"};

        argparse.parse(4, elements); // Parse all the arguments

        // Take the values and test the result
        int arg3         = argparse.getInteger("argument3");
        double arg5      = argparse.getDouble("argument5");
        std::string arg4 = argparse.getString("argument4");

        assert_eq<int>(arg3, 200);
        assert_eq<double>(arg5, 2.7);
        assert_eq_str(arg4.c_str(), "../conf", 7);

        argparse.clean();

        exit(1);
    }
    catch(const std::exception& e)
    {
        std::cout << "Passed" << std::endl;
        return;
    }
    
}

int main()
{
    cli::ArgumentParser argparse("argparse-test"); // Creates the argument parser

    // Add all the desired command line arguments and positional arguments
    argparse.addBooleanArgument({"argument1", "a1", "Argument 1", false}, false);
    argparse.addStringArgument({"argument2", "a2", "Argument 2", false});
    argparse.addIntegerArgument({"argument3", "", "Argument 3", false}, 100);
    argparse.addStringArgument({"argument4", "", "Argument 4", true});
    argparse.addDoubleArgument({"argument5", "a5", "Argument 5", false}, 2.0);

    test1(argparse);
    test2(argparse);
    test3(argparse);
        
    return 0;
}