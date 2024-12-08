#include "Message.hpp"

void CommonLib::Communication::Message::encode_(Message& msg)
{
    msg.clear();
    msg.put(msg.getMessageCounter());
    msg.put(msg.getMessageId());
    msg.put(static_cast<unsigned char>(msg.getMessageType()));
    msg.put(static_cast<unsigned char>(msg.getMessageSubType()));
    msg.put(msg.getMessageProtoFlags());
    msg.spare();
}

void CommonLib::Communication::Message::decode_(Message &msg)
{
    msg.position(0);
    msg.setMessageCounter(msg.getShort());
    msg.setMessageId(msg.getShort());
    msg.setMessageType(static_cast<MessageType>(msg.get()));
    msg.setMessageSubType(static_cast<MessageSubType>(msg.get()));
    msg.setMessageProtocol(static_cast<MessageProto>(msg.get() >> 6));
    msg.position(msg.position() + 1);
}

const unsigned short CommonLib::Communication::Message::getMessageCounter() const
{
    return _counter;
}

const unsigned short CommonLib::Communication::Message::getMessageId() const
{
    return _id;
}

const CommonLib::Communication::MessageType CommonLib::Communication::Message::getMessageType() const
{
    return _type;
}

const CommonLib::Communication::MessageSubType CommonLib::Communication::Message::getMessageSubType() const
{
    return _subType;
}

const CommonLib::Communication::MessageProto CommonLib::Communication::Message::getMessageProtocol() const
{
    return _proto;
}

const unsigned int CommonLib::Communication::Message::getMessageTypeId() const
{
    return (static_cast<unsigned int>(_type) << 8) + static_cast<unsigned int>(_subType);
}

const uint8_t CommonLib::Communication::Message::getMessageProtoFlags() const
{
    return _flag;
}

void CommonLib::Communication::Message::setMessageType(const MessageType &type)
{
    _type = type;
}

void CommonLib::Communication::Message::setMessageSubType(const MessageSubType &subType)
{
    _subType = subType;
}

void CommonLib::Communication::Message::setMessageId(const unsigned short id)
{
    _id = id;
}

void CommonLib::Communication::Message::setMessageCounter(const unsigned short counter)
{
    _counter = counter;
}

void CommonLib::Communication::Message::setMessageProtocol(const MessageProto &proto)
{
    _proto = proto; // Set the message protocol
    _flag = (static_cast<uint8_t>(_proto) + 1) << 6; // Set the protocol flags
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

void CommonLib::Communication::DiscoverHelloMessage::setUdpPort(const unsigned short udpPort)
{
    _udpPort = udpPort;
}

void CommonLib::Communication::DiscoverHelloMessage::setTcpPort(const unsigned short tcpPort)
{
    _tcpPort = tcpPort;
}

unsigned short CommonLib::Communication::DiscoverHelloMessage::getUdpPort() const
{
    return _udpPort;
}

unsigned short CommonLib::Communication::DiscoverHelloMessage::getTcpPort() const
{
    return _tcpPort;
}

void CommonLib::Communication::DiscoverHelloMessage::encode()
{
    Message::encode_(*this);
    put(_udpPort);
    put(_tcpPort);
}

void CommonLib::Communication::DiscoverHelloMessage::decode()
{
    Message::decode_(*this);
    setUdpPort(getShort());
    setTcpPort(getShort());
}

void CommonLib::Communication::DiscoverResponseMessage::setUdpPort(const unsigned short udpPort)
{
    _udpPort = udpPort;
}

void CommonLib::Communication::DiscoverResponseMessage::setTcpPort(const unsigned short tcpPort)
{
    _tcpPort = tcpPort;
}

void CommonLib::Communication::DiscoverResponseMessage::setAvailableMemory(const uint8_t memory)
{
    _memory = memory;
}

void CommonLib::Communication::DiscoverResponseMessage::setNumberOfCpus(const uint8_t ncpus)
{
    _ncpus = ncpus;
}

void CommonLib::Communication::DiscoverResponseMessage::setTaskQueueDimension(unsigned short qdim)
{
    _qdim = qdim;
}

unsigned short CommonLib::Communication::DiscoverResponseMessage::getUdpPort() const
{
    return _udpPort;
}

unsigned short CommonLib::Communication::DiscoverResponseMessage::getTcpPort() const
{
    return _tcpPort;
}

uint8_t CommonLib::Communication::DiscoverResponseMessage::getAvailableMemory() const
{
    return _memory;
}

uint8_t CommonLib::Communication::DiscoverResponseMessage::getNumberOfCpus() const
{
    return _ncpus;
}

unsigned short CommonLib::Communication::DiscoverResponseMessage::getTaskQueueDimension() const
{
    return _qdim;
}

void CommonLib::Communication::DiscoverResponseMessage::encode()
{
    Message::encode_(*this);
    put(_udpPort);
    put(_tcpPort);
    put(_memory);
    put(_ncpus);
    put(_qdim);
}

void CommonLib::Communication::DiscoverResponseMessage::decode()
{
    Message::decode_(*this);
    setUdpPort(getShort());
    setTcpPort(getShort());
    setAvailableMemory(get());
    setNumberOfCpus(get());
    setTaskQueueDimension(getShort());
}