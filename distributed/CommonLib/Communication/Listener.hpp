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
            
            virtual void listenFrom() = 0;
            virtual std::optional<T> receiveOne() = 0;
            virtual T handleArrivingMessages(
                unsigned char* buff, const std::size_t nofBytes,
                struct sockaddr_in* src) = 0;

            // Static method to receive just one single message
            static std::optional<T> recvOne(Listener& cls) { return cls.receiveOne(); }

        public:
            Listener(CommonLib::Concurrency::Queue_ptr<T> q) : Thread("Listener"), _queue(q), _stop(false) {};
            Listener(const std::size_t capacity);

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
        listenFrom();
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
        private:
            void listenFrom();

        protected:
            UdpSocket _socket;

            std::optional<T> receiveOne() override;

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

            std::optional<T> recvOne();
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
        return this->handleArrivingMessages((unsigned char*)buffer, nofBytes, &src);
    }

    template <typename T>
    inline std::optional<T> UdpListener<T>::recvOne()
    {
        return Listener<T>::recvOne(*this);
    }

    template <typename T>
    inline void UdpListener<T>::listenFrom()
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

    // template <typename T>
    // class TcpListener : public Listener<T>
    // {
    //     private:
    //         void listenFrom();

    //         TcpListener(const std::size_t n_accepts) : _nconn(n_accepts)
    //         {
    //             _connections.reserve(_nconn);
    //             _srcsockets.reserve(_nconn);
    //         }
            
    //     protected:
    //         TcpSocket                       _socket;
    //         std::size_t                     _nconn;       // Number of maximum incoming connections to accept
    //         std::vector<struct sockaddr_in> _connections; // Source addresses of connecting clients
    //         std::vector<int>                _srcsockets;  // Sockets FD of connecting clients
    //         std::vector<std::thread>        _threads;

    //         std::optional<T> receiveOne() override;

    //     public:
    //         TcpListener() = delete;
    //         TcpListener(
    //             const std::string& ip, unsigned short port, const std::size_t capacity, 
    //             const std::size_t n_accepts) \
    //                 : TcpListener<T>(n_accepts), Listener<T>(capacity), _socket(ip, port) {};

    //         TcpListener(
    //             const std::string& ip, unsigned short port, 
    //             CommonLib::Concurrency::Queue_ptr<T> q, const std::size_t n_accepts) \
    //                 : TcpListener<T>(n_accepts), Listener<T>(q), _socket(ip, port) {};

    //         TcpListener(const TcpSocket& socket, const std::size_t capacity,
    //             const std::size_t n_accepts) \
    //                 : TcpListener<T>(n_accepts), Listener<T>(capacity), _socket(socket) {};

    //         TcpListener(const TcpSocket& socket, CommonLib::Concurrency::Queue_ptr<T> q,
    //             const std::size_t n_accepts) \
    //                 : TcpListener<T>(n_accepts), Listener<T>(q), _socket(socket) {};

    //         ~TcpListener() = default;

    //         std::optional<T> recvOne();
    // };

    // template <typename T>
    // inline std::optional<T> TcpListener<T>::receiveOne()
    // {
    //     return std::optional<T>();
    // }

    // template <typename T>
    // inline std::optional<T> TcpListener<T>::recvOne()
    // {
    //     return Listener<T>::recvOne(*this);
    // }

    // template <typename T>
    // inline void TcpListener<T>::listenFrom()
    // {
    //     // Put the TCP Listener listening from incoming connections
    //     if (listen(_socket.getSocketFileDescriptor(), _nconn) < 0)
    //     {
    //         std::cerr << "[TcpListener::listenFrom] Failed listening: ";
    //         std::cerr << std::strerror(errno) << std::endl;
    //         throw std::runtime_error("[TcpListener::listenFrom] Failed listening");
    //     }

    //     int fd = _socket.getSocketFileDescriptor();

    //     while (!this->_stop)
    //     {
    //         struct sockaddr_in client;
    //         socklen_t addrlen = sizeof(client);
    //         int client_socket;
            
    //         // Accept incoming connections and save the source informations
    //         if ((client_socket = accept(fd, (struct sockaddr*)&client, &addrlen)) < 0)
    //         {
    //             std::cout << "[TcpListener::listenFrom] Failed Accepting: ";
    //             std::cerr << std::strerror(errno) << std::endl;
    //             continue;
    //         }

    //         _connections.push_back(client);
    //         _srcsockets.push_back(client_socket);
    //     }
    // }
}

#endif