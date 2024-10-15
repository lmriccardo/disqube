#include <iostream>
#include <CommonLib/Communication/Message.hpp>

using SimpleMessage = CommonLib::Communication::SimpleMessage;

int main()
{
    std::string msg = "Messaggio";
    SimpleMessage sm(1, 0, msg);

    std::cout << "Created Message: " << sm.getMessage() << std::endl;
    
    // Fill the buffer with the content
    sm.encode();

    std::vector<unsigned char> content = sm.getBuffer();
    SimpleMessage sm_d(content.data(), content.size());
    std::cout << "Received Message: " << sm_d.getMessage() << std::endl;

    SimpleMessage sm_c(sm);
    std::cout << "Received Message: " << sm_c.getMessage() << std::endl;

    return 0;
}