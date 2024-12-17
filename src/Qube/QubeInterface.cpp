#include "QubeInterface.hpp"

namespace net = Lib::Network;
namespace sys = Lib::System;

using namespace Qube;

void QubeMessageReceiver::getFromUdpInterface()
{
    try
    {
        struct net::ReceivedData data = this->_udpitf->getReceivedElement();
        this->_queue->push(data);
    }
    catch (const std::runtime_error &re)
    {
        return;
    }
}

void QubeMessageReceiver::getFromTcpInterface()
{
    try
    {
        struct net::ReceivedData data = this->_tcpitf->getReceivedElement();
        this->_queue->push(data);
    }
    catch (const std::runtime_error &re)
    {
        return;
    }
}

void QubeMessageReceiver::run()
{
    while (this->isRunning())
    {
        this->getFromUdpInterface();
        this->getFromTcpInterface();
    }
}

bool QubeMessageReceiver::isRunning() const
{
    return !this->_sigstop;
}

void QubeMessageReceiver::stop()
{
    this->_sigstop = true;
    if (this->isJoinable())
        this->join();
}

net::ReceivedData QubeMessageReceiver::getReceivedData()
{
    return this->_queue->pop();
}

const std::size_t QubeMessageReceiver::getCurrentQueueSize() const
{
    return this->_queue->getNofElements();
}

void QubeInterface::initUdpInterface(const std::string &ip)
{
    _udpitf = std::make_shared<net::UdpCommunicationInterface>(
        ip, _conf->getUdpSenderPort(), _conf->getUdpListenerPort(), _conf->getUdpMaxCapacityQueue());
}

void QubeInterface::initTcpInterface(const std::string &ip)
{
    _tcpitf = std::make_shared<net::TcpCommunicationInterface>(
        ip, _conf->getTcpSenderPort(), _conf->getTcpListenerPort(),
        _conf->getTcpMaxNumOfConnections(), _conf->getTcpMaxCapacityQueue(), 200, 0);
}

void QubeInterface::logInit()
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

void QubeInterface::init()
{
    std::string ip = net::Socket::getInterfaceIp(_conf->getNetworkInterface());
    initUdpInterface(ip); // Create Udp Communication Interface
    initTcpInterface(ip); // Create Tcp Communication Interface

    // Creates the message receiver with the tcp and udp inteface
    this->_receiver = std::make_shared<QubeMessageReceiver>(_udpitf, _tcpitf);

    // Logging initialization
    logInit();
}

bool QubeInterface::isMaster()
{
    return _isMaster;
}

void QubeInterface::start()
{
    this->_logger->info("Starting UDP Communication Interface");
    this->_udpitf->start();

    this->_logger->info("Starting TCP Communication Interface");
    this->_tcpitf->start();

    this->_logger->info("Starting Qube Message Receiver");
    this->_receiver->start();
}

void QubeInterface::stop()
{
    this->_logger->info("Shutting down UDP Communication Interface");
    this->_udpitf->close();

    this->_logger->info("Shutting down TCP Communication Interface");
    this->_tcpitf->close();

    this->_logger->info("Stopping the Qube Message Receiver");
    this->_receiver->stop();
}

void QubeInterface::qubeDiscovering()
{
    // Take the subnet configuration of the workers
    std::string subnetAddr = _conf->getQubesSubnetAddress();
    std::string subnetMask = _conf->getQubesSubnetMask();
    std::string subnetGtwy = _conf->getQubesSubnetGateway();
    unsigned short workerPort = _conf->getQubesWorkerUdpPort();

    // Fill a structure with required informations to perform
    // the automatic scans of all IP addresses in the network.
    net::Socket::SubnetInfo info = net::Socket::getSubnetConfiguration(subnetAddr, subnetMask);
    unsigned int gatewayNum = net::Socket::addressStringToNumber(subnetGtwy);

    // Let's perform the discover
    std::stringstream ss;
    unsigned int sysNofBits = 32 - info.userNofBits;
    ss << "Initializing Discover Mode: Subnet " << subnetAddr;
    ss << "/" << sysNofBits << " Gateway " << subnetGtwy << std::endl;
    _logger->info(ss.str());

    unsigned short messageId = 0;

    std::string ip = net::Socket::getInterfaceIp(_conf->getNetworkInterface());
    unsigned int ipaddr = net::Socket::addressStringToNumber(ip);

    auto discover_fn = [&](int x)
    {
        if (x == gatewayNum)
            return; // Skip the gateway address
        std::string addr_s = net::Socket::addressNumberToString(x, false);

        // Send the UDP Discover message to the currrent ip address
        net::DiscoverHelloMessage m_discover(messageId++, 0);
        m_discover.setUdpPort(_udpitf->getListenerPort());
        m_discover.setTcpPort(_tcpitf->getListenerPort());
        m_discover.setIpAddress(ipaddr);
        m_discover.setMessageProtocol(net::Message::MessageProto::UDP);

        _udpitf->sendTo(addr_s, workerPort, m_discover);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    };

    Logging::ProgressBar::display(info.first, info.last + 1, 1, "Seding Discover Hello Msg", discover_fn);
}

void QubeInterface::interfaceDiagnosticCheck()
{
    // Performs the diagnostic check on both interfaces
    this->_udpitf->performDiagnosticCheck();
    this->_tcpitf->performDiagnosticCheck();
}

net::DiagnosticCheckResult *QubeInterface::getUdpDiagnosticResult()
{
    return this->_udpitf->getDiagnosticResult();
}

net::DiagnosticCheckResult *QubeInterface::getTcpDiagnosticResult()
{
    return this->_tcpitf->getDiagnosticResult();
}

MessageIterator QubeInterface::receiveAllMessage()
{
    const std::size_t currSize = this->_receiver->getCurrentQueueSize();
    return MessageIterator(0, currSize, this->_receiver);
}

void QubeInterface::sendDiscoverResponse(const sys::SystemMetrics *metrics, 
    const unsigned short counter, const unsigned short id, 
    const QubeMasterInfo* master
) {
    // Create the Discover Response message
    net::DiscoverResponseMessage response(id, counter + 1);
    response.setUdpPort(_udpitf->getListenerPort());
    response.setTcpPort(_tcpitf->getListenerPort());

    std::string ip = net::Socket::getInterfaceIp(_conf->getNetworkInterface());
    unsigned int ipaddr = net::Socket::addressStringToNumber(ip);
    response.setIpAddress(ipaddr);
    
    unsigned int ram_mb = metrics->pram_free / (1024 * 1024);
    unsigned int ram_kb = metrics->pram_free / 1024 - ram_mb * 1000;
    response.setAvailableMemory_mb(ram_mb);
    response.setAvailableMemory_kb(ram_kb);
    response.setCpuUsage(static_cast<unsigned char>(metrics->cpu_usage));
    response.setMessageProtocol(net::Message::MessageProto::UDP);

    // Sends the message using the UDP socket
    ip = net::Socket::addressNumberToString(master->addr, false);
    _udpitf->sendTo(ip, master->udp_port, response);
}
