#include "Listener.hpp"

void CommonLib::Communication::Listener::stop()
{
    _sigstop = true;
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

        // Accept incoming connections and save the source informations
        if ((client_socket = accept(fd, (struct sockaddr*)&client, &clientlen)) < 0)
        {
            std::cerr << "[TcpListener::listenFrom] Failed Accepting: ";
            std::cerr << std::strerror(errno) << std::endl;
            continue;
        }

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
