#include "Listener.hpp"

void CommonLib::Communication::Listener::stop()
{
    _sigstop = true;
}

bool CommonLib::Communication::Listener::isRunning() const
{
    return !_sigstop;
}

struct CommonLib::Communication::ReceivedData CommonLib::Communication::Listener::getElement()
{
    return _queue->pop();
}

Queue_ptr<struct CommonLib::Communication::ReceivedData> CommonLib::Communication::Listener::getQueue()
{
    return _queue;
}

void CommonLib::Communication::UdpListener::run()
{
    // Loop until the stop signal becomes true
    while (!this->_sigstop)
    {
        // Receive any incoming messages from the Udp Socket
        // This action is blocking, with no timeout associated
        _recv.receive();

        // After received we need to check whether the receiver
        // has received a stop message
        if (_recv.hasStopped())
        {
            this->stop();
            continue;
        }
    }
}

const CommonLib::Communication::UdpSocket &CommonLib::Communication::UdpListener::getSocket()
{
    return _socket;
}

int CommonLib::Communication::TcpListener::handleReceivers()
{
    int voidpos = _recvs.capacity() - 1;

    // First step of each cycle is to detect which receiver has ends
    // its lifetime. If this happens than a new connection can be
    // established by a new client
    for (int idx = 0; idx < _recvs.capacity(); idx++)
    {
        auto receiver = _recvs.at(idx);

        // If there is no receiver continue
        if (receiver == nullptr)
        {
            // We would like to have the first possible void position
            voidpos = std::min(voidpos, idx);
            continue;
        }
        
        // Otherwise, we need to check that the receiver is still running
        if (receiver->hasStopped())
        {
            // Join the thread and remove it from the list
            receiver->join();
            _recvs[idx] = nullptr;
            _clientIdx--;
        }
    }

    return voidpos;
}

int CommonLib::Communication::TcpListener::acceptIncoming(struct sockaddr_in& client, socklen_t clientlen)
{
    // Get the socket file descriptor
    int fd = _socket.getSocketFileDescriptor();

    // Create the sets of readable sockets
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    // Set the timeout duration
    struct timeval timeout = _socket.getTimeout();

    // Wait for the listening socket to become readable (incoming connection)
    if (select(fd + 1, &readfds, nullptr, nullptr, &timeout) <= 0)
    {
        // Returns -1, meaning that either there was an error or the
        // timeout expired when waiting for incoming connections
        std::cerr << "[TcpListener::acceptIncoming] Error when selecting: ";
        std::cerr << std::strerror(errno) << std::endl;
        return -1;
    }

    // If we reach here, there is an incoming connection, proceed to accept it
    if (!FD_ISSET(fd, &readfds)) return -1;
   
    int client_socket;
    if ((client_socket = accept(fd, (struct sockaddr*)&client, &clientlen)) < 0)
    {

        std::cerr << "[TcpListener::acceptIncoming] Error when accepting: ";
        std::cerr << std::strerror(errno) << "->" << client_socket << std::endl;
        return -1;
    }

    return client_socket;
}

void CommonLib::Communication::TcpListener::run()
{
    // Take the file descriptor of the current TCP socket
    int fd = _socket.getSocketFileDescriptor();

#ifdef DEBUG_MODE
    std::cout << "Start Listening" << std::endl;
#endif

    // Put the TCP listener in listening mode from incoming connections
    if (listen(fd, _recvs.capacity()) < 0)
    {
        std::cerr << "[TcpListener::listenFrom] Failed listening: ";
        std::cerr << std::strerror(errno) << std::endl;
        throw std::runtime_error("[TcpListener::listenFrom] Failed listening");
    }

    // Loop until the listener is stopped. Differently from the Udp Listener
    // the TCP listener accepts client connections and creates, for each new 
    // connections, a receiver thread.
    while (!this->_sigstop)
    {
        std::cout << this->_sigstop << std::endl;

        // Handle all the receivers
        int voidpos = handleReceivers();

        // If there are no possible available connections sleep and continue
        if (_clientIdx > _recvs.capacity() - 1)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        // Otherwise, wait for a connection
        struct sockaddr_in client;
        memset(&client, 0, sizeof(client));
        socklen_t clientlen = sizeof(client);
        int client_socket;

#ifdef DEBUG_MODE
        std::cout << "Start Accepting" << std::endl;
#endif

        // Accept incoming connections and save the source informations
        if ((client_socket = acceptIncoming(client, clientlen)) < 0) continue;

#ifdef DEBUG_MODE
        std::cout << "Accepted Connection of: " << Socket::addressNumberToString(client.sin_addr.s_addr, true);
        std::cout << " on port " << ntohs(client.sin_port) << std::endl;
#endif

        _recvs[voidpos] = std::make_shared<TcpReceiver>(
            this->_queue, _socket, "TcpReceiver", client_socket, &client);

        // Start the receiver
        _recvs.at(voidpos)->start();

        _clientIdx++;
    }

    // Once this thread has stopped we need to join all 
    // the remaining receivers.
    for (auto &recv: _recvs)
    {
        if (!(recv == nullptr))
        {
            recv->join();
        }
    }
}

void CommonLib::Communication::TcpListener::setTimeout(long int sec, long int usec)
{
    _socket.setTimeout(sec, usec);
}

void CommonLib::Communication::TcpListener::setTimeout(long int sec)
{
    _socket.setTimeout(sec);
}

const CommonLib::Communication::TcpSocket &CommonLib::Communication::TcpListener::getSocket()
{
    return _socket;
}
