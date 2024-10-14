#include <iostream>
#include <CommonLib/Communication/Sender.hpp>
#include <CommonLib/Communication/Listener.hpp>
#include <CommonLib/Communication/SimpleMessage.hpp>
#include "Test.hpp"

#define LOCALHOST "127.0.0.1"
#define LISTENER_PORT 1234
#define SENDER_PORT 1235

using namespace Test;
using Socket = CommonLib::Communication::Socket;
using UdpSender = CommonLib::Communication::UdpSender;
using TcpSender = CommonLib::Communication::TcpSender;
using SimpleMessage = CommonLib::Communication::SimpleMessage;

template <typename T>
using UdpListener = CommonLib::Communication::UdpListener<T>;

std::string msg = "Ciao";
std::string stop = "Stop";
SimpleMessage sm(1, 0, msg);
SimpleMessage sm_stop(1, 0, stop);

struct Element
{
    std::shared_ptr<SimpleMessage> ss;
    struct sockaddr_in src;
};

class MyUdpListener : public UdpListener<struct Element>
{
    public:
        using UdpListener<struct Element>::UdpListener;

        struct Element handleArrivingMessages(
            unsigned char* buff, const std::size_t nofBytes, struct sockaddr_in* src
        ) {
            struct Element e;
            SimpleMessage _sm(buff, nofBytes);
            e.ss = std::make_shared<SimpleMessage>(_sm);
            e.src = *src;
            return e;
        }

        bool toBeStopped(const struct Element& e)
        {
            return e.ss->getMessage() == "Stop";
        }
};

void test_udp()
{
    try
    {
        std::cout << "[TEST 1/?] Constructing UdpSender and binding: ";
        UdpSender udp_s(LOCALHOST, SENDER_PORT);
        std::cout << "Passed" << std::endl;

        std::cout << "[TEST 2/?] Sending a SimpleMessage to Localhost using UdpSender: ";
        udp_s.sendTo(LOCALHOST, LISTENER_PORT, sm);
        assert_eq<int>(errno, 0);
        std::cout << "Passed" << std::endl;

        std::cout << "[TEST 3/?] Sending a SimpleMessage to external using UdpSender on localhost: ";
        udp_s.sendTo(Socket::getHostnameIp("google.com"), 80, sm);
        assert_eq<int>(errno, EINVAL);
        std::cout << "Passed" << std::endl;

        std::cout << "[TEST 4/?] Sending a SimpleMessage to localhost and check reception: ";
        MyUdpListener udp_l(LOCALHOST, LISTENER_PORT, 1);
        udp_l.start();

        udp_s.sendTo(LOCALHOST, LISTENER_PORT, sm);
        assert_eq<int>(errno, 0);
        udp_s.sendTo(LOCALHOST, LISTENER_PORT, sm_stop);
        assert_eq<int>(errno, 0);

        udp_l.join();
        struct Element e = udp_l.getQueue()->pop();
        std::shared_ptr<SimpleMessage> sm_ptr = e.ss;
        assert_eq<std::string>(sm_ptr->getMessage(), "Ciao");

        std::cout << "Passed" << std::endl;
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
    test_udp();
    return 0;
}