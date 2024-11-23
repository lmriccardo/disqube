#include "State.hpp"

Qube::StateManager::State_ptr Qube::StateManager::Transition::getTargetState() const
{
    return _target;
}

bool Qube::StateManager::Transition::evaluate(Input &input) const
{
    return _fn(input);
}

void Qube::StateManager::State::addTransition(Transition_ptr &transition)
{
    if (_nofTransitions >= _transitions.capacity())
    {
        throw std::overflow_error("[State::addTransition]"
            " No more space for additional transitions");
    }

    _transitions[_nofTransitions++] = transition;
}

void Qube::StateManager::State::addTransition(State_ptr target, Transition::_Condition_t condition)
{
    Transition_ptr t = std::make_shared<Transition>(target, condition);
    addTransition(t);
}

std::size_t Qube::StateManager::State::getNumberOfTransitions() const
{
    return _nofTransitions;
}

std::size_t Qube::StateManager::State::getMaxTransitionCapacity() const
{
    return _transitions.capacity();
}

Qube::StateManager::State_ptr Qube::StateManager::State::getNextState(Transition::Input &input)
{
    for (auto& transition: _transitions)
    {
        if (transition->evaluate(input))
        {
            return transition->getTargetState();
        }
    }

    return nullptr;
}

Qube::StateManager::State::StateType Qube::StateManager::State::getType() const
{
    return _type;
}
