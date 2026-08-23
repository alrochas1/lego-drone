#include <gtest/gtest.h>

#include "system_state_machine.hpp"

TEST(SystemStateMachineTest, StartsInInit)
{
    SystemStateMachine fsm;

    EXPECT_EQ(fsm.getState(), SystemState::INIT);
}