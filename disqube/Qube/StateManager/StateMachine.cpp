#include "StateMachine.hpp"

using namespace Qube::StateManager;

State::StateType StateMachine::getCurrentType() const
{
    return _currState->getType();
}

void StateMachine::update(Transition::Input_t &i)
{
    State_ptr nextState = _currState->getNextState(i);
    if (nextState == nullptr) return;

    // Otherwise update the current state
    _currState = nextState;
}

bool StateMachine::checkCurrentState(Transition::Input_t &i) const
{
    return isReachable(_currState->getType(), i);
}

bool StateMachine::isReachable(
    const State::StateType &type, Transition::Input_t &i) const 
{
    State_ptr nextState = _currState->getNextState(i);
    return (
           (nextState == nullptr && _currState->getType() == type)
        || (nextState != nullptr && nextState->getType() == type)
    );
}
