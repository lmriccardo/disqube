#include "QubeInterface.hpp"

void Qube::QubeInterface::initUdpInterface(const std::string &ip)
{
    _udpitf = std::make_shared<UdpCommunicationInterface>(
        ip, _conf->getUdpSenderPort(), _conf->getUdpListenerPort(), _conf->getUdpMaxCapacityQueue()
    );
}

void Qube::QubeInterface::initTcpInterface(const std::string &ip)
{
    _tcpitf = std::make_shared<TcpCommunicationInterface>(
        ip, _conf->getTcpSenderPort(), _conf->getTcpListenerPort(),
        _conf->getTcpMaxNumOfConnections(), _conf->getTcpMaxCapacityQueue(), 200, 0
    );
}

void Qube::QubeInterface::logInit()
{
    // Logging initialization
    _logger->info("Initializing the Qube Interface");
    
    // Logging UDP interface creation and initialization
    std::stringstream udp_ss;
    std::string itf = _conf->getNetworkInterface();
    unsigned short udp_sport = _conf->getUdpSenderPort();
    unsigned short udp_lport = _conf->getUdpListenerPort();

    udp_ss << "UDP Communication Interface binded" << std::endl;
    udp_ss << "\tNETWORK INTERFACE: " << itf;
    udp_ss << " SEND PORT: " << udp_sport;
    udp_ss << " LISTENING PORT: " << udp_lport;

    _logger->info(udp_ss.str());

    // Logging TCP interface creation and initialization
    unsigned short tcp_sport = _conf->getTcpSenderPort();
    unsigned short tcp_lport = _conf->getTcpListenerPort();
    std::stringstream tcp_ss;

    tcp_ss << "TCP Communication Interface binded" << std::endl;
    tcp_ss << "\tNETWORK INTERFACE: " << itf;
    tcp_ss << " SEND PORT: " << tcp_sport;
    tcp_ss << " LISTENING PORT: " << tcp_lport;

    _logger->info(tcp_ss.str());
}

void Qube::QubeInterface::init()
{
    std::string ip = Socket::getInterfaceIp(_conf->getNetworkInterface());
    initUdpInterface(ip); // Create Udp Communication Interface
    initTcpInterface(ip); // Create Tcp Communication Interface

    // Reserve space for the nodes vector
    _nodes.reserve(_conf->getMaxNumOfQubes());

    // Logging initialization
    logInit();
}

bool Qube::QubeInterface::isMaster()
{
    return _isMaster;
}

void Qube::QubeInterface::start()
{
    this->_logger->info("Starting UDP Communication Interface");
    this->_udpitf->start();

    this->_logger->info("Starting TCP Communication Interface");
    this->_tcpitf->start();
}

void Qube::QubeInterface::stop()
{
    this->_logger->info("Shutting down UDP Communication Interface");
    this->_udpitf->close();

    this->_logger->info("Shutting down TCP Communication Interface");
    this->_tcpitf->close();
}

void Qube::QubeInterface::qubeDiscovering()
{
    // Take the subnet configuration of the workers
    std::string    subnetAddr = _conf->getQubesSubnetAddress();
    std::string    subnetMask = _conf->getQubesSubnetMask();
    std::string    subnetGtwy = _conf->getQubesSubnetGateway();
    unsigned short workerPort = _conf->getQubesWorkerUdpPort();
    
    // Fill a structure with required informations to perform
    // the automatic scans of all IP addresses in the network.
    SubnetInfo info = Socket::getSubnetConfiguration(subnetAddr, subnetMask);
    unsigned int gatewayNum = Socket::addressStringToNumber(subnetGtwy);

    // Let's perform the discover
    std::stringstream ss;
    unsigned int sysNofBits = 32 - info.userNofBits;
    ss << "Initializing Discover Mode: Subnet " << subnetAddr;
    ss << "/" << sysNofBits << " Gateway " << subnetGtwy << std::endl;
    _logger->info(ss.str()); 

    unsigned short messageId = 0;

    auto discover_fn = [&](int x) {
        if (x == gatewayNum) return; // Skip the gateway address
        std::string addr_s = Socket::addressNumberToString(x, false);

        // Send the UDP Discover message to the currrent ip address
        CommonLib::Communication::DiscoverHelloMessage m_discover(messageId++, 0);
        m_discover.setUdpPort(_udpitf->getListenerPort());
        m_discover.setTcpPort(_tcpitf->getListenerPort());
        m_discover.setMessageProtocol(CommonLib::Communication::MessageProto::UDP);
        
        _udpitf->sendTo(addr_s, workerPort, m_discover);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    };

    Logging::ProgressBar::display(info.first, info.last + 1, 1, 
        "Seding Discover Hello Msg", discover_fn);
}

void Qube::QubeInterface::interfaceDiagnosticCheck()
{
    // Performs the diagnostic check on both interfaces
    this->_udpitf->performDiagnosticCheck();
    this->_tcpitf->performDiagnosticCheck();
}

DiagnosticCheckResult *Qube::QubeInterface::getUdpDiagnosticResult()
{
    return this->_udpitf->getDiagnosticResult();
}

DiagnosticCheckResult *Qube::QubeInterface::getTcpDiagnosticResult()
{
    return this->_tcpitf->getDiagnosticResult();
}
