#ifndef _SOCKET_HPP
#define _SOCKET_HPP

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
#include <math.h>
#include <sys/select.h>
#include <poll.h>

#define TCPRECONNECTIONS 5
#define TCPTIMEOUT 1

namespace CommonLib::Communication
{
    enum SocketType
    {
        UDP,
        TCP,
        RAW,
        UNIX
    };

    struct SubnetInfo
    {
        unsigned int subnet;         // The subnet address
        unsigned short userNofBits;  // The number of bits reserved for the user
        unsigned int first;          // First usable address
        unsigned int last;           // Last usable address
        unsigned int broadcast;      // The broadcast address
        unsigned int nofUsable;      // The total number of usable address
    };

    struct SocketInfo
    {
        bool active;         // If the socket is still active
        bool ready_to_read;  // If the socket is ready to read data
        bool ready_to_write; // If the socket can write data
        bool socket_error;   // If there is any error regarding the socket
        bool timeout_ela;    // The timeout has elapsed
        int  error;          // The actual error (if socket_error is true, -1 otherwise)
    };

    class Socket
    {
        protected:
            std::string        _ip;     // The Ip address of the Socket
            unsigned short     _port;   // The port of the Socket
            struct sockaddr_in _src;    // The structure used when creating the socket
            struct SocketInfo  _info;   // Some info about the socket (errors, alive, etc .)
            int                _fd;     // Socket file descriptor
            SocketType         _type;   // The type of the socket (UDP, TCP)

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

            bool isSocketValid() const;
            void closeSocket();
            bool isClosed() const;
            const std::string& getIpAddress() const;
            unsigned short getPortNumber() const;
            int getSocketFileDescriptor() const;
            const struct sockaddr_in& getSource() const;
            void updateSocketInfo();
            struct SocketInfo* getSocketInfo();
            void flushSocketError();

            static std::string addressNumberToString(unsigned int addr, const bool be);
            static unsigned int addressStringToNumber(const std::string& addr);
            static std::string getHostnameIp(const std::string& hostname);
            static std::string getInterfaceIp(const std::string& interface);
            static std::string getBroadcastIp(const std::string& interface);
            static struct SubnetInfo getSubnetConfiguration(const std::string& addr, const std::string& mask);
            static void performDiagnosticCheck(int sockfd, struct SocketInfo* sockinfo);
    };

    class UdpSocket : public Socket
    {
        public:
            UdpSocket(const std::string& ip, const unsigned short port) \
                : Socket(ip, port, SocketType::UDP) {};

            UdpSocket(const UdpSocket& other) : Socket(other) {};

            bool send(unsigned char* buff, const std::size_t n, struct sockaddr_in* dst);
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
            void disconnect();
            void connectTo(const std::string& ip, const unsigned short port);
            std::string getDestinationIp() const;
            unsigned short getDestinationPort() const;
            const struct sockaddr_in& getDestination() const;
            
            bool sendTo(const std::string &ip, const unsigned short port, 
                unsigned char *buff, const std::size_t n);
    };
}

#endif