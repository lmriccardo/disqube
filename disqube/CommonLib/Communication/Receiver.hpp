#ifndef _RECEIVER_HPP
#define _RECEIVER_HPP

#include <iostream>
#include <optional>
#include <CommonLib/Communication/Socket.hpp>
#include <CommonLib/Concurrency/Thread.hpp>
#include <CommonLib/Concurrency/Queue.hpp>
#include <CommonLib/Communication/ByteBuffer.hpp>
#include <CommonLib/Communication/Message.hpp>

#define RECVBUFFSIZE 4096

namespace Lib::Network
{
    class Receiver
    {
    protected:
        Concurrency::Queue_ptr<struct ReceivedData> _queue;
        bool _stopped;

        static struct ReceivedData handleReceivedMessages(unsigned char *buff,
                                                          const std::size_t n, struct sockaddr_in *src);

    public:
        Receiver(const Concurrency::Queue_ptr<struct ReceivedData> &queue) : _queue(queue), _stopped(false) {};

        virtual void receive() = 0;
        bool hasStopped() const;
        void stop();
    };

    class UdpReceiver : public Receiver
    {
    protected:
        UdpSocket _socket; // The Udp Socket of the listener

    public:
        UdpReceiver(const Concurrency::Queue_ptr<struct ReceivedData> &queue, const UdpSocket &socket)
            : Receiver(queue), _socket(socket) {};

        void receive() override;
    };

    class TcpReceiver : public Receiver, public Concurrency::Thread
    {
    protected:
        TcpSocket _socket;           // The Tcp Socket of the listener
        int _clientfd;               // Socket file descriptor of accepted client
        struct sockaddr_in *_client; // Structure containings all client information

    private:
        void receive() override;
        void run() override;

    public:
        TcpReceiver(
            const Concurrency::Queue_ptr<struct ReceivedData> &queue, const TcpSocket &socket, 
            const std::string &name, int clientfd, struct sockaddr_in *client)
            : Receiver(queue), Thread(name), _socket(socket),
              _clientfd(clientfd), _client(client) {};

        bool isRunning() const override;
    };
}

#endif