#include <iostream>
#include <Qube/StateManager/StateMachine.hpp>
#include <Qube/StateManager/State.hpp>
#include <cmath>

#include "Test.hpp"

using StateMachine_ptr = Qube::StateManager::StateMachine_ptr;
using StateMachine = Qube::StateManager::StateMachine;
using State = Qube::StateManager::State;
using State_ptr = Qube::StateManager::State_ptr;
using StateType = Qube::StateManager::State::StateType;
using Input_t = Qube::StateManager::Transition::Input_t;

using namespace Test;


class TestStateMachine
{
    public:
        StateMachine_ptr sm;
        Input_t param;
        bool shutdownFlag = false;
        std::vector<StateType> visited;
        int maint_cnt = 3;
        double currTime = 0.0;
        double maintRate = 10.0;
        int disc_cnt = 0;
        int init_cnt = 0;

        void initStateMachine()
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

            sm = std::make_shared<StateMachine>(s0);
        }

        TestStateMachine(bool disc)
        {
            param = {true, disc, false, false, false};
            initStateMachine();
        }

        void run()
        {
            while (!shutdownFlag)
            {
                switch (sm->getCurrentType())
                {
                    case StateType::QUBE_INIT:
                        init();
                        break;

                    case StateType::QUBE_DISCOVERING:
                        discovering();
                        break;

                    case StateType::QUBE_OPERATIVE:
                        operative();
                        break;

                    case StateType::QUBE_MAINTENANCE:
                        maintenance();
                        break;

                    case StateType::QUBE_SHUTDOWN:
                        shutdownFlag = true;
                        break;
                    
                    default:
                        break;
                }
            }
        }

        void init()
        {
            std::cout << "init" << std::endl;
            visited.push_back(sm->getCurrentType());

            init_cnt++;

            if (init_cnt > 1)
            {
                param.shutdown = true;
            }

            if (!param.discoverFlag) param.anyWorker = true;

            int cnt = 0;
            while (sm->checkCurrentState(param))
            {
                if (cnt < 6) continue;
                param.itfReady = true;
            }

            sm->update(param);
        }

        void discovering()
        {
            std::cout << "discovering" << std::endl;
            visited.push_back(sm->getCurrentType());
            disc_cnt++;

            if (disc_cnt > 1)
            {
                param.shutdown = true;
            }
            
            int cnt = -300;
            while (sm->checkCurrentState(param))
            {
                cnt++;
                if (cnt > 0) param.anyWorker = true;
            }

            sm->update(param);
        }

        void checkMaint()
        {
            if ((currTime / maintRate) >= 1)
            {
                param.maintenance = true;
                currTime = 0.0;
            }
        }

        void operative()
        {
            std::cout << "operative" << std::endl;
            visited.push_back(sm->getCurrentType());

            while (sm->checkCurrentState(param))
            {
                currTime += 0.5;
                checkMaint();
            }

            sm->update(param);
        }

        void maintenance()
        {
            std::cout << "maintenance" << std::endl;
            visited.push_back(sm->getCurrentType());

            maint_cnt--;
            param.anyWorker = maint_cnt > 0;
            param.maintenance = false;

            sm->update(param);
        }
};


void test_without_discovering()
{
    TestStateMachine tsm(false);
    tsm.run();

    std::vector<StateType> expected = 
    {
        StateType::QUBE_INIT,
        StateType::QUBE_OPERATIVE,
        StateType::QUBE_MAINTENANCE,
        StateType::QUBE_OPERATIVE,
        StateType::QUBE_MAINTENANCE,
        StateType::QUBE_OPERATIVE,
        StateType::QUBE_MAINTENANCE,
        StateType::QUBE_OPERATIVE,
        StateType::QUBE_DISCOVERING,
        StateType::QUBE_OPERATIVE,
        StateType::QUBE_MAINTENANCE,
        StateType::QUBE_OPERATIVE,
        StateType::QUBE_DISCOVERING,
    };

    for (int i = 0; i < expected.size(); i++)
    {
        assert_eq<StateType>(expected[i], tsm.visited[i]);
    }
}

void test_discovering()
{
    TestStateMachine tsm(true);
    tsm.run();

    std::vector<StateType> expected = 
    {
        StateType::QUBE_INIT,
        StateType::QUBE_DISCOVERING,
        StateType::QUBE_OPERATIVE,
        StateType::QUBE_MAINTENANCE,
        StateType::QUBE_OPERATIVE,
        StateType::QUBE_MAINTENANCE,
        StateType::QUBE_OPERATIVE,
        StateType::QUBE_MAINTENANCE,
        StateType::QUBE_OPERATIVE,
        StateType::QUBE_DISCOVERING
    };

    for (int i = 0; i < expected.size(); i++)
    {
        assert_eq<StateType>(expected[i], tsm.visited[i]);
    }
}


int main()
{
    test_without_discovering();
    test_discovering();
    return 0;
}