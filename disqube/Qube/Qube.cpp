#include "Qube.hpp"

unsigned int Qube::Qube::generateId()
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);

    // Take hours, minutes and seconds and returns the id
    int hour = localTime->tm_hour;
    int minutes = localTime->tm_min;
    int seconds = localTime->tm_sec;
    return (unsigned int)((hour << 16) + (minutes << 8) + seconds);
}

void Qube::Qube::initStateMachine()
{
    State_ptr s0 = std::make_shared<State>(StateType::QUBE_INIT, 2);
    State_ptr s1 = std::make_shared<State>(StateType::QUBE_DISCOVERING, 2);
    State_ptr s2 = std::make_shared<State>(StateType::QUBE_OPERATIVE, 3);
    State_ptr s3 = std::make_shared<State>(StateType::QUBE_MAINTENANCE, 2);
    State_ptr s4 = std::make_shared<State>(StateType::QUBE_SHUTDOWN, 1);

    s0->addTransition(s1, [](Input_t p){return p.itfReady && (p.discoverFlag && p.isMaster);});
    s0->addTransition(s2, [](Input_t p){return p.itfReady && !(p.discoverFlag && p.isMaster);});
    s1->addTransition(s4, [](Input_t p){return p.shutdown;});
    s1->addTransition(s2, [](Input_t p){return p.anyWorker;});
    s2->addTransition(s4, [](Input_t p){return p.shutdown;});
    s2->addTransition(s3, [](Input_t p){return p.maintenance;});
    s2->addTransition(s1, [](Input_t p){return !p.anyWorker && p.isMaster;});
    s3->addTransition(s4, [](Input_t p){return p.shutdown;});
    s3->addTransition(s2, [](Input_t p){return !p.maintenance;});

    _stateMachine = std::make_shared<StateMachine>(s0);
}

void Qube::Qube::init()
{
    // Load the configuration
    _conf = std::make_shared<DisqubeConfiguration>(_confFile);

    // Create the logger
    _logger = std::make_shared<Logging::DisqubeLogger>(
        generateId(), _conf->getLogOnFile(), 
        _conf->getLogRootFolder());

    // Fill the qube data
    bool discFlag = _conf->isDiscoverEnabled();
    _qubeData = {false, discFlag, _isMaster, false, false, false};

    // Initialize the Qube interface
    _itf = std::make_shared<QubeInterface>(_conf, _logger);
}

void Qube::Qube::setMasterFlag(bool value)
{
    _isMaster = value;
}

bool Qube::Qube::isMaster() const
{
    return _isMaster;
}

bool Qube::Qube::isDiscoverEnabledAtStartup() const
{
    return _conf->isDiscoverEnabled();
}

void Qube::Qube::run()
{
    while (!_shutdownFlag)
    {
        switch (_stateMachine->getCurrentType())
        {
            case StateType::QUBE_INIT:
                init();
                break;
            
            default:
                break;
        }
    }
}
