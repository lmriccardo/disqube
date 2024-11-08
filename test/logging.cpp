#include <Logging/DisqubeLogger.hpp>
#include "Test.hpp"

using DisqubeLogger = Qube::Logging::DisqubeLogger;
using namespace Test;

int main()
{
    DisqubeLogger dl(1001234378, 1, "../log");
    dl.info("Ciaooo");
    dl.warning("A Warning message");
    dl.error("An error message");

    return 0;
}