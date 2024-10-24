#include "Message.hpp"

void CommonLib::Communication::Message::encode_(Message& msg)
{
    msg.clear();
    msg.put(static_cast<unsigned char>(msg.getMessageType()));
    msg.put(msg.getMessageId());
    msg.put(msg.getMessageCounter());
}

void CommonLib::Communication::Message::decode_(Message &msg)
{
    msg.position(0);
    msg.setMessageType(static_cast<MessageType>(msg.get()));
    msg.setMessageId(msg.get());
    msg.setMessageCounter(msg.getShort());
}

const unsigned short CommonLib::Communication::Message::getMessageCounter() const
{
    return _counter;
}

const unsigned char CommonLib::Communication::Message::getMessageId() const
{
    return _id;
}

const CommonLib::Communication::MessageType CommonLib::Communication::Message::getMessageType() const
{
    return MessageType();
}

void CommonLib::Communication::Message::setMessageType(const MessageType &type)
{
    _type = type;
}

void CommonLib::Communication::Message::setMessageId(const unsigned char id)
{
    _id = id;
}

void CommonLib::Communication::Message::setMessageCounter(const unsigned short counter)
{
    _counter = counter;
}

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
