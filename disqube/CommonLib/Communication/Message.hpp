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

            Message(const unsigned char* buffer, const std::size_t nofBytes) \
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

    // The data received from a socket will be put into a structure
    // divided into the ByteBuffer and the client information
    struct ReceivedData
    {
        ByteBuffer_ptr      data; // The ByteBuffer with the received bytes
        struct sockaddr_in* src;  // Informations of the sender
    };

    class SimpleMessage : public Message
    {
        private:
            std::string _msg;
        
        public:
            SimpleMessage(const uint8_t id, const uint16_t counter, std::string& msg) \
                : Message(
                    MessageType::SIMPLE, id, counter, 
                    NUM_HEAD_BYTES + msg.size()
                ), _msg(msg) {};

            SimpleMessage(const unsigned char* buff, const std::size_t nofBytes) \
                : Message(buff, nofBytes) 
            {
                decode();
            }

            SimpleMessage(const ByteBuffer& buffer) : Message(buffer) 
            {
                decode();
            }

            ~SimpleMessage() = default;

            const std::string& getMessage() const;
            void encode();
            void decode();
    };
}

#endif