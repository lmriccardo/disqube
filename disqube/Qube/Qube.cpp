#include "Qube.hpp"

void Qube::Qube::initStateMachine()
{
    State_ptr s0 = std::make_shared<State>(StateType::QUBE_INIT, 2);
    State_ptr s1 = std::make_shared<State>(StateType::QUBE_DISCOVERING, 2);
    State_ptr s2 = std::make_shared<State>(StateType::QUBE_OPERATIVE, 3);
    State_ptr s3 = std::make_shared<State>(StateType::QUBE_MAINTENANCE, 2);
    State_ptr s4 = std::make_shared<State>(StateType::QUBE_SHUTDOWN, 1);

    s0->addTransition(s1, [](Input_t p){return p.itfReady && p.discoverFlag;});
    s0->addTransition(s2, [](Input_t p){return p.itfReady && !p.discoverFlag;});
    s1->addTransition(s4, [](Input_t p){return p.shutdown;});
    s1->addTransition(s2, [](Input_t p){return p.anyWorker;});
    s2->addTransition(s4, [](Input_t p){return p.shutdown;});
    s2->addTransition(s3, [](Input_t p){return p.maintenance;});
    s2->addTransition(s1, [](Input_t p){return !p.anyWorker;});
    s3->addTransition(s4, [](Input_t p){return p.shutdown;});
    s3->addTransition(s2, [](Input_t p){return !p.maintenance;});

    _stateMachine = std::make_shared<StateMachine>(s0);
}