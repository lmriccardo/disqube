#ifndef _QUBE_H
#define _QUBE_H

#include <Qube/StateManager/State.hpp>
#include <Qube/StateManager/StateMachine.hpp>
#include <Qube/QubeInterface.hpp>
#include <Configuration/Configuration.hpp>
#include <Logging/DisqubeLogger.hpp>

using StateMachine_ptr = Qube::StateManager::StateMachine_ptr;
using StateMachine = Qube::StateManager::StateMachine;
using State = Qube::StateManager::State;
using State_ptr = Qube::StateManager::State_ptr;
using StateType = Qube::StateManager::State::StateType;
using Input_t = Qube::StateManager::Transition::Input;
using DisqubeConfiguration = Configuration::DisqubeConfiguration;
using DisqubeLogger_ptr = Qube::Logging::DisqubeLogger_ptr;

namespace Qube
{
    class Qube
    {
        private:
            StateMachine_ptr         _stateMachine; // The State Machine of Qube
            QubeInterface_ptr        _itf;          // The qube interface
            Input_t                  _qubeData;     // Some informations for state machine
            DisqubeConfiguration_ptr _conf;         // General configuration
            DisqubeLogger_ptr        _logger;       // A single prompt/file Logger
            bool                     _shutdownFlag; // A shutdown flag
            std::string              _confFile;     // The configuration file path
            bool                     _isMaster;     // If the current qube is a master or not

            static unsigned int generateId(); // Generates the ID for logging
            void initStateMachine(); // Initialize the state machine

            void init(); // The initial method (INIT State of State Machine)

        public:
            Qube(const std::string& confFile) : _confFile(confFile), _shutdownFlag(false) {};

            void setMasterFlag(bool value);
            bool isMaster() const;
            bool isDiscoverEnabledAtStartup() const;
            void run(); // The main method of the Qube
    };
};

#endif