#include "Socket.hpp"

CommonLib::Communication::Socket::Socket(
    const std::string &ip, const unsigned short port, const SocketType &type
) : _ip(ip), _port(port), _type(type), _closed(false)
{
    // First construct the socket and get the file descriptor
    struct protoent *prot;
    int sockType;

    switch (_type)
    {
        case SocketType::UDP:
            prot = getprotobyname("udp");
            sockType = SOCK_DGRAM;
            break;
        case SocketType::TCP:
            prot = getprotobyname("tcp");
            sockType = SOCK_STREAM;
            break;
        default:
            throw std::invalid_argument(
                "[Socket] Input type must be either TCP or UDP"
            );
    }

    // Handle socket creation failure
    if ((_fd = socket(AF_INET, sockType, prot->p_proto)) < 0)
    {
        std::cerr << std::strerror(errno) << std::endl;
        throw std::runtime_error("[Socket creation] Failed socket creation: ");
    }

    // Set the structure values
    memset(&_src, 0, sizeof(_src));
    _src.sin_family = AF_INET;
    _src.sin_port = htons(_port);
    inet_pton(AF_INET, _ip.c_str(), &_src.sin_addr);

#ifdef REUSE_MODE
    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << std::strerror(errno) << std::endl;
        closeSocket();
        throw std::runtime_error("[Socket setsockopt] Failed to set reusable address");
    }

    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        std::cerr << std::strerror(errno) << std::endl;
        closeSocket();
        throw std::runtime_error("[Socket setsockopt] Failed to set reusable port");
    }
#endif

    // Try to bind, if it is unsuccessfull than throws an exception
    // and close the socket previously created.
    if (bind(_fd, (struct sockaddr*)&_src, sizeof(_src)) < 0)
    {
        std::cerr << std::strerror(errno) << std::endl;
        closeSocket();
        throw std::runtime_error("[Socket binding] Binding failed.");
    }
}

CommonLib::Communication::Socket::Socket(const Socket &other)
{
    _ip = other._ip;
    _port = other._port;
    _type = other._type;
    _closed = other._closed;
    _src = other._src;
    _fd = other._fd;
}

bool CommonLib::Communication::Socket::isSocketValid() const
{
    return fcntl(_fd, F_GETFD) != -1 || errno != EBADF;
}

void CommonLib::Communication::Socket::closeSocket()
{
    shutdown(_fd, SHUT_RDWR);
    int ret = close(_fd);
    _closed = true;
}

bool CommonLib::Communication::Socket::isClosed() const
{
    return _closed;
}

const std::string &CommonLib::Communication::Socket::getIpAddress() const
{
    return _ip;
}

unsigned short CommonLib::Communication::Socket::getPortNumber() const
{
    return _port;
}

int CommonLib::Communication::Socket::getSocketFileDescriptor() const
{
    return _fd;
}

const sockaddr_in &CommonLib::Communication::Socket::getSource() const
{
    return _src;
}

std::string CommonLib::Communication::Socket::addressNumberToString(unsigned int addr, const bool be)
{
    // If it is in Little-Endian, then convert it to Big-Endian
    if (!be) addr = htonl(addr);
    
    char _addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr, _addr, INET_ADDRSTRLEN);
    return std::string(_addr);
}

std::string CommonLib::Communication::Socket::getHostnameIp(const std::string &hostname)
{
    struct hostent *hostentry = gethostbyname(hostname.c_str());
    if (hostentry == NULL)
    {
        throw std::runtime_error("[Socket::getHostnameIp] GetHostByName call failed.");
    }

    return addressNumberToString((*((struct in_addr*) hostentry->h_addr_list[0])).s_addr, true);
}

std::string CommonLib::Communication::Socket::getInterfaceIp(const std::string &interface)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , interface.c_str(), IFNAMSIZ - 1);
    ioctl(sockfd, SIOCGIFADDR, &ifr);
    close(sockfd);

    u_int32_t _addr_i = ( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr.s_addr;
    return addressNumberToString(_addr_i, true);
}

