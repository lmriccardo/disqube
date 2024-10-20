#ifndef _LISTENER_HPP
#define _LISTENER_HPP

#pragma once

#include <iostream>
#include <optional>
#include <algorithm>
#include <CommonLib/Communication/Socket.hpp>
#include <CommonLib/Communication/Message.hpp>
#include <CommonLib/Communication/Receiver.hpp>
#include <CommonLib/Concurrency/Thread.hpp>
#include <CommonLib/Concurrency/Queue.hpp>

using Thread = CommonLib::Concurrency::Thread;
template <typename T> using Queue_ptr = CommonLib::Concurrency::Queue_ptr<T>;

namespace CommonLib::Communication
{
    class Listener : public Thread
    {
        protected:
            Queue_ptr<struct ReceivedData> _queue;   // The queue of received and converted messages
            bool                           _sigstop; // Flag indicating when the listener must be stopped

        public:
            Listener(Queue_ptr<struct ReceivedData> queue, const std::string& name) 
                : Thread(name), _queue(queue), _sigstop(false) {};

            Listener(const std::size_t capacity, const std::string& name) : Thread(name) 
            {
                _queue = std::make_shared<CommonLib::Concurrency::Queue<struct ReceivedData>>(capacity);
                _sigstop = false;
            };

            /**
             * Stop the listening thread
             */
            void stop();

            /**
             * Returns an element from the queue
             */
            struct ReceivedData getElement();

            /**
             * Returns the pointer to the queue
             */
            Queue_ptr<struct ReceivedData> getQueue();
    };

    class UdpListener : public Listener
    {
        private:
            UdpSocket   _socket; // The Udp Socket
            UdpReceiver _recv;   // The object receiving the messages

        public:
            /**
             * @param ip The Ip address needed to bind the socket
             * @param port The port number, also needed by the socket
             * @param q A shared pointer to a Queue
             */
            UdpListener(const std::string& ip, unsigned short port, Queue_ptr<struct ReceivedData> q)
                : Listener(q, "UdpListener"), _socket(ip, port), _recv(q, _socket) {};

            UdpListener(const std::string& ip, unsigned short port, const std::size_t c)
                : Listener(c, "UdpListener"), _socket(ip, port), _recv(this->_queue, _socket) {};

            UdpListener(const UdpSocket& s, Queue_ptr<struct ReceivedData> q)
                : Listener(q, "UdpListener"), _socket(s), _recv(q, _socket) {};

            UdpListener(const UdpSocket& s, const std::size_t c)
                : Listener(c, "UdpListener"), _socket(s), _recv(this->_queue, _socket) {};

            /**
             * This function will listen from incoming messages from any
             * sender. It will use the receiver to setup Udp reception and 
             * fill the queue with received messages. 
             */
            void run() override;
    };

    class TcpListener : public Listener
    {
        private:
            TcpSocket                                 _socket;    // The Tcp Socket
            std::vector<std::shared_ptr<TcpReceiver>> _recvs;     // The vector of all receivers
            std::size_t                               _clientIdx; // The current number of clients connected to the server

            int handleReceivers();

            /**
             * Wait for a connection of a client until the timeout expires.
             */
            int acceptIncoming(struct sockaddr_in& client, socklen_t clientlen);

        public:
            TcpListener(const std::string& ip, unsigned short port, Queue_ptr<struct ReceivedData> queue, const std::size_t nconn)
                : Listener(queue, "TcpListener"), _socket(ip, port), _recvs(nconn, nullptr), _clientIdx(0) {};

            TcpListener(const std::string& ip, unsigned short port, const std::size_t capacity, const std::size_t nconn)
                : Listener(capacity, "TcpListener"), _socket(ip, port), _recvs(nconn, nullptr), _clientIdx(0) {};

            TcpListener(const TcpSocket& s, Queue_ptr<struct ReceivedData> queue, const std::size_t nconn)
                : Listener(queue, "TcpListener"), _socket(s), _recvs(nconn, nullptr), _clientIdx(0) {};

            TcpListener(const TcpSocket& s, const std::size_t capacity, const std::size_t nconn)
                : Listener(capacity, "TcpListener"), _socket(s), _recvs(nconn, nullptr), _clientIdx(0) {};

            /**
             * @throws runtime_error If listen syscall fail.
             */
            void run() override;

            void setTimeout(long int sec, long int usec);
            void setTimeout(long int sec);
    };
}

#endif