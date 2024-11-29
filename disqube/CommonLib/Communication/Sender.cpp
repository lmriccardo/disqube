#include "Sender.hpp"

bool CommonLib::Communication::Sender::sendTo(const std::string &ip, const unsigned short port, Message &msg)
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
    return sendTo(ip, port, buff, nofBytes);
}

bool CommonLib::Communication::TcpSender::sendTo(
    const std::string &ip, const unsigned short port, unsigned char *buff, const std::size_t n
) {
    return _socket.sendTo(ip, port, buff, n);
}

void CommonLib::Communication::TcpSender::setTimeout(long int sec, long int usec)
{
    _socket.setTimeout(sec, usec);
}

void CommonLib::Communication::TcpSender::setTimeout(long int sec)
{
    _socket.setTimeout(sec);
}

void CommonLib::Communication::TcpSender::disconnect()
{
    // If the socket is not connected to any address then returns
    if (!_socket.isConnected()) return;
    
    std::string ip = _socket.getDestinationIp();
    unsigned short port = _socket.getDestinationPort();
    sendTo(ip, port, (unsigned char*)"1", 1);
}

bool CommonLib::Communication::UdpSender::sendTo(
    const std::string &ip, const unsigned short port, unsigned char *buff, const std::size_t n
) {
    struct sockaddr_in dst;
    memset(&dst, 0, sizeof(struct sockaddr_in));
    dst.sin_family = AF_INET;
    dst.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &dst.sin_addr);

    return _socket.send(buff, n, &dst);
}
