#include "StateMachine.hpp"

Qube::StateManager::State::StateType Qube::StateManager::StateMachine::getCurrentType() const
{
    return _currState->getType();
}

void Qube::StateManager::StateMachine::update(Transition::Input &i)
{
    State_ptr nextState = _currState->getNextState(i);
    if (nextState == nullptr) return;

    // Otherwise update the current state
    _currState = nextState;
}

bool Qube::StateManager::StateMachine::checkCurrentState(Transition::Input &i) const
{
    return isReachable(_currState->getType(), i);
}

bool Qube::StateManager::StateMachine::isReachable(
    const State::StateType &type, Transition::Input &i) const 
{
    State_ptr nextState = _currState->getNextState(i);
    return (
           (nextState == nullptr && _currState->getType() == type)
        || (nextState != nullptr && nextState->getType() == type)
    );
}
