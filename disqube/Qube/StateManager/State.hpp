#ifndef _STATE_H
#define _STATE_H

#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>

namespace Qube::StateManager
{
    // Forward declaration of a state
    class State;
    typedef std::shared_ptr<State> State_ptr;

    /**
     * A unidirectional transition to a target state. The source state is
     * the one this transition is attached to. The condition is a boolean
     * function that if evaluates to true than the target state is a
     * reachable state.
     */
    class Transition
    {
    public:
        struct Input_t
        {
            bool itfReady;     // The qube interface is ready
            bool discoverFlag; // The discover flag (always true after init)
            bool isMaster;     // If the current qube is a master or not
            bool anyWorker;    // If any worker is still alive
            bool maintenance;  // If is the time to go in maintenance
            bool shutdown;     // Shutdown flag
        };

        typedef std::function<bool(struct Input_t &)> _Condition_t;

    private:
        State_ptr _target; // The target state of the transition
        _Condition_t _fn;  // The condition of the transition

    public:
        Transition(State_ptr target, _Condition_t condition)
            : _target(target), _fn(condition) {};

        State_ptr getTargetState() const;
        bool evaluate(struct Input_t &input) const;
    };

    typedef std::shared_ptr<Transition> Transition_ptr;

    class State
    {
    public:
        enum StateType
        {
            QUBE_INIT,        // Init state, conf loading, interface binding etc.
            QUBE_DISCOVERING, // Discover Mode, protocol application (only master)
            QUBE_OPERATIVE,   // Operative state, job posting, job performing, logging ...
            QUBE_MAINTENANCE, // Maintenance state (only master)
            QUBE_SHUTDOWN     // Stop the running Qube
        };

    private:
        StateType _type;                          // The state type
        std::vector<Transition_ptr> _transitions; // All the transitions of the state
        std::size_t _nofTransitions = 0;          // The current number of transitions

    public:
        State(const StateType &type, const std::size_t maxNofTransitions)
            : _type(type)
        {
            _transitions.resize(maxNofTransitions, nullptr);
        }

        void addTransition(Transition_ptr &transition);
        void addTransition(State_ptr target, Transition::_Condition_t condition);
        std::size_t getNumberOfTransitions() const;
        std::size_t getMaxTransitionCapacity() const;
        State_ptr getNextState(struct Transition::Input_t &input);
        StateType getType() const;
    };
};

#endif