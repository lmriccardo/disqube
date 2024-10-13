#include <iostream>
#include <CommonLib/Concurrency/Queue.hpp>
#include <CommonLib/Concurrency/Thread.hpp>
#include "Test.hpp"

using QueueInt = CommonLib::Concurrency::Queue<int>;
using Thread = CommonLib::Concurrency::Thread;

using namespace Test;

void test_simple()
{
    std::cout << "[TEST 1/2] Single Thread Queue: ";
    QueueInt q(10);
    
    for (int i = 0; i < 10; i++)
    {
        q.push(i);
    }

    for (int i = 0; i < 10; i++)
    {
        assert_eq<int>(q.pop(), i);
    }
    std::cout << "Passed" << std::endl;
}

void test_threaded()
{
    std::cout << "[TEST 2/2] Consumer/Producer Thread Queue: " << std::endl;
    QueueInt q(3);

    std::thread prod = Thread::start([&q]()
    {
        for (int i = 0; i < 10; i++)
        {
            q.push(i);

            std::cout << "Thread " << std::this_thread::get_id();
            std::cout << " pushed " << i << std::endl;
        }
    }, false);

    std::thread cons = Thread::start([&q]()
    {
        for (int i = 0; i < 10; i++)
        {
            int x = q.pop();

            std::cout << "Thread " << std::this_thread::get_id();
            std::cout << " popped " << x << std::endl;
        }
    }, false);

    prod.join();
    cons.join();

    assert_eq<std::size_t>(q.getNofElements(), 0);
    std::cout << "Passed" << std::endl;
}

int main()
{
    test_simple();
    test_threaded();
    return 0;
}