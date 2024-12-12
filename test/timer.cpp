#include <iostream>
#include "Test.hpp"
#include <CommonLib/Concurrency/WakeUpTimer.hpp>

namespace conc = Lib::Concurrency;
using namespace Test;

void test1()
{
    std::cout << "Timer Test: ";
    conc::AbstractTimerable::prepareSignals(); // First we need to prepare for the signal
    
    const int timeout_ms = 5 * 1000;
    conc::WakeUpTimer timer(20 * 1000);
    timer.resetTimeout();
    timer.start();

    int counter = 0;

    while (!timer.checkTimeout(timeout_ms))
    {
        timer.wait();
        counter++;
    }

    assert_eq<int>(counter, 250);
    std::cout << "Passed" << std::endl;
}

int main()
{
    test1();
    return 0;
}