#include "Thread.hpp"

bool CommonLib::Concurrency::Thread::isJoinable() const
{
    return !_daemon;
}

const std::string &CommonLib::Concurrency::Thread::getThreadName() const
{
    return _name;
}

std::thread::id CommonLib::Concurrency::Thread::getThreadId() const
{
    return _id;
}

void CommonLib::Concurrency::Thread::join()
{
    if (_started) Thread::join(_thread);
}

void CommonLib::Concurrency::Thread::join(std::thread &thread)
{
    if (thread.joinable()) thread.join();
}

void CommonLib::Concurrency::Thread::start()
{
    _thread = std::thread(&Thread::run, this);
    _id = _thread.get_id();
    _started = true;
}
