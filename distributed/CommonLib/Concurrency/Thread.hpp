#ifndef _THREAD_HPP
#define _THREAD_HPP

#pragma once

#include <thread>
#include <functional>
#include <string>
#include <memory>

namespace CommonLib::Concurrency
{
    class Thread
    {
        protected:
            std::string     _name;    // The name of the thread
            std::thread::id _id;      // The id of the thread
            bool            _daemon;  // If the thread is a daemon or not
            std::thread     _thread;  // The actual running thread
            bool            _started; // Flag indicating whether the thread has started or not
        
        public:
            Thread() = delete;
            Thread(const std::string& name, bool daemon) \
                : _name(name), _daemon(daemon), _id(0), _started(false) {};
            
            Thread(const std::string& name) : Thread(name, false) {};
            ~Thread()
            {
                join();
            }

            bool isJoinable() const;
            const std::string& getThreadName() const;
            std::thread::id getThreadId() const;

            void join();
            virtual void run() = 0;

            template<typename _Callable, typename ..._Args>
            static std::thread start(_Callable&& fun, bool daemon, _Args&&... args);

            static void join(std::thread& thread);
            void start();
    };
    
    template <typename _Callable, typename ..._Args>
    std::thread Thread::start(_Callable&& fun, bool daemon, _Args&&... args)
    {
        std::thread t;
        t = std::thread(fun, std::forward<_Args>(args)...);
        if (daemon) t.detach(); // Detach the thread if it should be daemon
        return std::move(t);
    }
}

#endif