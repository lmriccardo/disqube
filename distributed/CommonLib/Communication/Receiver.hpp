#ifndef _RECEIVER_HPP
#define _RECEIVER_HPP

#pragma once

#include <iostream>
#include <optional>
#include <CommonLib/Communication/Socket.hpp>
#include <CommonLib/Concurrency/Thread.hpp>
#include <CommonLib/Concurrency/Queue.hpp>

using Thread = CommonLib::Concurrency::Thread;

namespace CommonLib::Communication
{
    template <typename T>
    class Receiver : public Thread
    {
        protected:
            CommonLib::Concurrency::Queue_ptr<T> _queue; // The queue of received messages

        public:
            Receiver(CommonLib::Concurrency::Queue_ptr<T> q) : _queue(q) {};
    };
}

#endif