#include "Interface.hpp"

bool CommonLib::Communication::CommunicationInterface::isClosed()
{
    // Check if the sender socket is closed and the listening is not running
    return _sender->isSocketClosed() && !_listener->isRunning();
}

void CommonLib::Communication::CommunicationInterface::sendTo(const std::string &ip, unsigned short port, Message& msg)
{
    // Sends a message using the sender socket
    bool result = _sender->sendTo(ip, port, msg);
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

void CommonLib::Communication::CommunicationInterface::senderStop()
{
    this->_sender->closeSocket();
}

unsigned short CommonLib::Communication::CommunicationInterface::getSenderPort() const
{
    return _sender->getSocket().getPortNumber();
}

unsigned short CommonLib::Communication::CommunicationInterface::getListenerPort() const
{
    return _listener->getSocket().getPortNumber();
}

bool CommonLib::Communication::CommunicationInterface::performDiagnosticCheck()
{
    return true;
}

CommonLib::Communication::UdpCommunicationInterface::UdpCommunicationInterface(
    const std::string &ip, unsigned short sport, unsigned short lport, const std::size_t capacity
) : CommunicationInterface(capacity)
{
    _sender = std::make_shared<UdpSender>(ip, sport);
    _listener = std::make_shared<UdpListener>(ip, lport, _queue); // The receiver queue is the same
}

void CommonLib::Communication::UdpCommunicationInterface::close()
{
    // Stop the listener
    this->_listener->stop();
    
    // To stop the listening thread we need to send empty message
    std::string ip = _listener->getSocket().getIpAddress();
    unsigned short port = _listener->getSocket().getPortNumber();
    _sender->sendTo(ip, port, (unsigned char*)"", 0);

    // First close the sender socket
    if (!this->_sender->isSocketClosed()) this->_sender->closeSocket();

    // Then, close the receiver socket. Listeners, being
    // thread first needs to be stopped and then to be joined.
    // All listener threads are joinable, no check is needed
    this->_listener->join();
}

CommonLib::Communication::CommunicationInterface::~CommunicationInterface() {}

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

void CommonLib::Communication::TcpCommunicationInterface::close()
{
    // First close the sender socket
    if (!this->_sender->isSocketClosed()) this->_sender->closeSocket();

    // Stop the listener
    this->_listener->stop();

    // Then, close the receiver socket. Listeners, being
    // thread first needs to be stopped and then to be joined.
    // All listener threads are joinable, no check is needed
    this->_listener->join();
}

void CommonLib::Communication::TcpCommunicationInterface::disconnect()
{
    std::static_pointer_cast<TcpSender>(_sender)->disconnect();
}
