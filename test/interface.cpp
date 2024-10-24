#include "Test.hpp"
#include <CommonLib/Communication/Interface.hpp>

using UdpCommunicationInterface = CommonLib::Communication::UdpCommunicationInterface;
using TcpCommunicationInterface = CommonLib::Communication::TcpCommunicationInterface;

void test_udp_interface()
{
    std::cout << "[TEST 1/?] Udp Communication interface creation: " << std::endl;
    UdpCommunicationInterface udp_int("127.0.0.1", 1234, 1235, 2);
    
}

int main()
{

}