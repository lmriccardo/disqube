#include <CommonLib/Concurrency/Thread.hpp>
#include <CommonLib/Concurrency/Queue.hpp>
#include <CommonLib/Communication/Socket.hpp>

using Thread = CommonLib::Concurrency::Thread;
using UdpSocket = CommonLib::Communication::UdpSocket;
using Socket = CommonLib::Communication::Socket;

template <typename T>
using Queue_ptr = CommonLib::Concurrency::Queue_ptr<T>;

template <typename T>
class Listener : public Thread
{
    protected:
        Queue_ptr<T> queue;

    public:
        Listener(Queue_ptr<T> q, const std::string& name) : Thread(name), queue(q) {};
        Listener(const std::size_t capacity, const std::string& name) : Thread(name) 
        {
            queue = std::make_shared<CommonLib::Concurrency::Queue<T>>(capacity);
        };

        virtual void listenFrom() = 0;

        void run()
        {
            listenFrom();
        }
};

template <typename T, typename U, typename = std::enable_if_t<std::is_base_of<Socket, U>::value>>
class Receiver
{
    protected:
        U            socket;
        Queue_ptr<T> queue;

    public:
        Receiver(Queue_ptr<T> q, const U& s): queue(q), socket(s) {};

        virtual void receive() = 0;
        virtual T handleReceivedMessages(
            const unsigned char* buff, const std::size_t n, struct sockaddr_in* src) = 0;
};

template <typename T>
class UdpReceiver : public Receiver<T, UdpSocket>
{
    public:
        UdpReceiver(Queue_ptr<T> q, const UdpSocket& s) : Receiver<T, UdpSocket>(q, s) {};

        void receive()
        {
            std::cout << "Receiving one message" << std::endl;
        }
};

template <typename T, typename U, typename = std::enable_if_t<std::is_base_of<UdpReceiver<T>, U>::value>>
class UdpListener : public Listener<T>
{
    private:
        UdpSocket socket;
        U recv;

    public:
        UdpListener(const std::string& ip, unsigned short port, Queue_ptr<T> q)
            : Listener<T>(q, "UdpListener"), socket(ip, port), recv(q, socket) {};

        UdpListener(const std::string& ip, unsigned short port, const std::size_t c)
            : Listener<T>(c, "UdpListener"), socket(ip, port), recv(this->queue, socket) {};

        UdpListener(const UdpSocket& s, Queue_ptr<T> q)
            : Listener<T>(q, "UdpListener"), socket(s), recv(q, socket) {};

        UdpListener(const UdpSocket& s, const std::size_t c)
            : Listener<T>(c, "UdpListener"), socket(s), recv(this->queue, socket) {};

        void listenFrom()
        {
            std::cout << "Listening" << std::endl;
        }
};

class MyUdpReceiver : public UdpReceiver<int>
{
    public:
        using UdpReceiver<int>::UdpReceiver;

        int handleReceivedMessages(
            const unsigned char* buff, const std::size_t n, struct sockaddr_in* src
        ) {
            return 1;
        }
};


int main()
{
    UdpListener<int, MyUdpReceiver> listener("127.0.0.1", 1234, 10);
    return 0;
}