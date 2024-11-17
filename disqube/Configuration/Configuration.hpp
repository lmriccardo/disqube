#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <memory>

#include <Configuration/Property.hpp>

namespace Configuration
{
    class DisqubeConfiguration
    {
        private:
            std::unordered_map<std::string, PropertyGroup_ptr> _groups;

            void readIniFile(const std::string& filepath);
            static std::vector<std::string> splitString(
                const std::string& line, const std::string& delimiter);
            
            std::string getConfigurationValue(
                const std::string& gname, const std::string& pname);

        public:
            DisqubeConfiguration(const std::string& iniFile)
            {
                readIniFile(iniFile);
            }

            ~DisqubeConfiguration() = default;

            void addGroup(PropertyGroup_ptr group);

            // Environment Configuration Values
            unsigned int getNumOfQubes();
            unsigned int getMaxNumOfQubes();
            bool isDiscoverEnabled();
            std::string getQubesSubnetAddress();
            std::string getQubesSubnetMask(); 
            std::string getQubesSubnetGateway();

            // Network Configuration Values
            std::string getNetworkInterface();
            unsigned short getTcpSenderPort();
            unsigned short getTcpListenerPort();
            unsigned short getUdpSenderPort();
            unsigned short getUdpListenerPort();
            unsigned short getBroadcastPort();
            std::size_t getTcpMaxCapacityQueue();
            std::size_t getTcpMaxNumOfConnections();
            std::size_t getUdpMaxCapacityQueue();
            
            // Logging configuration
            bool getLogOnFile();
            std::string getLogRootFolder();
    };
}

#endif