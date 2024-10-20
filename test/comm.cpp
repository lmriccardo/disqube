#include <iostream>
#include <CommonLib/Communication/Sender.hpp>
#include <CommonLib/Communication/Listener.hpp>
#include <CommonLib/Communication/Message.hpp>
#include <CommonLib/Communication/Receiver.hpp>
#include "Test.hpp"

#define LOCALHOST "127.0.0.1"
#define LISTENER_PORT 1234
#define SENDER_PORT 1235

using namespace Test;
using Socket = CommonLib::Communication::Socket;
using UdpSender = CommonLib::Communication::UdpSender;
using TcpSender = CommonLib::Communication::TcpSender;
using SimpleMessage = CommonLib::Communication::SimpleMessage;

using UdpListener = CommonLib::Communication::UdpListener;
using TcpListener = CommonLib::Communication::TcpListener;
using ReceivedData = CommonLib::Communication::ReceivedData;

std::string msg = "Ciao";
std::string stop = "Stop";
SimpleMessage sm(1, 0, msg);
SimpleMessage sm_stop(1, 0, stop);


void test_udp()
{
    try
    {
        std::cout << "[TEST 1/4] Constructing UdpSender and binding: ";
        UdpSender udp_s(LOCALHOST, SENDER_PORT);
        std::cout << "Passed" << std::endl;

        std::cout << "[TEST 2/4] Sending a SimpleMessage to Localhost using UdpSender: ";
        udp_s.sendTo(LOCALHOST, LISTENER_PORT, sm);
        assert_eq<int>(errno, 0);
        std::cout << "Passed" << std::endl;

        std::cout << "[TEST 3/4] Sending a SimpleMessage to external using UdpSender on localhost: ";
        udp_s.sendTo(Socket::getHostnameIp("google.com"), 80, sm);
        assert_eq<int>(errno, EINVAL);
        std::cout << "Passed" << std::endl;

        std::cout << "[TEST 4/4] Sending a SimpleMessage to localhost and check reception: ";
        UdpListener udp_l(LOCALHOST, LISTENER_PORT, 1);
        udp_l.start();

        udp_s.sendTo(LOCALHOST, LISTENER_PORT, sm);
        assert_eq<int>(errno, 0);
        udp_s.sendTo(LOCALHOST, LISTENER_PORT, (unsigned char*)"", 0);
        assert_eq<int>(errno, 0);

        udp_l.join();
        ReceivedData e = udp_l.getQueue()->pop();
        SimpleMessage sm_((*e.data));
        assert_eq<std::string>(sm_.getMessage(), "Ciao");

        std::cout << "Passed" << std::endl;
    }
    catch(const std::runtime_error& re)
    {
        std::cerr << re.what() << " -> ";
        std::cout << "Failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void test_tcp()
{
    try
    {
        std::cout << "[TEST 1/3] Constructing TcpSender and binding: ";
        TcpSender tcp_s(LOCALHOST, SENDER_PORT);
        std::cout << "Passed" << std::endl;

        std::cout << "[TEST 2/3] Sending a message to Google.com: ";
        TcpSender tcp_s_real(Socket::getInterfaceIp("eth0"), SENDER_PORT + 1);
        tcp_s_real.sendTo(Socket::getHostnameIp("google.com"), 80, sm);
        tcp_s_real.closeSocket();
        assert_eq<int>(errno, 0);
        assert_eq<bool>(tcp_s_real.isSocketClosed(), true);
        std::cout << "Passed" << std::endl;

        std::cout << "[TEST 3/3] Listener connection and reception: ";
        TcpListener tcp_l(LOCALHOST, LISTENER_PORT, 1, 1);
        tcp_l.setTimeout(2);
        tcp_l.start();

        // Send the message and disconnect the socket
        tcp_s.setTimeout(2);
        tcp_s.sendTo(LOCALHOST, LISTENER_PORT, sm);
        tcp_s.closeSocket();
        assert_eq<bool>(tcp_s.isSocketClosed(), true);
        std::cout << "Passed" << std::endl;

        // Check if the message has been received
        ReceivedData e = tcp_l.getQueue()->pop();
        SimpleMessage sm_((*e.data));
        assert_eq<std::string>(sm_.getMessage(), "Ciao");

        std::cout << "Passed" << std::endl;

        tcp_l.stop();
        tcp_l.join();
    }
    catch(const std::runtime_error& re)
    {
        std::cerr << re.what() << " -> ";
        std::cout << "Failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    
}

int main()
{
    std::cout << "--------------------------------- UDP ---------------------------------" << std::endl;
    test_udp();

    std::cout << "\n--------------------------------- TCP ---------------------------------" << std::endl;
    test_tcp();
    return 0;
}