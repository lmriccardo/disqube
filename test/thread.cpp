#include <iostream>
#include <vector>
#include <mutex>
#include <memory>
#include <chrono>
#include <CommonLib/Concurrency/Thread.hpp>

#include "Test.hpp"

using namespace Test;

std::mutex mutex;

class MyThread : public Lib::Concurrency::Thread
{
    public:
        std::vector<int> vec;
        int counter;

        MyThread(int _counter) : Thread("MyThread"), counter(_counter) {};

        void run()
        {
            for (int i = 0; i < this->counter; i++)
            {
                vec.push_back(i);
            }

            std::unique_lock<std::mutex> lock(mutex);
            std::cout << "Thread " << std::this_thread::get_id();
            std::cout << " finished putting " << this->counter;
            std::cout << " elements" << std::endl;
        }

        bool isRunning() const
        {
            return false;
        }
};

class MyDaemonThread : public Lib::Concurrency::Thread
{
    public:
        bool stopsig;
        bool stopped;

        MyDaemonThread(const std::string& name) : Thread(name, true), stopsig(false), stopped(false) {};

        void run()
        {
            int counter = 0;

            while (!stopsig)
            {
                std::unique_lock<std::mutex> lock(mutex);
                std::cout << this->getThreadName() << ": ";
                std::cout << "iteration " << counter++ << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            stopped = true;
        }

        void stop()
        {
            stopsig = true;
        }

        bool isStopped()
        {
            return stopped;
        }

        bool isRunning() const
        {
            return !stopped;
        }
};

template <typename T>
void start_all(std::vector<T*>& vec)
{
    for (const auto t: vec)
    {
        t->start();
    }
}

void stop_all(std::vector<MyDaemonThread*>& vec)
{
    for (auto t: vec)
    {
        t->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void join_all(std::vector<MyThread*>& vec)
{
    for (const auto t: vec)
    {
        t->join();
    }
}

void test_nof_elements(std::vector<MyThread*>& vec, const std::vector<std::size_t>& obj)
{
    for (int idx = 0; idx < vec.size(); idx++)
    {
        MyThread* t_ptr = vec.at(idx);
        assert_eq<std::size_t>(t_ptr->vec.size(), obj.at(idx));
    }
}

void test_all_stopped(std::vector<MyDaemonThread*>& vec)
{
    for (auto t: vec)
    {
        if (!t->isStopped())
        {   
            std::cout << "Not Passed -> ";
            std::cout << "Thread: " << t->getThreadName() << " still running..." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

void test_thread_class()
{
    std::cout << "[TEST 1/3] Testing Joinable custom Thread classes: ";
    MyThread mt1(100);
    MyThread mt2(10);
    MyThread mt3(1000);

    std::vector<MyThread*> vec = {&mt1, &mt2, &mt3};
    start_all<MyThread>(vec);
    join_all(vec);

    test_nof_elements(vec, {100, 10, 1000});
    std::cout << "Passed" << std::endl;
}

void test_daemon_threads()
{
    std::cout << "[TEST 2/3] Testing Daemon custom Thread classes: ";
    MyDaemonThread mt1("Thread1");
    MyDaemonThread mt2("Thread2");
    MyDaemonThread mt3("Thread3");

    std::vector<MyDaemonThread*> vec = {&mt1, &mt2, &mt3};
    start_all<MyDaemonThread>(vec);
    stop_all(vec);
    test_all_stopped(vec);

    std::cout << "Passed" << std::endl;
}

void test_function_thread()
{
    std::cout << "[TEST 3/3] Testing lambda function thread: ";
    int x = 10;
    std::thread t1 = Lib::Concurrency::Thread::start(
        [](int x)
        {
            for (int i = 0; i < x; i++)
            {
                std::cout << "Thread " << std::this_thread::get_id();
                std::cout << " says " << i << std::endl;
            }
        },
        false,
        x
    );

    t1.join();
    std::cout << "Passed" << std::endl;
}

int main()
{
    test_thread_class();
    test_daemon_threads();
    test_function_thread();
    return 0;
}