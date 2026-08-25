#include <gtest/gtest.h>

#include "drone_project/config/project_config.hpp"
#include "system_state_machine.hpp"


// ##################################################
// ################## INIT TESTS ####################
// ##################################################

// Test that the state machine starts in the INIT state
TEST(SystemStateMachineTest, StartsInInit)
{
    SystemStateMachine fsm;

    EXPECT_EQ(fsm.getState(), SystemState::INIT);
}

// Test that the state machine transitions from INIT to USB when USB is connected
// Also test that USB has priority over other states
TEST(SystemStateMachineTest, InitToUsb)
{
    SystemStateMachine fsm;

    SystemInputs inputs{};
    inputs.usb_connected    = true;
    inputs.imu_ok           = true;

    fsm.update_state(inputs);

    EXPECT_EQ(fsm.getState(), SystemState::USB);
}

// Test that the state machine transitions from INIT to DISARMED
TEST(SystemStateMachineTest, InitToDisarmed)
{
    SystemStateMachine fsm;

    // Check that the state machine starts in INIT state
    EXPECT_EQ(fsm.getState(), SystemState::INIT);

    SystemInputs inputs{};
    inputs.usb_connected    = false;
    inputs.imu_ok           = true;

    // Until there is not enough cycles, it should remain in INIT state
    for (uint32_t i = 0; i < config::tasks::INIT_COUNTDOWN-1; ++i) {
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::INIT);
    }

    // After enough cycles, it should transition to DISARMED state
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::DISARMED);
}

// Test that the state machine transitions from INIT to ERROR
TEST(SystemStateMachineTest, InitToError)
{
    SystemStateMachine fsm;

    // Check that the state machine starts in INIT state
    EXPECT_EQ(fsm.getState(), SystemState::INIT);

    SystemInputs inputs{};
    inputs.usb_connected    = false;
    inputs.imu_ok           = false;

    // Until there is not enough cycles, it should remain in INIT state
    for (uint32_t i = 0; i < config::tasks::IMU_COUNTDOWN-1; ++i) {
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::INIT);
    }

    // After enough cycles, it should transition to ERROR state
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::ERROR);
}

// Test that the state machine transitions from INIT to ERROR only when IMU fails consecutively
TEST(SystemStateMachineTest, InitToErrorConsecutiveCycles)
{
    SystemStateMachine fsm;

    SystemInputs inputs{};
    inputs.usb_connected    = false;
    inputs.imu_ok           = false;

    // Until there is not enough cycles, it should remain in INIT state
    for (uint32_t i = 0; i < config::tasks::IMU_COUNTDOWN - 1; ++i) {
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::INIT);
    }

    // A good IMU reading should reset the counter and keep the state in INIT
    inputs.imu_ok = true;
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::INIT);

    // New errors should start counting again
    inputs.imu_ok = false;
    for (uint32_t i = 0; i < config::tasks::IMU_COUNTDOWN - 1; ++i) {
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::INIT);
    }

    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::ERROR);
}


// ##################################################
// ################ DISARMED TESTS ##################
// ##################################################


