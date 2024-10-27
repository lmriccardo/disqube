#ifndef _CONFIGURATIONLOADER_H
#define _CONFIGURATIONLOADER_H

#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace Configuration
{
    class ConfigurationLoader
    {
        private:
            static void readIniFile(const std::string& filepath);

        public:
            ConfigurationLoader(const std::string& filepath)
            {
                ConfigurationLoader::readIniFile(filepath);
            }
    };
}

#endif