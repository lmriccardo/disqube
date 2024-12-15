#ifndef _QUBE_H
#define _QUBE_H

#include <CommonLib/Concurrency/WakeUpTimer.hpp>
#include <Qube/StateManager/State.hpp>
#include <Qube/StateManager/StateMachine.hpp>
#include <Qube/QubeInterface.hpp>
#include <Configuration/Configuration.hpp>
#include <Logging/DisqubeLogger.hpp>

namespace Qube
{
    class Qube
    {
    protected:
        struct
        {
            int tcperror_l; // Error for TCP listener
            int tcperror_s; // Error for TCP sender
            int udperror_l; // Error for UDP listener
            int udperror_s; // Error for UDP sender
        } _error;

        StateManager::StateMachine_ptr _stateMachine;  // The State Machine of Qube
        QubeInterface_ptr _itf;                        // The qube interface
        StateManager::Transition::Input_t _qubeData;   // Some informations for state machine
        Configuration::DisqubeConfiguration_ptr _conf; // General configuration
        Logging::DisqubeLogger_ptr _logger;            // A single prompt/file Logger
        Lib::Concurrency::WakeUpTimer_ptr _timer;      // The wake up timer

        bool _shutdownFlag;    // A shutdown flag
        std::string _confFile; // The configuration file path
        bool _isMaster;        // If the current qube is a master or not

        static unsigned int generateId();              // Generates the ID for logging
        void initStateMachine();                       // Initialize the state machine
        int checkDiagnosticResults();                  // Check diagnostic results for TCP and UDP interfaces
        void handleDiagnosticErrors(const int result); // Handle diagnostic results in case of errors

        void init();     // The initial method (INIT State of State Machine)
        void shutdown(); // The shutdown state

        virtual void discover() = 0;
        virtual void operative() = 0;
        virtual void processMessage(const Lib::Network::ReceivedData &recvData) = 0;

    public:
        Qube(const std::string &confFile) : _confFile(confFile), _shutdownFlag(false)
        {
            memset(&this->_error, 0, sizeof(this->_error));
            this->initStateMachine();
        };

        void setMasterFlag(bool value);          // Set the current qube as master or client
        bool isMaster() const;                   // Check if the current qube is a master
        bool isDiscoverEnabledAtStartup() const; // Flag indicating discovering protocol actuation
        void run();                              // The main method of the Qube
    };

    class QubeManager : public Qube
    {
    private:
        void discover() override;  // The discover state (DISCOVERING State of the State Machine)
        void operative() override; // The operative state

        void processMessage(const Lib::Network::ReceivedData &recvData) override;

    public:
        QubeManager(const std::string &confFile) : Qube(confFile)
        {
            setMasterFlag(true);
        };
    };

    class QubeWorker : public Qube
    {
    private:
        void discover() override {}; // The discover state (DISCOVERING State of the State Machine)
        void operative() override; // The operative state for the Qube worker

        void processMessage(const Lib::Network::ReceivedData &recvData) override;
        void handleDiscoverHello(Lib::Network::ByteBuffer_ptr& buffer);

    public:
        QubeWorker(const std::string &confFile) : Qube(confFile)
        {
            setMasterFlag(false);
        };
    };
};

#endif