std::string CommonLib::Communication::Socket::getBroadcastIp(const std::string& interface)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , interface.c_str(), IFNAMSIZ - 1);
    ioctl(sockfd, SIOCGIFBRDADDR, &ifr);
    close(sockfd);

    u_int32_t _addr_i = ( (struct sockaddr_in *)&ifr.ifr_broadaddr )->sin_addr.s_addr;
    return std::move(addressNumberToString(_addr_i, true));
}

void CommonLib::Communication::TcpSocket::setNumberOfReconnections(int reconn)
{
    _nreconn = reconn;
}

void CommonLib::Communication::TcpSocket::setTimeout(long int sec, long int usec)
{
    _timeout_s = sec;
    _timeout_us = usec;
}

void CommonLib::Communication::TcpSocket::setTimeout(long int sec)
{
    setTimeout(sec, 0);   
}

timeval CommonLib::Communication::TcpSocket::getTimeout()
{
    struct timeval timeout;
    timeout.tv_sec = _timeout_s;
    timeout.tv_usec = _timeout_us;
    return timeout;
}

bool CommonLib::Communication::TcpSocket::isConnected() const
{
    return _connected;
}

void CommonLib::Communication::TcpSocket::disconnect()
{
    _connected = false;
    memset(&_dst, 0, sizeof(struct sockaddr_in));
}

bool CommonLib::Communication::TcpSocket::connectOne(struct sockaddr_in* dst)
{
    // Set the socket to non-blocking mode
    int flags = fcntl(_fd, F_GETFL, 0);
    fcntl(_fd, F_SETFL, flags | O_NONBLOCK);

    // Try connection to the destination server. Since the socket is now
    // non-blocking it will instantly returns a result. Notice that the
    // result can be -1, and it is OKAY only if the corresponding ERRNO
    // is set to EINPROGRESS.
    int result = connect(_fd, (struct sockaddr*)&_dst, sizeof(_dst));
    if (result < 0 && errno != EINPROGRESS) 
    {
        // It can be that the socket is already connected
        if (errno == EISCONN) return true;
        return false;
    }

    // Let's create the set of file descriptors the select() operation
    // will look into, to see if they are accessible for R/W operations.
    // Read and Write FD sets are different, and we only need to set
    // Write FD, since our last objective is to be able to send data.
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(_fd, &writefds);

    // Set the timeout for the select operation
    struct timeval timeout;
    timeout.tv_sec = _timeout_s;
    timeout.tv_usec = _timeout_us;

    // Perform the select syscall and wait for the writable socket to be ready
    if ((result = select(_fd + 1, nullptr, &writefds, nullptr, &timeout)) <= 0)
    {
        return false;
    }

    // Check for connection success or error using getsockopt
    int sock_error;
    socklen_t len = sizeof(sock_error);
    if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, &sock_error, &len) < 0 || sock_error != 0) 
    {
        return false;
    }

    // Reset socket to blocking mode after successful connection
    fcntl(_fd, F_SETFL, flags);
    return true;
}

void CommonLib::Communication::TcpSocket::connectTo(const std::string &ip, const unsigned short port)
{
    memset(&_dst, 0, sizeof(_dst));
    _dst.sin_family = AF_INET;
    _dst.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &_dst.sin_addr);

    int reconn_counter = 0;
    while (reconn_counter < _nreconn)
    {
        if (connectOne(&_dst))
        {
            _connected = true;
            errno = EXIT_SUCCESS;
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        reconn_counter++;
    }

    _connected = false;
}

std::string CommonLib::Communication::TcpSocket::getDestinatioIp() const
{
    char addr[INET_ADDRSTRLEN];
    memset(addr, 0, sizeof(addr));
    inet_ntop(AF_INET, &_dst.sin_addr, addr, sizeof(addr));

    std::string ip_addr = addr;
    return ip_addr;
}

unsigned short CommonLib::Communication::TcpSocket::getDestinationPort() const
{
    return ntohs(_dst.sin_port);
}

const sockaddr_in &CommonLib::Communication::TcpSocket::getDestination() const
{
    return _dst;
}
