#ifndef _LISTENER_HPP
#define _LISTENER_HPP

#pragma once

#include <iostream>
#include <optional>
#include <CommonLib/Communication/Socket.hpp>
#include <CommonLib/Communication/Message.hpp>
#include <CommonLib/Concurrency/Thread.hpp>
#include <CommonLib/Concurrency/Queue.hpp>

#define RECVBUFFSIZE 4096

using Thread = CommonLib::Concurrency::Thread;

namespace CommonLib::Communication
{
    template <typename T>
    class Listener : public Thread
    {
        protected:
            CommonLib::Concurrency::Queue_ptr<T> _queue; // The queue of received messages
            bool _stop;  // Flag indicating whether to stop or not the listening thread
            
            virtual std::optional<T> receiveOne() = 0;

            // Static method to receive just one single message
            static std::optional<T> recvOne(Listener& cls) { return cls.receiveOne(); }

        public:
            Listener(CommonLib::Concurrency::Queue_ptr<T> q) : Thread("Listener"), _queue(q), _stop(false) {};
            Listener(const std::size_t capacity);

            virtual void listen() = 0;
            virtual bool toBeStopped(const T& value) = 0;

            void run();

            CommonLib::Concurrency::Queue_ptr<T> getQueue();
            T getElement();
    };

    template <typename T>
    inline Listener<T>::Listener(const std::size_t capacity) : Thread("Listener")
    {
        _stop = false;
        _queue = std::make_shared<CommonLib::Concurrency::Queue<T>>(capacity);
    }

    template <typename T>
    inline void Listener<T>::run()
    {
        listen();
    }

    template <typename T>
    inline CommonLib::Concurrency::Queue_ptr<T> Listener<T>::getQueue()
    {
        return _queue;
    }

    template <typename T>
    inline T Listener<T>::getElement()
    {
        return _queue->pop();
    }

    template <typename T>
    class UdpListener : public Listener<T>
    {
        protected:
            UdpSocket _socket;

            std::optional<T> receiveOne() override;
            virtual T handleArrivingMessages(
                unsigned char* buff, const std::size_t nofBytes,
                struct sockaddr_in* src) = 0;

        public:
            UdpListener() = delete;
            UdpListener(const std::string& ip, unsigned short port, const std::size_t capacity) \
                : Listener<T>(capacity), _socket(ip, port) {};

            UdpListener(const std::string& ip, unsigned short port, CommonLib::Concurrency::Queue_ptr<T> q) \
                : Listener<T>(q), _socket(ip, port) {};

            UdpListener(const UdpSocket& socket, CommonLib::Concurrency::Queue_ptr<T> q) \
                : Listener<T>(q), _socket(socket) {};

            UdpListener(const UdpSocket& socket, const std::size_t capacity) \
                : Listener<T>(capacity), _socket(socket) {};

            ~UdpListener() = default;

    	    void listen();
    };

    template <typename T>
    inline std::optional<T> UdpListener<T>::receiveOne()
    {
        char buffer[RECVBUFFSIZE];
        memset(buffer, 0, sizeof(buffer));

        int fd = _socket.getSocketFileDescriptor();
        struct sockaddr_in src;
        socklen_t srclen = sizeof(src);
        ssize_t nofBytes;

        // If the reception has failed, we returns nothing
        if ((nofBytes = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&src, &srclen)) < 0)
        {
            std::cout << "[UdpListener::receiveOne] Error when receiving data: ";
            std::cout << std::strerror(errno) << std::endl;
            return std::nullopt;
        }

        // Otherwise we need the user-defined handleArrivingMessages functions
        // should convert, in some way, the inputs into the type T
        return handleArrivingMessages((unsigned char*)buffer, nofBytes, &src);
    }

    template <typename T>
    inline void UdpListener<T>::listen()
    {
        while (!this->_stop)
        {
            // Receive one single message and get the result
            std::optional<T> result = receiveOne();

            // We need to check that the result is actually there
            if (!result.has_value()) break;

            // Otherwise, before putting the element into the queue
            // we need to check whether it is necessarily to stop, 
            // maybe it is a stop message.
            if (this->toBeStopped(*result)) break;

            // Otherwise, put the result into the queue
            this->_queue->push(*result);
        }
    }
}

#endif