#include "Qube.hpp"

namespace sm   = Qube::StateManager;
namespace net  = Lib::Network;
namespace conc = Lib::Concurrency;
namespace sys  = Lib::System;

unsigned int Qube::Qube::generateId()
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm *localTime = std::localtime(&currentTime);

    // Take hours, minutes and seconds and returns the id
    int hour = localTime->tm_hour;
    int minutes = localTime->tm_min;
    int seconds = localTime->tm_sec;
    return (unsigned int)((hour << 16) + (minutes << 8) + seconds);
}

void Qube::Qube::initStateMachine()
{
    sm::State_ptr s0 = std::make_shared<sm::State>(sm::State::StateType::QUBE_INIT, 3);
    sm::State_ptr s1 = std::make_shared<sm::State>(sm::State::StateType::QUBE_DISCOVERING, 2);
    sm::State_ptr s2 = std::make_shared<sm::State>(sm::State::StateType::QUBE_OPERATIVE, 3);
    sm::State_ptr s3 = std::make_shared<sm::State>(sm::State::StateType::QUBE_MAINTENANCE, 2);
    sm::State_ptr s4 = std::make_shared<sm::State>(sm::State::StateType::QUBE_SHUTDOWN, 1);

    s0->addTransition(s4, [](sm::Transition::Input_t p)
                      { return p.shutdown; });
    s0->addTransition(s1, [](sm::Transition::Input_t p)
                      { return p.itfReady && (p.discoverFlag && p.isMaster); });
    s0->addTransition(s2, [](sm::Transition::Input_t p)
                      { return p.itfReady && !(p.discoverFlag && p.isMaster); });
    s1->addTransition(s4, [](sm::Transition::Input_t p)
                      { return p.shutdown; });
    s1->addTransition(s2, [](sm::Transition::Input_t p)
                      { return p.anyWorker; });
    s2->addTransition(s4, [](sm::Transition::Input_t p)
                      { return p.shutdown; });
    s2->addTransition(s3, [](sm::Transition::Input_t p)
                      { return p.maintenance; });
    s2->addTransition(s1, [](sm::Transition::Input_t p)
                      { return !p.anyWorker && p.isMaster; });
    s3->addTransition(s4, [](sm::Transition::Input_t p)
                      { return p.shutdown; });
    s3->addTransition(s2, [](sm::Transition::Input_t p)
                      { return !p.maintenance; });

    _stateMachine = std::make_shared<sm::StateMachine>(s0);
}

int Qube::Qube::checkDiagnosticResults()
{
    memset(&this->_error, 0, sizeof(this->_error));
    this->_itf->interfaceDiagnosticCheck();

    net::DiagnosticCheckResult *udpresult = this->_itf->getUdpDiagnosticResult();
    net::DiagnosticCheckResult *tcpresult = this->_itf->getTcpDiagnosticResult();

    int result = 0;

    // First let's check for UDP results
    if (udpresult->listener_exitOnError)
    {
        result = result + UDP_LISTENER_RUNNING;
        this->_error.udperror_l = udpresult->listener_sockError;
    }

    if (udpresult->sender_sockError)
    {
        result = result + UDP_SENDER_ERROR;
        this->_error.udperror_s = udpresult->sender_sockError;
    }

    // Then we need to check for TCP results
    if (tcpresult->listener_exitOnError)
    {
        result = result + TCP_LISTENER_RUNNING;
        this->_error.tcperror_l = tcpresult->listener_sockError;
    }

    if (tcpresult->sender_sockError)
    {
        result = result + TCP_SENDER_ERROR;
        this->_error.tcperror_s = tcpresult->sender_sockError;
    }

    return result;
}

void Qube::Qube::handleDiagnosticErrors(const int result)
{
    std::stringstream ss;
    ss << "Diagnostic Check detected the following errors: " << std::endl;

    if (result & UDP_LISTENER_RUNNING)
    {
        // If the UDP listener has stop running
        ss << "- UDP Listener exited with error: ";
        ss << strerror(_error.udperror_l) << std::endl;
    }

    if (result & UDP_SENDER_ERROR)
    {
        ss << "- UDP Sender raise the following error: ";
        ss << strerror(_error.udperror_s) << std::endl;
    }

    if (result & TCP_LISTENER_RUNNING)
    {
        // If the TCP listener has stop running
        ss << "- TCP Listener exited with error: ";
        ss << strerror(_error.tcperror_l) << std::endl;
    }

    if (result & TCP_SENDER_ERROR)
    {
        ss << "- TCP Sender raise the following error: ";
        ss << strerror(_error.tcperror_s) << std::endl;
    }

    this->_logger->error(ss.str());
}

