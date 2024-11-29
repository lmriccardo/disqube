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
        // Update the socket info and check if there are incoming messages
        this->_socket.updateSocketInfo();
        struct SocketInfo* si = this->_socket.getSocketInfo();

        // Check for errors
        if (!si->active && si->socket_error)
        {
            this->stop();
            break;
        }

        // Check for timeout or no messages arrived
        if (si->timeout_ela || !si->ready_to_read) continue;

        // Receive any incoming messages from the Udp Socket
        // This actions is blocking for the poll timeout (1 sec)
        _recv.receive();

        // Before going on we need to check if during the poll
        // an external call to stop the received have been made
        if (!this->isRunning()) break;

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

bool CommonLib::Communication::UdpListener::hasStoppedWithErrors()
{
    struct SocketInfo* si = this->_socket.getSocketInfo();
    return si->socket_error && (si->error != 0);
}

int CommonLib::Communication::UdpListener::getSocketError()
{
    return this->_socket.getSocketInfo()->error;
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
    int fd = this->_socket.getSocketFileDescriptor();
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

    struct SocketInfo* si;
    struct sockaddr_in client;
    int client_socket;
    socklen_t clientlen;

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

        this->_socket.updateSocketInfo();
        si = this->_socket.getSocketInfo();

        // Check for any errors
        if (!si->active && si->socket_error)
        {
            this->stop();
            break;
        }

        if (si->timeout_ela || !si->ready_to_read) continue;

        // Before going on we need to check if during the poll
        // an external call to stop the received have been made
        if (!this->isRunning()) break;

        // Otherwise, wait for a connection
        memset(&client, 0, sizeof(client));
        clientlen = sizeof(client);

#ifdef DEBUG_MODE
        std::cout << "Start Accepting" << std::endl;
#endif

        // Accept incoming connections and save the source informations
        if ((client_socket = acceptIncoming(client, clientlen)) < 0)
        {
            si->ready_to_read = false;
            continue;
        }

#ifdef DEBUG_MODE
        std::cout << "Accepted Connection of: " << Socket::addressNumberToString(client.sin_addr.s_addr, true);
        std::cout << " on port " << ntohs(client.sin_port) << std::endl;
#endif

        _recvs[voidpos] = std::make_shared<TcpReceiver>(
            this->_queue, _socket, "TcpReceiver", client_socket, &client);

        // Start the receiver
        _recvs.at(voidpos)->start();
        _clientIdx++;

        si->ready_to_read = false;
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

bool CommonLib::Communication::TcpListener::hasStoppedWithErrors()
{
    struct SocketInfo* si = this->_socket.getSocketInfo();
    return si->socket_error && (si->error != 0);
}

int CommonLib::Communication::TcpListener::getSocketError()
{
    return this->_socket.getSocketInfo()->error;
}
