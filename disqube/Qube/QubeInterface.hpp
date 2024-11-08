#ifndef _QUBEINTERFACE_H
#define _QUBEINTERFACE_H

#pragma once

#include <iostream>
#include <string>

#include <CommonLib/Communication/Interface.hpp>
#include <Configuration/Configuration.hpp>

using DisqubeConfiguration = Configuration::DisqubeConfiguration;
using UdpCommunicationInterface_ptr = CommonLib::Communication::UdpCommunicationInterface_ptr;
using TcpCommunicationInterface_ptr = CommonLib::Communication::TcpCommunicationInterface_ptr;
using TcpCommunicationInterface = CommonLib::Communication::TcpCommunicationInterface;
using UdpCommunicationInterface = CommonLib::Communication::UdpCommunicationInterface;
using Socket = CommonLib::Communication::Socket;

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
     */
    class QubeInterface
    {
        private:
            DisqubeConfiguration          _conf;   // General configuration
            UdpCommunicationInterface_ptr _udpitf; // Udp Communication Interface
            TcpCommunicationInterface_ptr _tcpitf; // Tcp Communication Interface

            void initUdpInterface(const std::string& ip);
            void initTcpInterface(const std::string& ip);

        public:
            QubeInterface(const std::string& inFile) : _conf(inFile)
            {
                // Create Udp Communication Interface
                std::string ip = Socket::getInterfaceIp(_conf.getNetworkInterface());
                initUdpInterface(ip);

                // Create Tcp Communication Interface
                initTcpInterface(ip);
            }
    };
}

#endif