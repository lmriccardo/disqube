#include <iostream>
#include <CommonLib/Communication/Socket.hpp>

using namespace CommonLib::Communication;

void test_udp()
{
    std::cout << "[TEST 1/3] Udp Socket creation: ";

    try
    {
        UdpSocket udpsock("127.0.0.1", 1234);
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
    std::cout << "[TEST 2/3] Tcp Socket creation: ";

    try
    {
        TcpSocket tcpsock(Socket::getInterfaceIp("eth0"), 1234);

        std::string dstIp = Socket::getHostnameIp("github.com");
        tcpsock.connectTo(dstIp, 443);

        if (!tcpsock.isConnected())
        {
            tcpsock.closeSocket();
            throw std::runtime_error("[TcpSocket connect] Connection failed");
        }

        std::cout << "Passed" << std::endl;
        tcpsock.closeSocket();
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
    std::cout << "[TEST 3/3] Socket Type Test: ";

    try
    {
        Socket s("127.0.0.1", 1234, SocketType::RAW);
        std::cout << "Failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    catch(const std::invalid_argument& e)
    {
        std::cout << "Passed" << std::endl;
        std::cerr << e.what() << '\n';
        exit(EXIT_SUCCESS);
    }
    
}

int main()
{
    test_udp();
    test_tcp();
    test_type_failure();

    return 0;
}