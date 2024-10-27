#include "Receiver.hpp"

struct CommonLib::Communication::ReceivedData CommonLib::Communication::Receiver::handleReceivedMessages(
    unsigned char *buff, const std::size_t n, sockaddr_in *src
) {
    struct ReceivedData rdata;
    rdata.data = std::make_shared<ByteBuffer>(buff, n);
    rdata.src = src;
    return rdata;
}

bool CommonLib::Communication::Receiver::hasStopped() const
{
    return _stopped;
}

void CommonLib::Communication::UdpReceiver::receive()
{
    // Creates the buffer that will receives the data and initialize it to 0
    char buffer[RECVBUFFSIZE];
    memset(buffer, 0, sizeof(buffer));

    // Initiziale all the datas needed for the reception of a single message
    int fd = this->_socket.getSocketFileDescriptor();
    struct sockaddr_in src;
    socklen_t srclen = sizeof(src);
    ssize_t nofBytes;

    // If the reception has failed, we returns nothing
    if ((nofBytes = recvfrom(fd, buffer, sizeof(buffer), 0, 
                (struct sockaddr*)&src, &srclen
        )) < 0
    ) {
        std::cout << "[UdpReceiver::receive] Error when receiving data: ";
        std::cout << std::strerror(errno) << std::endl;
        return;
    }

    // We need to check whether the current receiver must be stopped
    // or not. When 0 bytes are received, like TCP, the receiver stop.
    if (nofBytes == 0)
    {
        this->_stopped = true;
        return;
    }

    // Otherwise we need the user-defined handleReceivedMessages function to
    // convert, in some way, the received message and put it in the queue.
    auto result = this->handleReceivedMessages((unsigned char*)buffer, nofBytes, &src);

    // If the sigstop is set to True then return
    if (this->_stopped) return;

    // Otherwise push the data into the queue
    this->_queue->push(result);
}

void CommonLib::Communication::TcpReceiver::receive()
{
    while (!this->_stopped)
    {
        // Initialize the buffer for receiving the data from the socket
        char buffer[RECVBUFFSIZE];
        memset(buffer, 0, sizeof(buffer));
        ssize_t nofBytes;

        // Receives the data from the socket. If there was an error when receiving
        // we need to print the error message and continue with the reception.
        if ((nofBytes = recv(_clientfd, buffer, sizeof(buffer), 0)) < 0)
        {
            // Take the IP and port of the client
            unsigned short port = ntohs(_client->sin_port);
            std::string ipaddr = Socket::addressNumberToString(_client->sin_addr.s_addr, true);
            std::cerr << "[TcpReceiver::receive] Error when receiving from IP ";
            std::cerr << ipaddr << " Port " << port << ": " << std::strerror(errno) << std::endl;
            continue;
        }

        // In the other case, if the number of received bytes is equal
        // to 0, this means that the client disconnected
        if (nofBytes == 1 || nofBytes == 0)
        {
            this->_stopped = true;
            break;
        }

        // Otherwise, we have received something and needs to convert it
        auto result = this->handleReceivedMessages((unsigned char*)buffer, nofBytes, _client);

        // Otherwise, push the received message into the queue
        this->_queue->push(result);
    }
}

void CommonLib::Communication::TcpReceiver::run()
{
    receive();
}

bool CommonLib::Communication::TcpReceiver::isRunning() const
{
    return !hasStopped();
}
