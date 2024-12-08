#include <thread>
#include <Logging/ProgressBar.hpp>
#include "Test.hpp"

using namespace Test;

using ProgressBar = Qube::Logging::ProgressBar;

void test1()
{
    int x = 0;
    auto fn = [&x](int i){
        x++;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    };

    ProgressBar::display(0, 1000, 1, "Iterating ... ", fn);

    assert_eq<int>(x, 1000);
}

int main()
{
    test1();

    return 0;
}