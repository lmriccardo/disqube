#include "Socket.hpp"

using namespace Lib::Network;

Socket::Socket(
    const std::string &ip, const unsigned short port, const SocketType &type
) : _ip(ip), _port(port), _type(type)
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

    // Performs some checks at socket creation
    updateSocketInfo();
}

Socket::Socket(const Socket &other)
{
    _ip = other._ip;
    _port = other._port;
    _type = other._type;
    _info = other._info;
    _src = other._src;
    _fd = other._fd;
}

bool Socket::isSocketValid() const
{
    return _info.active;
}

void Socket::closeSocket()
{
    shutdown(_fd, SHUT_RDWR);
    int ret = close(_fd);
    _info.active = false;
}

bool Socket::isClosed() const
{
    return !_info.active;
}

const std::string &Socket::getIpAddress() const
{
    return _ip;
}

unsigned short Socket::getPortNumber() const
{
    return _port;
}

int Socket::getSocketFileDescriptor() const
{
    return _fd;
}

const sockaddr_in &Socket::getSource() const
{
    return _src;
}

void Socket::updateSocketInfo()
{
    Socket::getSocketInfo(this->_fd, &this->_info);
}

Socket::SocketInfo *Socket::getSocketInfo()
{
    return &_info;
}

void Socket::flushSocketError()
{
    _info.socket_error = false;
    _info.error = 0;
}

std::string Socket::addressNumberToString(unsigned int addr, const bool be)
{
    // If it is in Little-Endian, then convert it to Big-Endian
    if (!be) addr = htonl(addr);
    
    char _addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr, _addr, INET_ADDRSTRLEN);
    return std::string(_addr);
}

unsigned int Socket::addressStringToNumber(const std::string &addr)
{
    struct sockaddr_in sa;

    // Convert the string IP to in_addr (network byte order)
    if (inet_pton(AF_INET, addr.c_str(), &(sa.sin_addr)) != 1)
    {
        throw std::invalid_argument("Invalid IP address format");
    }
    
    // Convert the in_addr to unsigned int (32-bit), from
    // network-byte order to host byte order (be -> le)
    return ntohl(sa.sin_addr.s_addr);
}

std::string Socket::getHostnameIp(const std::string &hostname)
{
    struct hostent *hostentry = gethostbyname(hostname.c_str());
    if (hostentry == NULL)
    {
        throw std::runtime_error("[Socket::getHostnameIp] GetHostByName call failed.");
    }

    return addressNumberToString((*((struct in_addr*) hostentry->h_addr_list[0])).s_addr, true);
}

struct Socket::SubnetInfo Socket::getSubnetConfiguration(
    const std::string &addr, const std::string &mask
) {
    struct SubnetInfo si; // Initialize the output structure
    
    // Convert both the subnet address and subnet mask into integer
    unsigned int addr_i = Socket::addressStringToNumber(addr);
    unsigned int mask_i = Socket::addressStringToNumber(mask);

    // Compute the broadcast address
    unsigned int brd_i = addr_i | (0xFFFFFFFF - mask_i);
    
    // Compute the number of usable addresses and user-reserved bits
    unsigned int usable = brd_i - (addr_i + 1);
    unsigned short user_bits = (unsigned short)(floor(log2(usable)) + 1);

    // Compute the first and last usable addresses
    unsigned int first = addr_i + 1;
    unsigned int last = brd_i - 1;

    // Fill the structure
    si.subnet = addr_i;
    si.broadcast = brd_i;
    si.first = first;
    si.last = last;
    si.nofUsable = usable;
    si.userNofBits = user_bits;

    return si;
}

void Socket::getSocketInfo(int sockfd, SocketInfo *sockinfo)
{
    // Before getting the info reset the structure
    Socket::resetSocketInfo(sockinfo);

    // Construct the pollfd struct for the pool system call
    struct pollfd pfd;
    pfd.fd = sockfd;
    pfd.events = POLLIN | POLLOUT | POLLERR | POLLNVAL | POLLHUP;

    int pollResult = poll(&pfd, 1, 1000); // Wait for any events to happen

    // If the timeout has expired than do nothing
    if (pollResult == 0)
    {
        sockinfo->timeout_ela = true;
        return;
    }

    // Otherwise, check for any errors and get the right one
    if (pollResult < 0 || (pfd.revents & POLLERR) || (pfd.revents & POLLNVAL))
    {
        sockinfo->socket_error = true;
        socklen_t errlen = sizeof(sockinfo->error);
        int result = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &sockinfo->error, &errlen);
        
        // Check for any errors regarding getsockopt. If there is any
        // then the sockinfo->error field is not set.
        if (result < 0) sockinfo->error = errno;
        sockinfo->active = false;
        return;
    }

    // Otherwise, check for disconnections
    if (pfd.revents & POLLHUP) sockinfo->connecition_cld = true;

    // Otherwise, check for readability
    if (pfd.revents & POLLIN) sockinfo->ready_to_read = true;

    // Finally, checks for writability
    if (pfd.revents & POLLOUT) sockinfo->ready_to_write = true;
}

