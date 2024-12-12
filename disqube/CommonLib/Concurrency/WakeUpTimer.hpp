#pragma once

#include <semaphore.h>
#include <CommonLib/Concurrency/AbstractTimerable.hpp>

namespace Lib::Concurrency
{
    class WakeUpTimer : public AbstractTimerable
    {
    private:
        // For simplicity redefined the chrono time point
        typedef std::chrono::_V2::high_resolution_clock::time_point time_point_t;

        time_point_t m_Start; // The starting time
        time_point_t m_StartTimeout; // The timeout starting time
        sem_t m_WaitSem; // The waiting semaphore

        void callback() override;
        
    public:
        WakeUpTimer(unsigned int time_us);
        virtual ~WakeUpTimer();

        void wait();
        double getCurrentTime_s() const;
        double getCurrentTime_us() const;
        double getElapsedTime() const;
        bool checkTimeout(unsigned int timeout_ms) const;
        void resetTimeout();
    };

    typedef std::shared_ptr<WakeUpTimer> WakeUpTimer_ptr;
}