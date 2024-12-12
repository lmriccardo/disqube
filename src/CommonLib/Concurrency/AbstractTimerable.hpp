#ifndef _ABSTRACT_TIMER_H
#define _ABSTRACT_TIMER_H

#include <iostream>
#include <cstring>
#include <thread>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <atomic>

#include <CommonLib/Concurrency/Thread.hpp>

#define MIN_TIMEOUT_TRIGGER 10 * 1000 // [us] 10 milli
#define TIMER_SIGNAL SIGUSR1
#define TRIGGER_NSEC MIN_TIMEOUT_TRIGGER * 1000

namespace Lib::Concurrency
{
    class AbstractTimerable : public Thread
    {
    protected:
        unsigned int m_Timeout_us;         // Base Timeout in milliseconds
        unsigned int m_MaxTimeoutCounter;  // Number of base ticks up to timeout
        unsigned int m_TimeoutStepCounter; // Current step counter
        std::atomic<bool> m_Running;       // True if the thread is running, false otherwise

        timer_t m_Timer; // The actual timer

        // The functions that will be runned everytime the wake up
        // signal arrives, i.e., when the timer has elapsed.
        virtual void callback() = 0;
        bool tick();

    public:
        AbstractTimerable(const std::string &name, unsigned int timeout_us);
        virtual ~AbstractTimerable();

        void setTimerTrigger(unsigned int trigger_us);
        void run() override;
        bool isRunning() const override;
        void stop();

        static void prepareSignals();
    };
}

#endif