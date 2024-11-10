#include "QubeInterface.hpp"

unsigned int Qube::QubeInterface::generateId()
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

void Qube::QubeInterface::initUdpInterface(const std::string &ip)
{
    _udpitf = std::make_shared<UdpCommunicationInterface>(
        ip, _conf.getUdpSenderPort(), _conf.getUdpListenerPort(), _conf.getUdpMaxCapacityQueue()
    );
}

void Qube::QubeInterface::initTcpInterface(const std::string &ip)
{
    _tcpitf = std::make_shared<TcpCommunicationInterface>(
        ip, _conf.getTcpSenderPort(), _conf.getTcpListenerPort(),
        _conf.getTcpMaxNumOfConnections(), _conf.getTcpMaxCapacityQueue(), 200, 0
    );
}

void Qube::QubeInterface::logInit()
{
    // Logging initialization
    _logger->info("Initializing the Qube Interface");
    
    // Logging UDP interface creation and initialization
    std::stringstream udp_ss;
    std::string itf = _conf.getNetworkInterface();
    unsigned short udp_sport = _conf.getUdpSenderPort();
    unsigned short udp_lport = _conf.getUdpListenerPort();

    udp_ss << "UDP Communication Interface binded" << std::endl;
    udp_ss << "\tNETWORK INTERFACE: " << itf << std::endl;
    udp_ss << "\tSEND PORT: " << udp_sport << std::endl;
    udp_ss << "\tLISTENING PORT: " << udp_lport;

    _logger->info(udp_ss.str());

    // Logging TCP interface creation and initialization
    unsigned short tcp_sport = _conf.getTcpSenderPort();
    unsigned short tcp_lport = _conf.getTcpListenerPort();
    std::stringstream tcp_ss;

    tcp_ss << "TCP Communication Interface binded" << std::endl;
    tcp_ss << "\tNETWORK INTERFACE: " << itf << std::endl;
    tcp_ss << "\tSEND PORT: " << tcp_sport << std::endl;
    tcp_ss << "\tLISTENING PORT: " << tcp_lport;

    _logger->info(tcp_ss.str());
}

void Qube::QubeInterface::init()
{
    std::string ip = Socket::getInterfaceIp(_conf.getNetworkInterface());
    initUdpInterface(ip); // Create Udp Communication Interface
    initTcpInterface(ip); // Create Tcp Communication Interface

    // Reserve space for the nodes vector
    _nodes.reserve(_conf.getMaxNumOfQubes());

    // Logging initialization
    logInit();
}

bool Qube::QubeInterface::isMaster()
{
    return _isMaster;
}
