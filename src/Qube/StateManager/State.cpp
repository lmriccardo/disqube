#include "State.hpp"

using namespace Qube::StateManager;

State_ptr Transition::getTargetState() const
{
    return _target;
}

bool Transition::evaluate(Input_t &input) const
{
    return _fn(input);
}

void State::addTransition(Transition_ptr &transition)
{
    if (_nofTransitions >= _transitions.capacity())
    {
        throw std::overflow_error("[State::addTransition]"
            " No more space for additional transitions");
    }

    _transitions[_nofTransitions++] = transition;
}

void State::addTransition(State_ptr target, Transition::_Condition_t condition)
{
    Transition_ptr t = std::make_shared<Transition>(target, condition);
    addTransition(t);
}

std::size_t State::getNumberOfTransitions() const
{
    return _nofTransitions;
}

std::size_t State::getMaxTransitionCapacity() const
{
    return _transitions.capacity();
}

State_ptr State::getNextState(Transition::Input_t &input)
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

State::StateType State::getType() const
{
    return _type;
}
