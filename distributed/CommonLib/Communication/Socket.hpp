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
#include <fcntl.h>
#include <thread>

#define TCPRECONNECTIONS 5
#define TCPTIMEOUT 1

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
            bool               _connected;  // Flag indicating whether the socket is connected or not
            struct sockaddr_in _dst;        // The destination address
            int                _nreconn;    // The number of reconnections if previous attemps failed
            long int           _timeout_s;  // Timeout in seconds
            long int           _timeout_us; // Timeout in microsecods

            /**
             * Attemps a single connection to given destination and returns
             * True if the connection happened, False otherwise.
             */
            bool connectOne(struct sockaddr_in* dst);

        public:

            TcpSocket(const std::string& ip, const unsigned short port, const int nreconn) \
                : Socket(ip, port, SocketType::TCP), _nreconn(nreconn), _timeout_s(TCPTIMEOUT), 
                  _timeout_us(0) {};
            
            TcpSocket(const std::string& ip, const unsigned short port)
                : TcpSocket(ip, port, TCPRECONNECTIONS) {};

            TcpSocket(const TcpSocket& other) : Socket(other) 
            {
                _connected = other._connected;
                _dst = other._dst;
                _nreconn = other._nreconn;
                _timeout_s = other._timeout_s;
                _timeout_us = other._timeout_us;
            }

            void setNumberOfReconnections(int reconn);
            void setTimeout(long int sec, long int usec);
            void setTimeout(long int sec);
            struct timeval getTimeout();

            bool isConnected() const;
            void connectTo(const std::string& ip, const unsigned short port);
            std::string getDestinatioIp() const;
            unsigned short getDestinationPort() const;
            const struct sockaddr_in& getDestination() const;
    };
}

#endif