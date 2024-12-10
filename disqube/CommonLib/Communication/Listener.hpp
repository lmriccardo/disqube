#ifndef _LISTENER_HPP
#define _LISTENER_HPP

#include <iostream>
#include <optional>
#include <algorithm>
#include <CommonLib/Communication/Socket.hpp>
#include <CommonLib/Communication/Message.hpp>
#include <CommonLib/Communication/Receiver.hpp>
#include <CommonLib/Concurrency/Thread.hpp>
#include <CommonLib/Concurrency/Queue.hpp>

namespace Lib::Network
{
    class Listener : public Concurrency::Thread
    {
    protected:
        Concurrency::Queue_ptr<struct ReceivedData> _queue; // The queue of received and converted messages
        bool _sigstop;                                      // Flag indicating when the listener must be stopped

    public:
        Listener(const Concurrency::Queue_ptr<struct ReceivedData>& queue, const std::string &name)
            : Concurrency::Thread(name), _queue(queue), _sigstop(false) {};

        Listener(const std::size_t capacity, const std::string &name) : Thread(name)
        {
            _queue = std::make_shared<Concurrency::Queue<struct ReceivedData>>(capacity);
            _sigstop = false;
        };

        void stop();
        bool isRunning() const;
        struct ReceivedData getElement();
        Concurrency::Queue_ptr<struct ReceivedData> getQueue();

        virtual const Socket &getSocket() = 0;
        virtual bool hasStoppedWithErrors() = 0;
        virtual int getSocketError() = 0;
    };

    class UdpListener : public Listener
    {
    private:
        UdpSocket _socket; // The Udp Socket
        UdpReceiver _recv; // The object receiving the messages

    public:
        /**
         * @param ip The Ip address needed to bind the socket
         * @param port The port number, also needed by the socket
         * @param q A shared pointer to a Queue
         */
        UdpListener(const std::string &ip, unsigned short port, const Concurrency::Queue_ptr<struct ReceivedData>& q)
            : Listener(q, "UdpListener"), _socket(ip, port), _recv(q, _socket) {};

        UdpListener(const std::string &ip, unsigned short port, const std::size_t c)
            : Listener(c, "UdpListener"), _socket(ip, port), _recv(this->_queue, _socket) {};

        UdpListener(const UdpSocket &s, const Concurrency::Queue_ptr<struct ReceivedData>& q)
            : Listener(q, "UdpListener"), _socket(s), _recv(q, _socket) {};

        UdpListener(const UdpSocket &s, const std::size_t c)
            : Listener(c, "UdpListener"), _socket(s), _recv(this->_queue, _socket) {};

        ~UdpListener()
        {
            _socket.closeSocket();
        }

        using Listener::isRunning;

        void run() override;
        const UdpSocket &getSocket() override;
        bool hasStoppedWithErrors() override;
        int getSocketError() override; 
    };

    class TcpListener : public Listener
    {
    private:
        TcpSocket _socket;                                // The Tcp Socket
        std::vector<std::shared_ptr<TcpReceiver>> _recvs; // The vector of all receivers
        std::size_t _clientIdx;                           // The current number of clients connected to the server

        int handleReceivers();
        int acceptIncoming(struct sockaddr_in &client, socklen_t clientlen);

    public:
        TcpListener(const std::string &ip, unsigned short port, const Concurrency::Queue_ptr<struct ReceivedData>& queue, 
            const std::size_t nconn) : Listener(queue, "TcpListener"), _socket(ip, port), 
                _recvs(nconn, nullptr), _clientIdx(0) {};

        TcpListener(const std::string &ip, unsigned short port, const std::size_t capacity, const std::size_t nconn)
            : Listener(capacity, "TcpListener"), _socket(ip, port), _recvs(nconn, nullptr), _clientIdx(0) {};

        TcpListener(const TcpSocket &s, const Concurrency::Queue_ptr<struct ReceivedData>& queue, const std::size_t nconn)
            : Listener(queue, "TcpListener"), _socket(s), _recvs(nconn, nullptr), _clientIdx(0) {};

        TcpListener(const TcpSocket &s, const std::size_t capacity, const std::size_t nconn)
            : Listener(capacity, "TcpListener"), _socket(s), _recvs(nconn, nullptr), _clientIdx(0) {};

        ~TcpListener()
        {
            _socket.closeSocket();
        }

        using Listener::isRunning;

        void run() override;
        void setTimeout(long int sec, long int usec);
        void setTimeout(long int sec);
        const TcpSocket &getSocket() override;
        bool hasStoppedWithErrors() override;
        int getSocketError() override;
    };
}

#endif