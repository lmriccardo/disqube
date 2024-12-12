#include "Thread.hpp"

using namespace Lib::Concurrency;

bool Thread::isJoinable() const
{
    return !_daemon;
}

const std::string &Thread::getThreadName() const
{
    return _name;
}

std::thread::id Thread::getThreadId() const
{
    return _id;
}

void Thread::join()
{
    if (_started) Thread::join(_thread);
}

void Thread::join(std::thread &thread)
{
    if (thread.joinable()) thread.join();
}

void Thread::start()
{
    _thread = std::thread([this](){this->run();});
    _id = _thread.get_id();
    _started = true;
}
