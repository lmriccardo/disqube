#include "AbstractTimerable.hpp"

using namespace Lib::Concurrency;

bool AbstractTimerable::tick()
{
    m_TimeoutStepCounter--;

    if (m_TimeoutStepCounter == 0)
    {
        m_TimeoutStepCounter = m_MaxTimeoutCounter;
        return true;
    }

    return false;
}

AbstractTimerable::AbstractTimerable(const std::string &name, unsigned int timeout_us)
    : Thread(name), m_Timeout_us(MIN_TIMEOUT_TRIGGER)
{
    setTimerTrigger(timeout_us);
}

AbstractTimerable::~AbstractTimerable()
{
    if (isJoinable()) join(); // Join the running thread

    // Destroy the timer
    if (timer_delete(m_Timer) != 0)
    {
        printf("Error: %s - timer_delete %s\n", __FUNCTION__, std::strerror(errno));
    }
}

void AbstractTimerable::setTimerTrigger(unsigned int trigger_us)
{
    unsigned int tmpvar = (trigger_us > m_Timeout_us) ? trigger_us : m_Timeout_us;
    m_MaxTimeoutCounter = m_Timeout_us / tmpvar;
    m_TimeoutStepCounter = m_MaxTimeoutCounter;
}

void AbstractTimerable::run()
{
    // CLOCK_REALTIME generated signal implementation
    struct sigevent   timerCb;
    struct itimerspec timerSpec;
    sigset_t          timerMask;
    int               recvSig;
    siginfo_t         sigInfo;

    int oldtype, oldstate;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);

    // Prepare the signal mask to wait for signal
    if (sigemptyset(&timerMask) != 0)
    {
        printf("Error: %s - sigemptyset %s\n", __FUNCTION__, std::strerror(errno));
    }

    // Add a signal to the signal set
    if (sigaddset(&timerMask, TIMER_SIGNAL) != 0)
    {
        printf("Error: %s - sigaddset %s\n", __FUNCTION__, std::strerror(errno));
    }

    timerCb.sigev_notify = SIGEV_SIGNAL;
    timerCb.sigev_signo = TIMER_SIGNAL;
    timerCb.sigev_value.sival_ptr = NULL;

    // Create the timer
    if (timer_create(CLOCK_REALTIME, &timerCb, &m_Timer) != 0)
    {
        printf("Error: %s - timer_create %s\n", __FUNCTION__, std::strerror(errno));
        return;
    }

    // Sets the timer to trigger after TRIGGER seconds every time.
    timerSpec.it_value.tv_sec = 0;
    timerSpec.it_value.tv_nsec = TRIGGER_NSEC;
    timerSpec.it_interval.tv_sec = 0;
    timerSpec.it_interval.tv_nsec = TRIGGER_NSEC;

    // Starts the interval timer
    if (timer_settime(m_Timer, 0, &timerSpec, NULL) != 0)
    {
        printf("Error: %s - timer_settime %s\n", __FUNCTION__, std::strerror(errno));
        return;
    }

    bool result;

    while (1)
    {
        // Wait for an internal timer synchronization
        if ((recvSig = sigwaitinfo(&timerMask, &sigInfo)) < 0)
        {
            printf("\nError %s - unexp. signal %d (ret: %d) due to code %d\n",
                __FUNCTION__, sigInfo.si_signo, recvSig, sigInfo.si_code);
            
            continue;
        }

        // Analyze the event received in input
        if (recvSig != TIMER_SIGNAL || sigInfo.si_signo != TIMER_SIGNAL 
            || sigInfo.si_code != SI_TIMER)
        {
            printf("\nError %s - unexp. signal %d (ret: %d) due to code %d\n",
                __FUNCTION__, sigInfo.si_signo, recvSig, sigInfo.si_code);
        }

        // If it is all okay, call the tick function
        result = tick();
        if (!result) continue;

        // Otherwise call the callback function
        callback();
    }
}