void Qube::Qube::init()
{
    // Load the configuration
    _conf = std::make_shared<Configuration::DisqubeConfiguration>(_confFile);

    // Create the logger
    _logger = std::make_shared<Logging::DisqubeLogger>(
        generateId(), _conf->getLogOnFile(),
        _conf->getLogRootFolder());

    // Fill the qube data
    bool discFlag = _conf->isDiscoverEnabled();
    _qubeData = {false, discFlag, _isMaster, false, false, false};

    // Initialize the Qube interface
    _itf = std::make_shared<QubeInterface>(_conf, _logger);
    _itf->start();

    // Perform a first diagnostic check
    int result = this->checkDiagnosticResults();
    if (result == 0)
    {
        // If there are no errors we can continue to next step
        this->_logger->info("The Qube Is Ready to proceed.");
        this->_qubeData.itfReady = true;
        this->_stateMachine->update(this->_qubeData);

        // Create the timer
        unsigned int time_us = _conf->getReceptionTimer_ms() * 1000;
        _timer = std::make_shared<conc::WakeUpTimer>(time_us);
        _timer->start();
        return;
    }

    // If there are errors we need to handle them
    this->handleDiagnosticErrors(result);
    this->_qubeData.shutdown = true;
    this->_stateMachine->update(this->_qubeData);
}

void Qube::Qube::shutdown()
{
    this->_logger->info("Starting to shutdown the Qube Manager");
    this->_itf->stop();
    this->_shutdownFlag = true;
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
        case sm::State::StateType::QUBE_INIT:
            init();
            break;
        case sm::State::StateType::QUBE_DISCOVERING:
            discover();
            break;
        case sm::State::StateType::QUBE_OPERATIVE:
            operative();
            break;
        case sm::State::StateType::QUBE_SHUTDOWN:
            shutdown();
            break;
        default:
            break;
        }
    }
}

void Qube::QubeManager::discover()
{
    this->_itf->qubeDiscovering(); // Perform Qube discovering
    this->_timer->resetTimeout();  // Reset the timeout for waiting discover responses

    while (!this->_timer->checkTimeout(1000))
    {
        this->_timer->wait(); // Wait for the wake up signal
        MessageIterator it = this->_itf->receiveAllMessage(); // Receives all messages after wake up

        // Process all received messages
        for (auto message : it)
        {
            this->processMessage(message);
        }
    }

    this->_qubeData.shutdown = true;
    this->_stateMachine->update(this->_qubeData);
}

void Qube::QubeManager::operative()
{
}

void Qube::QubeManager::processMessage(const net::ReceivedData &recvData)
{
    net::ByteBuffer_ptr buffer = recvData.data;
    struct sockaddr_in *src = recvData.src;

    // Needs to check the message subtype
    switch (net::Message::fetchMessageSubType(buffer))
    {
    case net::Message::MessageSubType::DISCOVER_RESPONSE:
        handleDiscoverResponse(buffer);
        break;

    default:
        break;
    }
}

void Qube::QubeManager::handleDiscoverResponse(Lib::Network::ByteBuffer_ptr &buffer)
{
    net::DiscoverResponseMessage m_response(*buffer); // Decode the ByteBuffer into the message

    // Take some informations and print them ... for now
    std::cout << "Received a Reponse from ("
              << net::Socket::addressNumberToString(m_response.getIpAddress(), false)
              << ", " << m_response.getUdpPort() << ")"
              << " With following features: "
              << "Free RAM: " << m_response.getAvailableMemory() << " KB and "
              << "CPU Usage: " << static_cast<unsigned int>(m_response.getCpuUsage()) << " %"
              << std::endl;
}

void Qube::QubeWorker::operative()
{
    this->_timer->resetTimeout();

    while (1)
    {
        this->_timer->wait(); // Wait for the wake up signal
        MessageIterator it = this->_itf->receiveAllMessage(); // Receives all messages after wake up

        // Process all received messages
        for (auto message : it)
        {
            this->processMessage(message);
        }
    }
}

void Qube::QubeWorker::processMessage(const net::ReceivedData &recvData)
{
    net::ByteBuffer_ptr buffer = recvData.data;
    struct sockaddr_in *src = recvData.src;

    // Needs to check the message subtype
    switch (net::Message::fetchMessageSubType(buffer))
    {
    case net::Message::MessageSubType::DISCOVER_HELLO:
        handleDiscoverHello(buffer);
        break;

    default:
        break;
    }
}

void Qube::QubeWorker::handleDiscoverHello(net::ByteBuffer_ptr &buffer)
{
    net::DiscoverHelloMessage dhm(*buffer);

    // Take the data contained into the bytebuffer
    m_QubeMasterInfo.udp_port = dhm.getUdpPort();
    m_QubeMasterInfo.tcp_port = dhm.getTcpPort();
    m_QubeMasterInfo.addr = dhm.getIpAddress();
    
    // Load the System metrics to put them into the final message
    struct sys::SystemMetrics metrics;
    sys::collect(&metrics, 100);

    // Log the reception
    std::stringstream ss;
    ss << "Received DISCOVER HELLO Message From ("
       << net::Socket::addressNumberToString(m_QubeMasterInfo.addr, false)
       << ", " << m_QubeMasterInfo.udp_port << ") Sending response ... " 
       << std::endl;

    _logger->info(ss.str());

    // Sends the Discover response message
    this->_itf->sendDiscoverResponse(&metrics, dhm.getMessageCounter(), 
        dhm.getMessageId(), &m_QubeMasterInfo);
}
