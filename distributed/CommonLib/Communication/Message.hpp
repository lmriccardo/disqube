#ifndef _MESSAGE_HPP
#define _MESSAGE_HPP

#pragma once

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <CommonLib/Communication/ByteBuffer.hpp>

namespace CommonLib::Communication
{
    const unsigned short MAX_MESSAGE_CAPACITY = 65535;
    
    enum MessageType
    {
        SIMPLE = 1
    };

    class Message : public ByteBuffer
    {
        protected:
            MessageType    _type;
            unsigned short _counter;
            unsigned char  _id;

            const static std::size_t NUM_HEAD_BYTES = 4;

            static void encode_(Message& msg);
            static void decode_(Message& msg);

        public:
            Message(const MessageType& type, uint8_t id, uint16_t counter, 
                    const std::size_t nofBytes) \
                : ByteBuffer(nofBytes), _type(type), _counter(counter), _id(id) {}

            Message(const MessageType& type, uint8_t id, uint16_t counter) \
                : Message(type, id, counter, MAX_MESSAGE_CAPACITY) {}

            Message(unsigned char* const& buffer, const std::size_t nofBytes) \
                : ByteBuffer(nofBytes)
            {
                put(buffer, nofBytes);
                position(0);
            }

            Message(const ByteBuffer& buffer) : ByteBuffer(buffer) {};

            ~Message() = default;

            const unsigned short getMessageCounter() const;
            const unsigned char getMessageId() const;
            const MessageType getMessageType() const;

            void setMessageType(const MessageType& type);
            void setMessageId(const unsigned char id);
            void setMessageCounter(const unsigned short counter);

            virtual void encode() = 0;
            virtual void decode() = 0;
    };

    typedef std::shared_ptr<Message> Message_ptr;

    struct ReceivedMessage
    {
        Message_ptr         m;    // The arrived message
        struct sockaddr_in* _src; // The source address and port
    };
}

#endif