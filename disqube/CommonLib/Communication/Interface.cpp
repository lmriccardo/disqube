#include "Interface.hpp"

void CommonLib::Communication::CommunicationInterface::close()
{
    // First close the sender socket
    this->_sender->closeSocket();

    // Then, close the receiver socket. Listeners, being
    // thread first needs to be stopped and then to be joined.
    // All listener threads are joinable, no check is needed
    this->_listener->stop();
    this->_listener->join();
}

bool CommonLib::Communication::CommunicationInterface::isClosed()
{
    // Check if the sender socket is closed and the listening is not running
    return _sender->isSocketClosed() && !_listener->isRunning();
}

void CommonLib::Communication::CommunicationInterface::sendTo(const std::string &ip, unsigned short port, Message& msg)
{
    // Sends a message using the sender socket
    _sender->sendTo(ip, port, msg);
}

CommonLib::Communication::ReceivedData CommonLib::Communication::CommunicationInterface::getReceivedElement()
{
    // Pop an element from the receiver queue (no priority involved)
    return _queue->pop();
}

void CommonLib::Communication::CommunicationInterface::start()
{
    _listener->start();
}

CommonLib::Communication::UdpCommunicationInterface::UdpCommunicationInterface(
    const std::string &ip, unsigned short sport, unsigned short lport, const std::size_t capacity
) : CommunicationInterface(capacity)
{
    _sender = std::make_shared<UdpSender>(ip, sport);
    _listener = std::make_shared<UdpListener>(ip, lport, _queue); // The receiver queue is the same
}

CommonLib::Communication::TcpCommunicationInterface::TcpCommunicationInterface(
    const std::string &ip, unsigned short sport, unsigned short lport, const std::size_t nconn, 
    const std::size_t capacity, long int timesec, long int timeusec
) : CommunicationInterface(capacity)
{
    _sender = std::make_shared<TcpSender>(ip, sport);
    _listener = std::make_shared<TcpListener>(ip, lport, _queue, nconn); // The receiver queue is the same

    // Set the timeout in seconds and microseconds
    std::static_pointer_cast<TcpListener>(_listener)->setTimeout(timesec, timeusec);
}
