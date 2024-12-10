#include "Receiver.hpp"

using namespace Lib::Network;

struct ReceivedData Receiver::handleReceivedMessages(
    unsigned char *buff, const std::size_t n, sockaddr_in *src
) {
    struct ReceivedData rdata;
    rdata.data = std::make_shared<ByteBuffer>(buff, n);
    rdata.src = src;
    return rdata;
}

bool Receiver::hasStopped() const
{
    return _stopped;
}

void Receiver::stop()
{
    this->_stopped = true;
}

void UdpReceiver::receive()
{
    // If the sigstop is set to True then return
    if (this->_stopped) return;

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

    // Otherwise we need the user-defined handleReceivedMessages function to
    // convert, in some way, the received message and put it in the queue.
    auto result = this->handleReceivedMessages((unsigned char*)buffer, nofBytes, &src);

    // Otherwise push the data into the queue
    this->_queue->push(result);
}

void TcpReceiver::receive()
{
    char buffer[RECVBUFFSIZE];
    ssize_t nofBytes;
    struct Socket::SocketInfo si = {true, false, false, false, false, false, 0};

    while (!this->_stopped)
    {
        // Get the socket info for the client socket
        Socket::getSocketInfo(_clientfd, &si);

        // Check for any possible errors
        if (!si.active && si.socket_error)
        {
            std::cerr << strerror(si.error) << std::endl;
            break;
        }

        // Check for connection closed
        if (si.connecition_cld)
        {
            std::cout << "Connection closed on client socket ";
            std::cout << _clientfd << std::endl;
            break;
        }

        if (si.timeout_ela || !si.ready_to_read) continue;

        // Before going on we need to check if during the poll
        // an external call to stop the received have been made
        if (this->hasStopped()) break;

        // Initialize the buffer for receiving the data from the socket
        memset(buffer, 0, sizeof(buffer));

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
        if (nofBytes == 1 || nofBytes == 0) break;

        // Otherwise, we have received something and needs to convert it
        auto result = this->handleReceivedMessages((unsigned char*)buffer, nofBytes, _client);

        // Push the received message into the queue
        this->_queue->push(result);
    }

    this->_stopped = true;
    
    // Close the connection with the client
    // close(_clientfd);
}

void TcpReceiver::run()
{
    receive();
}

bool TcpReceiver::isRunning() const
{
    return !hasStopped();
}
