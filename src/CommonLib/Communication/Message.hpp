#ifndef _MESSAGE_HPP
#define _MESSAGE_HPP

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <CommonLib/Communication/ByteBuffer.hpp>

namespace Lib::Network
{
    const unsigned short MAX_MESSAGE_CAPACITY = 65535;

    class Message : public ByteBuffer
    {
    public:
        enum class MessageProto
        {
            TCP, // Tcp Message Protocol
            UDP  // Udp Message Protocol
        };

        enum class MessageType
        {
            SIMPLE,  // Used only for simple string message
            DISCOVER // Used for the discover protocol
        };

        enum class MessageSubType
        {
            SIMPLE = 0,
            DISCOVER_HELLO = 1,   // A message usually sent from the master to workers
            DISCOVER_RESPONSE = 2 // A response message for the HELLO
        };

        const static unsigned int MSG_COUNTER_OFFSET = 0;
        const static unsigned int MSG_ID_OFFSET = 2;
        const static unsigned int MSG_TYPE_OFFSET = 4;
        const static unsigned int MSG_SUBTYPE_OFFSET = 5;
        const static unsigned int MSG_PROTO_FLAG_OFFSET = 6;

    protected:
        MessageType _type;
        MessageSubType _subType;
        MessageProto _proto;
        unsigned short _counter;
        unsigned short _id;
        uint8_t _flag;

        const static std::size_t NUM_HEAD_BYTES = 8;

        static void encode_(Message &msg);
        static void decode_(Message &msg);

    public:
        Message(const MessageType &type, const MessageSubType &subType,
                uint16_t id, uint16_t counter, const std::size_t nofBytes)
            : ByteBuffer(nofBytes), _type(type), _subType(subType),
              _counter(counter), _id(id) {}

        Message(const MessageType &type, const MessageSubType &subType, uint16_t id,
                uint16_t counter) : Message(type, subType, id, counter, MAX_MESSAGE_CAPACITY) {}

        Message(const unsigned char *buffer, const std::size_t nofBytes)
            : ByteBuffer(nofBytes)
        {
            put(buffer, nofBytes);
            position(0);
        }

        Message(const ByteBuffer &buffer) : ByteBuffer(buffer) {};

        ~Message() = default;

        const unsigned short getMessageCounter() const;
        const unsigned short getMessageId() const;
        const MessageType getMessageType() const;
        const MessageSubType getMessageSubType() const;
        const MessageProto getMessageProtocol() const;
        const unsigned int getMessageTypeId() const;
        const uint8_t getMessageProtoFlags() const;

        void setMessageType(const MessageType &type);
        void setMessageSubType(const MessageSubType &subType);
        void setMessageId(const unsigned short id);
        void setMessageCounter(const unsigned short counter);
        void setMessageProtocol(const MessageProto &proto);

        virtual void encode() = 0;
        virtual void decode() = 0;

        static const MessageSubType fetchMessageSubType(const ByteBuffer_ptr& buffer);
    };

    // The data received from a socket will be put into a structure
    // divided into the ByteBuffer and the client information
    struct ReceivedData
    {
        ByteBuffer_ptr data;     // The ByteBuffer with the received bytes
        struct sockaddr_in *src; // Informations of the sender
    };

    class SimpleMessage : public Message
    {
    private:
        std::string _msg;

    public:
        SimpleMessage(const uint16_t id, const uint16_t counter, std::string &msg)
            : Message(
                  Message::MessageType::SIMPLE, Message::MessageSubType::SIMPLE, id, counter,
                  NUM_HEAD_BYTES + msg.size()),
              _msg(msg) {};

        SimpleMessage(const unsigned char *buff, const std::size_t nofBytes)
            : Message(buff, nofBytes)
        {
            decode();
        }

        SimpleMessage(const ByteBuffer &buffer) : Message(buffer)
        {
            decode();
        }

        ~SimpleMessage() = default;

        const std::string &getMessage() const;
        void encode();
        void decode();
    };

    class DiscoverHelloMessage : public Message
    {
    private:
        unsigned short _udpPort;
        unsigned short _tcpPort;
        unsigned int _ipaddr;

        static const std::size_t MSG_NUM_BYTES = 8;

    public:
        DiscoverHelloMessage(const uint16_t id, const uint16_t counter)
            : Message(MessageType::DISCOVER, MessageSubType::DISCOVER_HELLO,
                      id, counter, NUM_HEAD_BYTES + MSG_NUM_BYTES) {};

        DiscoverHelloMessage(const ByteBuffer &buffer) : Message(buffer)
        {
            decode();
        }

        void setUdpPort(const unsigned short udpPort);
        void setTcpPort(const unsigned short tcpPort);
        void setIpAddress(const unsigned int ipAddr);

        unsigned short getUdpPort() const;
        unsigned short getTcpPort() const;
        unsigned int getIpAddress() const;

        void encode();
        void decode();
    };

    class DiscoverResponseMessage : public Message
    {
    private:
        unsigned short _udpPort;
        unsigned short _tcpPort;
        unsigned int _ipaddr;
        unsigned int _freeRamMb;
        unsigned int _freeRamKb;
        uint8_t _cpuUsage;

        static const std::size_t MSG_NUM_BYTES = 20;

    public:
        DiscoverResponseMessage(const uint16_t id, const uint16_t counter)
            : Message(MessageType::DISCOVER, MessageSubType::DISCOVER_RESPONSE,
                      id, counter, NUM_HEAD_BYTES + MSG_NUM_BYTES) {};

        DiscoverResponseMessage(const ByteBuffer &buffer) : Message(buffer)
        {
            decode();
        }

        void setUdpPort(const unsigned short udpPort);
        void setTcpPort(const unsigned short tcpPort);
        void setIpAddress(const unsigned int ipAddr);
        void setAvailableMemory_mb(const uint32_t memory_mb);
        void setAvailableMemory_kb(const uint32_t memory_kb);
        void setCpuUsage(const uint8_t cpu_usage);

        unsigned short getUdpPort() const;
        unsigned short getTcpPort() const;
        unsigned int getIpAddress() const;
        uint32_t getAvailableMemory_kb() const;
        uint32_t getAvailableMemory_mb() const;
        unsigned long long getAvailableMemory() const;
        uint8_t getCpuUsage() const;

        void encode();
        void decode();
    };
}

#endif