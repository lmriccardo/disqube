#include "Test.hpp"
#include <CommonLib/Communication/Interface.hpp>

using UdpCommunicationInterface = CommonLib::Communication::UdpCommunicationInterface;
using TcpCommunicationInterface = CommonLib::Communication::TcpCommunicationInterface;
using SimpleMessage = CommonLib::Communication::SimpleMessage;
using ReceivedData = CommonLib::Communication::ReceivedData;

using namespace Test;

std::string msg = "Ciao";
SimpleMessage sm(1, 1, msg);

void test_udp_interface()
{
    std::cout << "[TEST 1/4] Udp Communication interface creation: ";
    UdpCommunicationInterface udp_int_1("127.0.0.1", 1234, 1235, 2);
    UdpCommunicationInterface udp_int_2("127.0.0.1", 1236, 1237, 2);
    std::cout << "Passed" << std::endl;

    // Starts the listening thread for both interfaces
    udp_int_1.start();
    udp_int_2.start();

    std::cout << "[TEST 2/4] Sending a message (1) -> (2): ";
    udp_int_1.sendTo("127.0.0.1", 1237, sm); // Sends the message from 1 to 2
    assert_eq<int>(errno, 0); // Assert no error on sending the message
    ReceivedData data1 = udp_int_2.getReceivedElement(); // Pop the element from the receiver queue
    SimpleMessage recv1((*data1.data)); // Cast it to a Simple Message
    assert_eq<std::string>(recv1.getMessage(), "Ciao"); // Assert that the content is the same
    std::cout << "Passed" << std::endl;

    std::cout << "[TEST 3/4] Sending a message (2) -> (1): ";
    udp_int_2.sendTo("127.0.0.1", 1235, sm); // Sends the message from 2 to 1
    assert_eq<int>(errno, 0);
    ReceivedData data2 = udp_int_1.getReceivedElement();
    SimpleMessage recv2((*data2.data));
    assert_eq<std::string>(recv2.getMessage(), "Ciao");
    std::cout << "Passed" << std::endl;

    // Close the two interfaces
    std::cout << "[TEST 4/4] Closing: ";
    udp_int_1.close();
    udp_int_2.close();
    assert_eq<bool>(udp_int_1.isClosed(), true);
    assert_eq<bool>(udp_int_2.isClosed(), true);
    std::cout << "Passed" << std::endl;
}

void test_tcp_interface()
{
    std::cout << "[TEST 1/4] Tcp Communication interface creation: ";
    TcpCommunicationInterface tcp_int_1("127.0.0.1", 1234, 1235, 2, 2, 1, 0);
    TcpCommunicationInterface tcp_int_2("127.0.0.1", 1238, 1237, 2, 2, 1, 0);
    std::cout << "Passed" << std::endl;

    tcp_int_1.start();
    tcp_int_2.start();

    std::cout << "[TEST 2/4] Sending a message (1) -> (2): ";
    tcp_int_1.sendTo("127.0.0.1", 1237, sm); // Sends the message from 1 to 2
    assert_eq<int>(errno, 0); // Assert no error on sending the message
    ReceivedData data1 = tcp_int_2.getReceivedElement(); // Pop the element from the receiver queue
    SimpleMessage recv1((*data1.data)); // Cast it to a Simple Message
    assert_eq<std::string>(recv1.getMessage(), "Ciao"); // Assert that the content is the same
    std::cout << "Passed" << std::endl;

    std::cout << "[TEST 3/4] Sending a message (2) -> (1): ";
    tcp_int_2.sendTo("127.0.0.1", 1235, sm); // Sends the message from 2 to 1
    assert_eq<int>(errno, 0);
    ReceivedData data2 = tcp_int_1.getReceivedElement();
    SimpleMessage recv2((*data2.data));
    assert_eq<std::string>(recv2.getMessage(), "Ciao");
    std::cout << "Passed" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "[TEST 4/4] Closing: ";

    // First we need to close the connections between the two interfaces
    // Then we can stop and join the listener, otherwise there would be
    // a blocking factors that blocks the seconds interface to be closed.
    tcp_int_2.close();
    tcp_int_1.close();
    
    assert_eq<bool>(tcp_int_1.isClosed(), true);
    assert_eq<bool>(tcp_int_2.isClosed(), true);
    std::cout << "Passed" << std::endl;
}

int main()
{
    std::cout << "----------------------------------- UDP -----------------------------------" << std::endl;
    test_udp_interface();

    std::cout << "\n----------------------------------- TCP -----------------------------------" << std::endl;
    test_tcp_interface();
    return 0;
}