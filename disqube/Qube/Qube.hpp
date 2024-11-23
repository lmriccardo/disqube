#ifndef _QUBE_H
#define _QUBE_H

#include <Qube/StateManager/State.hpp>
#include <Qube/StateManager/StateMachine.hpp>

using StateMachine_ptr = Qube::StateManager::StateMachine_ptr;
using StateMachine = Qube::StateManager::StateMachine;
using State = Qube::StateManager::State;
using State_ptr = Qube::StateManager::State_ptr;
using StateType = Qube::StateManager::State::StateType;
using Input_t = Qube::StateManager::Transition::Input;

namespace Qube
{
    class Qube
    {
        private:
            StateMachine_ptr _stateMachine; // The State Machine of Qube

            void initStateMachine(); // Initialize the state machine
    };
};

#endif