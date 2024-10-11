#include "SimpleMessage.hpp"

const std::string &CommonLib::Communication::SimpleMessage::getMessage() const
{
    return this->_msg;
}

void CommonLib::Communication::SimpleMessage::encode()
{
    Message::encode_(*this);
    put((unsigned char*)this->_msg.c_str(), this->_msg.size());
}

void CommonLib::Communication::SimpleMessage::decode()
{
    Message::decode_(*this);

    // Decode the message
    std::size_t buffSize = getRemainingSize();
    
    // Initialize the output vector of bytes
    char msg[buffSize + 1];
    memset(msg, 0, buffSize + 1);

    getBuffer((unsigned char*)msg, buffSize);

    _msg = msg;
}
