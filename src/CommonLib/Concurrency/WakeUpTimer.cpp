#include "WakeUpTimer.hpp"

using namespace Lib::Concurrency;

void WakeUpTimer::callback()
{
    if (sem_post(&m_WaitSem) != 0)
    {
        printf("Error: %s - sem_post %s\n", __FUNCTION__, std::strerror(errno));
    }
}

WakeUpTimer::WakeUpTimer(unsigned int time_us) : AbstractTimerable("WakeUpTimer", time_us) 
{
    m_Start = std::chrono::high_resolution_clock::now();
    m_StartTimeout = m_Start;

    if (sem_init(&m_WaitSem, 0, 0) != 0)
    {
        printf("Error: %s - sem_init %s\n", __FUNCTION__, std::strerror(errno));
    }
};

WakeUpTimer::~WakeUpTimer()
{
    stop(); // Stop the running thread

    if (sem_destroy(&m_WaitSem) != 0)
    {
        printf("Error: %s - sem_destroy %s\n", __FUNCTION__, std::strerror(errno));
    }
}

void WakeUpTimer::wait()
{
    if (sem_wait(&m_WaitSem) != 0)
    {
        printf("Error: %s - sem_wait %s\n", __FUNCTION__, std::strerror(errno));
    }
}

double WakeUpTimer::getCurrentTime_s() const
{
    auto currTime = std::chrono::high_resolution_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(currTime).count();
}

double WakeUpTimer::getCurrentTime_us() const
{
    auto currTime = std::chrono::high_resolution_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(currTime).count();
}

double WakeUpTimer::getElapsedTime() const
{
    time_point_t end = std::chrono::high_resolution_clock::now();
    auto duration = end - m_Start;
    return std::chrono::duration<double>(duration).count();
}

bool WakeUpTimer::checkTimeout(unsigned int timeout_ms) const
{
    time_point_t end = std::chrono::high_resolution_clock::now();
    auto duration = end - m_StartTimeout;
    auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

    // Check if the timeout has expired
    return millisec.count() >= timeout_ms;
}

void WakeUpTimer::resetTimeout()
{
    m_StartTimeout = std::chrono::high_resolution_clock::now();
}
