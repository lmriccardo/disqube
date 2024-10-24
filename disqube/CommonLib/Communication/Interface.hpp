#ifndef _INTERFACE_H
#define _INTERFACE_H

#pragma once

#include <CommonLib/Concurrency/Queue.hpp>
#include <CommonLib/Communication/Message.hpp>
#include <CommonLib/Communication/Listener.hpp>
#include <CommonLib/Communication/Sender.hpp>


namespace CommonLib::Communication
{
    class CommunicationInterface
    {
        protected:
            std::shared_ptr<Sender>   _sender;   // A pointer to a sender obeject
            std::shared_ptr<Listener> _listener; // A pointer to a listener object

            // A pointer to the shared queue between sender and receiver
            Concurrency::Queue_ptr<ReceivedData> _queue;

        public:
            CommunicationInterface(const std::size_t capacity) {
                // Construct the queue pointer given the maximum capacity of the queue
                _queue = std::make_shared<Concurrency::Queue<ReceivedData>>(capacity);
            }

            ~CommunicationInterface()
            {
                this->close(); // Close all sockets on exit
            }

            void close(); // Close the interface, meaning sender and listener socket
            bool isClosed(); // Check if the interface has been closed successfully

            // Sends a single message to the destination address and port
            void sendTo(const std::string& ip, unsigned short port, Message& msg);

            // Get a single message from the receiving queue
            struct ReceivedData getReceivedElement();

            // Start the communication interface, which means starting the listener
            void start();
    };

    class UdpCommunicationInterface : public CommunicationInterface
    {
        public:
            UdpCommunicationInterface(const std::string& ip, unsigned short sport, 
                unsigned short lport, const std::size_t capacity);
    };

    class TcpCommunicationInterface : public CommunicationInterface
    {
        public:
            TcpCommunicationInterface(const std::string& ip, unsigned short sport, 
                unsigned short lport, const std::size_t nconn, 
                const std::size_t capacity, long int timesec,
                long int timeusec);
    };
}

#endif