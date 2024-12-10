#include "Interface.hpp"

using namespace Lib::Network;

bool CommunicationInterface::isClosed()
{
    // Check if the sender socket is closed and the listening is not running
    return _sender->isSocketClosed() && !_listener->isRunning();
}

void CommunicationInterface::sendTo(const std::string &ip, unsigned short port, Message& msg)
{
    // Sends a message using the sender socket
    bool result = _sender->sendTo(ip, port, msg);
}

ReceivedData CommunicationInterface::getReceivedElement()
{
    // Pop an element from the receiver queue (no priority involved)
    return _queue->pop();
}

void CommunicationInterface::start()
{
    _listener->start();
}

void CommunicationInterface::senderStop()
{
    this->_sender->closeSocket();
}

unsigned short CommunicationInterface::getSenderPort() const
{
    return _sender->getSocket().getPortNumber();
}

unsigned short CommunicationInterface::getListenerPort() const
{
    return _listener->getSocket().getPortNumber();
}

void CommunicationInterface::performDiagnosticCheck()
{
    this->zeroDiagnosticCheck();

    // Fill up the Listener fields
    this->_check.listener_exitOnError = this->_listener->hasStoppedWithErrors();
    this->_check.listener_isRunning = this->_listener->isRunning();
    this->_check.listener_sockError = this->_listener->getSocketError();

    // Fill the Sender fields
    this->_check.sender_sockError = this->_sender->getSocket().getSocketInfo()->error;
}

DiagnosticCheckResult *CommunicationInterface::getDiagnosticResult()
{
    return &this->_check;
}

UdpCommunicationInterface::UdpCommunicationInterface(
    const std::string &ip, unsigned short sport, unsigned short lport, const std::size_t capacity
) : CommunicationInterface(capacity)
{
    _sender = std::make_shared<UdpSender>(ip, sport);
    _listener = std::make_shared<UdpListener>(ip, lport, _queue); // The receiver queue is the same
}

void UdpCommunicationInterface::close()
{
    // Stop the listener
    this->_listener->stop();

    // First close the sender socket
    if (!this->_sender->isSocketClosed()) this->_sender->closeSocket();

    // Then, close the receiver socket. Listeners, being
    // thread first needs to be stopped and then to be joined.
    // All listener threads are joinable, no check is needed
    this->_listener->join();
}

void CommunicationInterface::zeroDiagnosticCheck()
{
    this->_check.listener_exitOnError = false;
    this->_check.listener_isRunning = true;
    this->_check.listener_sockError = 0;
    this->_check.sender_sockError = 0;
}

CommunicationInterface::~CommunicationInterface() {}

TcpCommunicationInterface::TcpCommunicationInterface(
    const std::string &ip, unsigned short sport, unsigned short lport, const std::size_t nconn, 
    const std::size_t capacity, long int timesec, long int timeusec
) : CommunicationInterface(capacity)
{
    _sender = std::make_shared<TcpSender>(ip, sport);
    _listener = std::make_shared<TcpListener>(ip, lport, _queue, nconn); // The receiver queue is the same

    // Set the timeout in seconds and microseconds
    std::static_pointer_cast<TcpListener>(_listener)->setTimeout(timesec, timeusec);
}

void TcpCommunicationInterface::close()
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

void TcpCommunicationInterface::disconnect()
{
    std::static_pointer_cast<TcpSender>(_sender)->disconnect();
}
