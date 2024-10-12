#include "Sender.hpp"

void CommonLib::Communication::Sender::sendTo(const std::string &ip, const unsigned short port, Message &msg)
{
    // Encode all the fields of the Message into the bytebuffer
    msg.encode();

    // Create the vector of bytes that will contains the buffer
    std::size_t nofBytes = msg.getBufferSize();
    unsigned char buff[nofBytes];
    memset(buff, 0, sizeof(buff));
    
    // Fill the byte vector with the content of the buffer 
    msg.getBuffer(buff, 0, nofBytes);

    // Send the message
    sendTo(ip, port, buff, nofBytes);
}

void CommonLib::Communication::TcpSender::sendTo(
    const std::string &ip, const unsigned short port, unsigned char *buff, const std::size_t n
) {
    // Try connection
    _socket.connectTo(ip, port);

    // Check if the connection was successfull, then send
    if (!_socket.isConnected()) return;
    if (send(_socket.getSocketFileDescriptor(), buff, n, 0) < 0)
    {
        std::cerr << "[TcpSender] Sent was unsuccessful: " << std::strerror(errno) << std::endl;
    }
}

void CommonLib::Communication::UdpSender::sendTo(
    const std::string &ip, const unsigned short port, unsigned char *buff, const std::size_t n
) {
    struct sockaddr_in dst;
    memset(&dst, 0, sizeof(struct sockaddr_in));
    dst.sin_family = AF_INET;
    dst.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &dst.sin_addr);

    if (sendto(_socket.getSocketFileDescriptor(), buff, n, 0, (struct sockaddr*)&dst, sizeof(dst)) < 0)
    {
        std::cerr << "[UdpSender] Sent was unsuccessful: " << std::strerror(errno) << std::endl;
    }
}
