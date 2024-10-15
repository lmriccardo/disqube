#ifndef _SENDER_HPP
#define _SENDER_HPP

#pragma once

#include <iostream>
#include <type_traits>
#include <CommonLib/Communication/Socket.hpp>
#include <CommonLib/Communication/Message.hpp>

namespace CommonLib::Communication
{
    class Sender
    {
        public:
            virtual void sendTo(const std::string& ip, const unsigned short port, unsigned char* buff, const std::size_t n) = 0;
            void sendTo(const std::string& ip, const unsigned short port, Message& msg);
    };

    class TcpSender : public Sender
    {
        private:
            TcpSocket _socket; // The socket of the sender

        public:
            TcpSender(const std::string& ip, unsigned short port) : _socket(ip, port) {};
            TcpSender(const TcpSocket& socket) : _socket(socket) {};
            ~TcpSender()
            {
                // close the socket when the sender goes out of scope
                _socket.closeSocket();
            }

            void sendTo(
                const std::string& ip, const unsigned short port, 
                unsigned char* buff, const std::size_t n);

            using Sender::sendTo;
    };

    class UdpSender : public Sender
    {
        private:
            UdpSocket _socket; // The socket of the Udp Sender
        
        public:
            UdpSender(const std::string& ip, unsigned short port) : _socket(ip, port) {};
            UdpSender(const UdpSocket& socket) : _socket(socket) {};
            ~UdpSender()
            {
                // Close the socket when the sender goes out of scope
                _socket.closeSocket();
            }

            void sendTo(
                const std::string& ip, const unsigned short port, 
                unsigned char* buff, const std::size_t n);

            using Sender::sendTo;
    };
}

#endif