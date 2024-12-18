#include "Message.hpp"

using namespace Lib::Network;

void Message::encode_(Message& msg)
{
    msg.clear();
    msg.put(msg.getMessageCounter());
    msg.put(msg.getMessageId());
    msg.put(static_cast<unsigned char>(msg.getMessageType()));
    msg.put(static_cast<unsigned char>(msg.getMessageSubType()));
    msg.put(msg.getMessageProtoFlags());
    msg.spare();
}

void Message::decode_(Message &msg)
{
    msg.position(0);
    msg.setMessageCounter(msg.getShort());
    msg.setMessageId(msg.getShort());
    msg.setMessageType(static_cast<MessageType>(msg.get()));
    msg.setMessageSubType(static_cast<MessageSubType>(msg.get()));
    msg.setMessageProtocol(static_cast<MessageProto>(msg.get() >> 6));
    msg.position(msg.position() + 1);
}

const unsigned short Message::getMessageCounter() const
{
    return _counter;
}

const unsigned short Message::getMessageId() const
{
    return _id;
}

const Message::MessageType Message::getMessageType() const
{
    return _type;
}

const Message::MessageSubType Message::getMessageSubType() const
{
    return _subType;
}

const Message::MessageProto Message::getMessageProtocol() const
{
    return _proto;
}

const unsigned int Message::getMessageTypeId() const
{
    return (static_cast<unsigned int>(_type) << 8) + static_cast<unsigned int>(_subType);
}

const uint8_t Message::getMessageProtoFlags() const
{
    return _flag;
}

void Message::setMessageType(const MessageType &type)
{
    _type = type;
}

void Message::setMessageSubType(const MessageSubType &subType)
{
    _subType = subType;
}

void Message::setMessageId(const unsigned short id)
{
    _id = id;
}

void Message::setMessageCounter(const unsigned short counter)
{
    _counter = counter;
}

void Message::setMessageProtocol(const MessageProto &proto)
{
    _proto = proto; // Set the message protocol
    _flag = (static_cast<uint8_t>(_proto) + 1) << 6; // Set the protocol flags
}

const Message::MessageSubType Message::fetchMessageSubType(const ByteBuffer_ptr& buffer)
{
    uint8_t msgSubtype[1];
    buffer->getBuffer(msgSubtype, Message::MSG_SUBTYPE_OFFSET, Message::BYTE_SIZE);
    return static_cast<Message::MessageSubType>(*(msgSubtype));
}

const std::string &SimpleMessage::getMessage() const
{
    return this->_msg;
}

void SimpleMessage::encode()
{
    Message::encode_(*this);
    put((unsigned char*)this->_msg.c_str(), this->_msg.size());
}

void SimpleMessage::decode()
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

void DiscoverHelloMessage::setUdpPort(const unsigned short udpPort)
{
    _udpPort = udpPort;
}

void DiscoverHelloMessage::setTcpPort(const unsigned short tcpPort)
{
    _tcpPort = tcpPort;
}

void DiscoverHelloMessage::setIpAddress(const unsigned int ipAddr)
{
    _ipaddr = ipAddr;
}

unsigned short DiscoverHelloMessage::getUdpPort() const
{
    return _udpPort;
}

unsigned short DiscoverHelloMessage::getTcpPort() const
{
    return _tcpPort;
}

unsigned int DiscoverHelloMessage::getIpAddress() const
{
    return _ipaddr;
}

void DiscoverHelloMessage::encode()
{
    Message::encode_(*this);
    put(_udpPort);
    put(_tcpPort);
    put(_ipaddr);
}

void DiscoverHelloMessage::decode()
{
    Message::decode_(*this);
    setUdpPort(getShort());
    setTcpPort(getShort());
    setIpAddress(getInt());
}

void DiscoverResponseMessage::setUdpPort(const unsigned short udpPort)
{
    _udpPort = udpPort;
}

void DiscoverResponseMessage::setTcpPort(const unsigned short tcpPort)
{
    _tcpPort = tcpPort;
}

void Lib::Network::DiscoverResponseMessage::setIpAddress(const unsigned int ipAddr)
{
    _ipaddr = ipAddr;
}

void DiscoverResponseMessage::setAvailableMemory_mb(const uint32_t memory_mb)
{
   _freeRamMb = memory_mb; 
}

void DiscoverResponseMessage::setAvailableMemory_kb(const uint32_t memory_kb)
{
    _freeRamKb = memory_kb;
}

void DiscoverResponseMessage::setCpuUsage(const uint8_t cpu_usage)
{
    _cpuUsage = cpu_usage;
}

unsigned short DiscoverResponseMessage::getUdpPort() const
{
    return _udpPort;
}

unsigned int Lib::Network::DiscoverResponseMessage::getIpAddress() const
{
    return _ipaddr;
}

unsigned short DiscoverResponseMessage::getTcpPort() const
{
    return _tcpPort;
}

uint32_t DiscoverResponseMessage::getAvailableMemory_kb() const
{
    return _freeRamKb;
}

uint32_t DiscoverResponseMessage::getAvailableMemory_mb() const
{
    return _freeRamMb;
}

unsigned long long DiscoverResponseMessage::getAvailableMemory() const
{
    return _freeRamMb * 1000 + _freeRamKb;
}

uint8_t DiscoverResponseMessage::getCpuUsage() const
{
    return _cpuUsage;
}

void DiscoverResponseMessage::encode()
{
    Message::encode_(*this);
    put(_udpPort);
    put(_tcpPort);
    put(_ipaddr);
    put(_freeRamMb);
    put(_freeRamKb);
    put(_cpuUsage);
    spare();
    spare();
    spare();
}

void DiscoverResponseMessage::decode()
{
    Message::decode_(*this);
    setUdpPort(getShort());
    setTcpPort(getShort());
    setIpAddress(getInt());
    setAvailableMemory_mb(getInt());
    setAvailableMemory_kb(getInt());
    setCpuUsage(get());
    position(position() + 3);
}