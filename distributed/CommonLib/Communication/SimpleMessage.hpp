#ifndef _SIMPLE_MESSAGE_HPP
#define _SIMPLE_MESSAGE_HPP

#pragma once

#include <iostream>
#include <CommonLib/Communication/Message.hpp>

namespace CommonLib::Communication
{
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

            SimpleMessage(unsigned char* const& buff, const std::size_t nofBytes) \
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