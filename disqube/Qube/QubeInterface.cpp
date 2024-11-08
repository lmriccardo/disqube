#include "QubeInterface.hpp"

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
