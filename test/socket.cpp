#include <iostream>
#include <CommonLib/Communication/Socket.hpp>
#include "Test.hpp"

using Socket = CommonLib::Communication::Socket;
using UdpSocket = CommonLib::Communication::UdpSocket;
using TcpSocket = CommonLib::Communication::TcpSocket;
using SocketType = CommonLib::Communication::SocketType;
using SocketInfo = CommonLib::Communication::SocketInfo;

using namespace Test;

void test_udp()
{
    std::cout << "[TEST 1/4] Udp Socket creation: ";

    try
    {
        UdpSocket udpsock("127.0.0.1", 1234);
        assert_eq<bool>(true, udpsock.isSocketValid());

        SocketInfo* info = udpsock.getSocketInfo();
        assert_eq<bool>(true, info->ready_to_write);
        assert_eq<bool>(false, info->socket_error);
        
        std::cout << "Passed" << std::endl;
    }
    catch(const std::runtime_error& e)
    {
        std::cout << "Failed" << std::endl;
        std::cerr << e.what() << '\n';
        exit(EXIT_FAILURE);
    }
}

void test_tcp()
{
    std::cout << "[TEST 2/4] Tcp Socket creation: ";

    try
    {
        TcpSocket tcpsock(Socket::getInterfaceIp("eth0"), 1234);
        assert_eq<bool>(true, tcpsock.isSocketValid());

        SocketInfo* info = tcpsock.getSocketInfo();
        assert_eq<bool>(false, info->socket_error);

        std::string dstIp = Socket::getHostnameIp("github.com");
        tcpsock.connectTo(dstIp, 443);
        tcpsock.updateSocketInfo();
        assert_eq<bool>(false, info->socket_error);

        if (!tcpsock.isConnected())
        {
            tcpsock.closeSocket();
            throw std::runtime_error("[TcpSocket connect] Connection failed");
        }

        std::cout << "Passed" << std::endl;
        tcpsock.closeSocket();
        tcpsock.updateSocketInfo();
        assert_eq<bool>(true, info->socket_error);
        assert_eq<int>(EBADF, info->error);
        assert_eq<bool>(false, tcpsock.isSocketValid());
    }
    catch(const std::runtime_error& e)
    {
        std::cout << "Failed" << std::endl;
        std::cerr << e.what() << '\n';
        exit(EXIT_FAILURE);
    }
}

void test_type_failure()
{
    std::cout << "[TEST 3/4] Socket Type Test: ";

    try
    {
        Socket s("127.0.0.1", 1234, SocketType::RAW);
        std::cout << "Failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    catch(const std::invalid_argument& e)
    {
        std::cout << "Passed -> ";
        std::cerr << e.what() << std::endl;
    }
    
}

void test_copy_constructor()
{
    std::cout << "[TEST 4/4] Socket Copy constructor Test: ";
    Socket s("127.0.0.1", 1234, SocketType::UDP);
    Socket s_c(s);

    assert_eq_str(s_c.getIpAddress().c_str(), "127.0.0.1", 9);
    assert_eq<unsigned short>(s_c.getPortNumber(), 1234);
    assert_eq<int>(s_c.getSocketFileDescriptor(), s.getSocketFileDescriptor());

    struct sockaddr_in src = s_c.getSource();
    struct sockaddr_in dst_addr;
    inet_aton("127.0.0.1", &dst_addr.sin_addr);

    assert_eq<unsigned short>(src.sin_port, htons(1234));
    assert_eq<int>(src.sin_family, AF_INET);
    assert_eq<in_addr_t>(src.sin_addr.s_addr, dst_addr.sin_addr.s_addr);

    std::cout << "Passed" << std::endl;
}

int main()
{
    test_udp();
    test_tcp();
    test_type_failure();
    test_copy_constructor();

    return 0;
}