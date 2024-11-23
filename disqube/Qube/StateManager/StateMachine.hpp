#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

#include <Qube/StateManager/State.hpp>

namespace Qube::StateManager
{
    class StateMachine
    {
        private:
            State_ptr _currState;
        
        public:
            StateMachine(State_ptr initState) : _currState(initState) {};

            State::StateType getCurrentType() const;
            void update(struct Transition::Input& i);
            bool checkCurrentState(struct Transition::Input& i) const;
            
            bool isReachable(const State::StateType& type, 
                struct Transition::Input& i) const;
    };

    typedef std::shared_ptr<StateMachine> StateMachine_ptr;
};

#endif