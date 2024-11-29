#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

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
                const std::string& gname, const std::string& pname) const;

        public:
            // Default constructor
            DisqubeConfiguration() {};

            DisqubeConfiguration(const std::string& iniFile)
            {
                readIniFile(iniFile);
            }

            ~DisqubeConfiguration() = default;

            void addGroup(PropertyGroup_ptr group);

            // Environment Configuration Values
            unsigned int getNumOfQubes() const;
            unsigned int getMaxNumOfQubes() const;
            bool isDiscoverEnabled() const;
            std::string getQubesSubnetAddress() const;
            std::string getQubesSubnetMask() const; 
            std::string getQubesSubnetGateway() const;

            // Network Configuration Values
            std::string getNetworkInterface() const;
            unsigned short getTcpSenderPort() const;
            unsigned short getTcpListenerPort() const;
            unsigned short getUdpSenderPort() const;
            unsigned short getUdpListenerPort() const;
            unsigned short getBroadcastPort() const;
            std::size_t getTcpMaxCapacityQueue() const;
            std::size_t getTcpMaxNumOfConnections() const;
            std::size_t getUdpMaxCapacityQueue() const;
            
            // Logging configuration
            bool getLogOnFile() const;
            std::string getLogRootFolder() const;
    };

    typedef std::shared_ptr<DisqubeConfiguration> DisqubeConfiguration_ptr;
}

#endif