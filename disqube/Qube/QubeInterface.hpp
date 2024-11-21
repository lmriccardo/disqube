#ifndef _QUBEINTERFACE_H
#define _QUBEINTERFACE_H

#include <iostream>
#include <string>
#include <CommonLib/Communication/Interface.hpp>
#include <CommonLib/Communication/Message.hpp>
#include <Configuration/Configuration.hpp>
#include <Logging/DisqubeLogger.hpp>

using DisqubeConfiguration = Configuration::DisqubeConfiguration;
using UdpCommunicationInterface_ptr = CommonLib::Communication::UdpCommunicationInterface_ptr;
using TcpCommunicationInterface_ptr = CommonLib::Communication::TcpCommunicationInterface_ptr;
using TcpCommunicationInterface = CommonLib::Communication::TcpCommunicationInterface;
using UdpCommunicationInterface = CommonLib::Communication::UdpCommunicationInterface;
using Socket = CommonLib::Communication::Socket;
using SubnetInfo = CommonLib::Communication::SubnetInfo;

namespace Qube
{
    /**
     * @class Qube::QubeInterface
     * 
     * Each cube is composed of a configuration and two communication interfaces:
     * an UDP Communication Interface and a TCP Interface. The UDP Communication
     * is used for those messages that do not require any layer of reliability or
     * that needs to be sent as fast as possible with no overhead due to re-transmission
     * and integrity check mechanism.
     * 
     * Finally, a cube also contains a message dispatcher that every time dequeue
     * a message from one of the two queues and perform releated operations.
     * 
     * A Qube can take up to 2 role: master or worker. A master qube is the one
     * dispatching the workload between multiple workers. There can only be one
     * master while all the others needs to be workers. The main difference between
     * these two roles is the kind of messages they can receive and send. For
     * example, a master qube can never receive a job posting message, as well
     * as a worker qube cannot send job posting requests.
     */
    class QubeInterface
    {
        private:
            DisqubeConfiguration          _conf;     // General configuration
            UdpCommunicationInterface_ptr _udpitf;   // Udp Communication Interface
            TcpCommunicationInterface_ptr _tcpitf;   // Tcp Communication Interface
            Logging::DisqubeLogger_ptr    _logger;   // Generic logging class
            bool                          _isMaster; // Master Qube interface or not.

            // A list of pair (ip, port no) for each node connected.
            std::vector<std::pair<std::string, unsigned short>> _nodes;

            static unsigned int generateId();
            
            void initUdpInterface(const std::string& ip);
            void initTcpInterface(const std::string& ip);
            void logInit();
            void init();

        public:
            QubeInterface(bool isMaster, const std::string& inFile) 
                : _conf(inFile), _isMaster(isMaster)
            {
                // Create the logger
                _logger = std::make_shared<Logging::DisqubeLogger>(
                    generateId(), _conf.getLogOnFile(), 
                    _conf.getLogRootFolder());

                // Call the init method
                init();
            }

            QubeInterface(bool isMaster, const std::string& inFile, 
                Logging::DisqubeLogger_ptr& logger
            ) : _isMaster(isMaster), _conf(inFile), _logger(logger)
            {
                init(); // Call the init method
            }

            bool isMaster();
            void qubeDiscovering();
    };
}

#endif