void Socket::resetSocketInfo(SocketInfo *sockinfo)
{
    sockinfo->active = true;
    sockinfo->connecition_cld = false;
    sockinfo->error = 0;
    sockinfo->ready_to_read = false;
    sockinfo->ready_to_write = false;
    sockinfo->socket_error = false;
    sockinfo->timeout_ela = false;
}

std::string Socket::getInterfaceIp(const std::string &interface)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , interface.c_str(), IFNAMSIZ - 1);
    ioctl(sockfd, SIOCGIFADDR, &ifr);
    close(sockfd);

    uint32_t _addr_i = ( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr.s_addr;
    return addressNumberToString(_addr_i, true);
}

std::string Socket::getBroadcastIp(const std::string& interface)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , interface.c_str(), IFNAMSIZ - 1);
    ioctl(sockfd, SIOCGIFBRDADDR, &ifr);
    close(sockfd);

    uint32_t _addr_i = ( (struct sockaddr_in *)&ifr.ifr_broadaddr )->sin_addr.s_addr;
    return std::move(addressNumberToString(_addr_i, true));
}

void TcpSocket::setNumberOfReconnections(int reconn)
{
    _nreconn = reconn;
}

void TcpSocket::setTimeout(long int sec, long int usec)
{
    _timeout_s = sec;
    _timeout_us = usec;
}

void TcpSocket::setTimeout(long int sec)
{
    setTimeout(sec, 0);   
}

timeval TcpSocket::getTimeout()
{
    struct timeval timeout;
    timeout.tv_sec = _timeout_s;
    timeout.tv_usec = _timeout_us;
    return timeout;
}

bool TcpSocket::isConnected() const
{
    return _connected;
}

void TcpSocket::disconnect()
{
    _connected = false;
    memset(&_dst, 0, sizeof(struct sockaddr_in));
}

bool TcpSocket::connectOne(struct sockaddr_in* dst)
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

void TcpSocket::connectTo(const std::string &ip, const unsigned short port)
{
    // Check of socket sanity
    updateSocketInfo();

    // Check that the socket is still active, with no errors
    if (!_info.active || _info.socket_error)
    {
        _connected = false;
        return;
    }

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

std::string TcpSocket::getDestinationIp() const
{
    char addr[INET_ADDRSTRLEN];
    memset(addr, 0, sizeof(addr));
    inet_ntop(AF_INET, &_dst.sin_addr, addr, sizeof(addr));

    std::string ip_addr = addr;
    return ip_addr;
}

unsigned short TcpSocket::getDestinationPort() const
{
    return ntohs(_dst.sin_port);
}

const sockaddr_in &TcpSocket::getDestination() const
{
    return _dst;
}

bool TcpSocket::sendTo(const std::string &ip, const unsigned short port, unsigned char *buff, const std::size_t n)
{
    connectTo(ip, port); // Try connection with the endpoint

    // Check if the connection was successfull, then send
    if (!isConnected()) return false;
    if (send(_fd, buff, n, 0) < 0)
    {
        _info.socket_error = true;
        _info.error = errno;
        std::cerr << "[TcpSender] Sent was unsuccessful: " << std::strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool UdpSocket::send(unsigned char *buff, const std::size_t n, sockaddr_in *dst)
{
    updateSocketInfo(); // Socket sanity check

    // Check for the socket being active, ready to write and with no errors
    if (!(_info.active && _info.ready_to_write) || _info.socket_error)
    {
        return false;   
    }

    // Otherwise we can send and check for possible errors
    if (sendto(_fd, buff, n, 0, (struct sockaddr *)dst, sizeof(*dst)) < 0)
    {
        _info.socket_error = true;
        _info.error = errno;
        std::cerr << "[UdpSender] Sent was unsuccessful: " << std::strerror(errno) << std::endl;
        return false;
    }

    return true;
}
