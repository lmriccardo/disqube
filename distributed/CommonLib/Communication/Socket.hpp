#ifndef _SOCKET_HPP
#define _SOCKET_HPP

#pragma once

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <stdexcept>
#include <linux/if.h>
#include <sys/ioctl.h>

namespace CommonLib::Communication
{
    enum SocketType
    {
        UDP,
        TCP,
        RAW
    };

    class Socket
    {
        protected:
            std::string        _ip;     // The Ip address of the Socket
            unsigned short     _port;   // The port of the Socket
            struct sockaddr_in _src;    // The structure used when creating the socket
            int                _fd;     // Socket file descriptor
            SocketType         _type;   // The type of the socket (UDP, TCP)
            bool               _closed; // Flag indicating whether the socket is closed or not

        public:
            /**
             * Constructs the Socket class.
             * 
             * @throw std::invalid_argument if the type is not UDP nor TCP
             * @throw std::runtime_error If the socket creation has failed
             * @throw std::runtime_error If binding failed
             */
            Socket(const std::string& ip, const unsigned short port, const SocketType& type);
            Socket(const SocketType& type) : _type(type) {};
            Socket(const Socket& other);
            ~Socket()
            {
                closeSocket();
            }

            void closeSocket();
            bool isClosed() const;
            const std::string& getIpAddress() const;
            unsigned short getPortNumber() const;
            int getSocketFileDescriptor() const;
            const struct sockaddr_in& getSource() const;

            static std::string addressNumberToString(unsigned int addr, const bool be);
            static std::string getHostnameIp(const std::string& hostname);
            static std::string getInterfaceIp(const std::string& interface);
    };

    class UdpSocket : public Socket
    {
        public:
            UdpSocket(const std::string& ip, const unsigned short port) \
                : Socket(ip, port, SocketType::UDP) {};

            UdpSocket(const UdpSocket& other) : Socket(other) {};
    };

    class TcpSocket : public Socket
    {
        private:
            bool               _connected; // Flag indicating whether the socket is connected or not
            struct sockaddr_in _dst;       // The destination address

        public:
            TcpSocket(const std::string& ip, const unsigned short port) \
                : Socket(ip, port, SocketType::TCP) {};
            
            TcpSocket(const TcpSocket& other) : Socket(other) 
            {
                _connected = other._connected;
                _dst = other._dst;
            }

            bool isConnected() const;
            void connectTo(const std::string& ip, const unsigned short port);
            std::string getDestinatioIp() const;
            unsigned short getDestinationPort() const;
            const struct sockaddr_in& getDestination() const;
    };
}

#endif