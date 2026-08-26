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

// Test that the state machine transitions from DISARMED to ARMED when throttle is low
TEST(SystemStateMachineTest, DisarmedToArmed)
{
    SystemStateMachine fsm;
    SystemInputs inputs{};
    
    // Get to DISARMED
    inputs.imu_ok   = true;
    inputs.rc_ok    = true;

    for (uint32_t i = 0; i < config::tasks::INIT_COUNTDOWN; ++i) {  // TODO: Check counter
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::DISARMED);

    // Check that the state machine remains in DISARMED when RC is not ok
    inputs.rc_ok    = false;
    for (uint32_t i = 0; i < config::tasks::RC_COUNTDOWN-1; ++i) {  // TODO: Check counter
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::DISARMED);
    }

    // Check that the state machine remains in DISARMED until RC is ok
    inputs.rc_ok    = true;
    for (uint32_t i = 0; i < config::tasks::RC_COUNTDOWN-1; ++i) {  // TODO: Check counter
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::DISARMED);
    }

    // Set throttle high and ensure it stays in DISARMED
    inputs.throttle = 1.0f;
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::DISARMED);

    // Check that the state machine transitions to ARMED when throttle is low
    inputs.throttle = 0.0f;
    for (uint32_t i = 0; i < config::tasks::THROTTLE_LOW_COUNTDOWN-1; ++i) {
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::DISARMED);
    }

    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::ARMED);
}


// Test that the state machine transitions from DISARMED to FAILSAFE when RC fails for enough cycles
TEST(SystemStateMachineTest, DisarmedToFailsafeAfterRcFailure)
{
    SystemStateMachine fsm;

    SystemInputs inputs{};
    inputs.imu_ok   = true;
    inputs.rc_ok    = true;

    // Get to DISARMED
    for (uint32_t i = 0; i < config::tasks::INIT_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }

    ASSERT_EQ(fsm.getState(), SystemState::DISARMED);

    // Check that the state machine transitions to FAILSAFE when RC fails for enough cycles, even if IMU is ok
    inputs.rc_ok = false;

    for (uint32_t i = 0; i < config::tasks::RC_COUNTDOWN - 1; ++i) {
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::DISARMED);
    }

    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::FAILSAFE);
}


// Test that the state machine transitions from DISARMED to ERROR when IMU fails for enough cycles
TEST(SystemStateMachineTest, DisarmedToErrorAfterImuFailure)
{
    SystemStateMachine fsm;

    SystemInputs inputs{};
    inputs.imu_ok   = true;
    inputs.rc_ok    = true;

    // Get to DISARMED
    for (uint32_t i = 0; i < config::tasks::INIT_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }

    ASSERT_EQ(fsm.getState(), SystemState::DISARMED);

    // Check that the state machine transitions to ERROR when IMU fails for enough cycles, even if RC is ok
    inputs.imu_ok = false;

    for (uint32_t i = 0; i < config::tasks::IMU_COUNTDOWN - 1; ++i) {
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::DISARMED);
    }

    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::ERROR);
}


// Check that the state machine transitions to ERROR when IMU fails for enough cycles, even if RC is ok
TEST(SystemStateMachineTest, DisarmedErrorHasPriority)
{
    SystemStateMachine fsm;

    SystemInputs inputs{};
    inputs.imu_ok   = true;
    inputs.rc_ok    = true;

    // Get to DISARMED
    for (uint32_t i = 0; i < config::tasks::INIT_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }

    ASSERT_EQ(fsm.getState(), SystemState::DISARMED);

    // Both IMU and RC fail simultaneously
    inputs.imu_ok   = false;
    inputs.rc_ok    = false;

    for (uint32_t i = 0; i < config::tasks::IMU_COUNTDOWN - 1; ++i) {
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::DISARMED);
    }

    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::ERROR);
}


// Check that the state machine transitions to FAILSAFE when RC fails for enough cycles, even if IMU is ok
TEST(SystemStateMachineTest, DisarmedFailsafeHasPriority)
{
    SystemStateMachine fsm;

    SystemInputs inputs{};
    inputs.imu_ok   = true;
    inputs.rc_ok = true;

    // Reach DISARMED
    for (uint32_t i = 0; i < config::tasks::INIT_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::DISARMED);

    // Check that the state machine transitions to FAILSAFE when RC fails for enough cycles, even if throttle is low
    inputs.rc_ok = false;
    inputs.throttle = 0.0f;

    for (uint32_t i = 0; i < config::tasks::RC_COUNTDOWN - 1; ++i) {
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::DISARMED);
    }

    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::FAILSAFE);
}


// ##################################################
// ############### ARMED TESTS ######################
// ##################################################

// Test that the state machine transitions from ARMED to FLIGHT when throttle is increased
TEST(SystemStateMachineTest, ArmedToFlight)
{
    SystemStateMachine fsm;

    SystemInputs inputs{};
    inputs.imu_ok   = true;
    inputs.rc_ok    = true;

    // Get to DISARMED
    for (uint32_t i = 0; i < config::tasks::INIT_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::DISARMED);

    // Get to ARMED
    inputs.throttle = 0.0f;
    for (uint32_t i = 0; i < config::tasks::THROTTLE_LOW_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::ARMED);

    // Check that the state machine transitions to FLIGHT when throttle is increased
    inputs.throttle = 1.0f;
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::FLIGHT);
}


// Test that the state machine transitions from ARMED to FAILSAFE when RC fails for enough cycles
TEST(SystemStateMachineTest, ArmedToFailsafeAfterRcFailure)
{
    SystemStateMachine fsm;

    SystemInputs inputs{};
    inputs.imu_ok   = true;
    inputs.rc_ok    = true;

    // Get to DISARMED
    for (uint32_t i = 0; i < config::tasks::INIT_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::DISARMED);

    // Get to ARMED
    inputs.throttle = 0.0f;
    for (uint32_t i = 0; i < config::tasks::THROTTLE_LOW_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::ARMED);

    // Check that the state machine transitions to FAILSAFE when RC fails for enough cycles, even if IMU is ok
    inputs.rc_ok = false;

    for (uint32_t i = 0; i < config::tasks::RC_COUNTDOWN - 1; ++i) {
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::ARMED);
    }

    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::FAILSAFE);
}


// Test that the state machine transitions from ARMED to ERROR when IMU fails for enough cycles
TEST(SystemStateMachineTest, ArmedToErrorAfterImuFailure)
{
    SystemStateMachine fsm;

    SystemInputs inputs{};
    inputs.imu_ok   = true;
    inputs.rc_ok    = true;

    // Get to DISARMED
    for (uint32_t i = 0; i < config::tasks::INIT_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::DISARMED);

    // Get to ARMED
    inputs.throttle = 0.0f;
    for (uint32_t i = 0; i < config::tasks::THROTTLE_LOW_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::ARMED);

    // Check that the state machine transitions to ERROR when IMU fails for enough cycles, even if RC is ok
    inputs.imu_ok = false;

    for (uint32_t i = 0; i < config::tasks::IMU_COUNTDOWN - 1; ++i) {
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), SystemState::ARMED);
    }

    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::ERROR);
}


// Test that the state machine transitions from ARMED to DISARMED when throttle is low
TEST(SystemStateMachineTest, ArmedToDisarmedWhenThrottleLow)
{
    SystemStateMachine fsm;

    SystemInputs inputs{};
    inputs.imu_ok   = true;
    inputs.rc_ok    = true;

    // Get to DISARMED
    for (uint32_t i = 0; i < config::tasks::INIT_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::DISARMED);

    // Get to ARMED
    inputs.throttle = 0.0f;
    for (uint32_t i = 0; i < config::tasks::THROTTLE_LOW_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::ARMED);

    // Check that the state machine transitions to FAILSAFE when RC fails
    inputs.rc_ok = false;
    for (uint32_t i = 0; i < config::tasks::RC_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::FAILSAFE);

    // Check that the state machine transitions to DISARMED when RC recovers and throttle is low
    inputs.rc_ok = true;
    inputs.throttle = 0.0f;
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::DISARMED);
}


// ##################################################
// ############### FLIGHT TESTS #####################
// ##################################################

// FLIGHT // TODO: Implement
//  ├── FlightRemainsWithShortLowThrottle
//  ├── FlightToArmedAfterLowThrottle
//  ├── FlightLowThrottleRequiresConsecutiveCycles
//  ├── FlightToFailsafeAfterRcFailure
//  └── FlightToErrorAfterImuFailure


// ##################################################
// ############### FAILSAFE TESTS ###################
// ##################################################

// Test that the state machine transitions from FAILSAFE to DISARMED when both RC and IMU recover
TEST(SystemStateMachineTest, FailsafeToDisarmedWhenInputsRecover)
{
    SystemStateMachine fsm;

    SystemInputs inputs{};
    inputs.imu_ok   = true;
    inputs.rc_ok    = true;
    inputs.throttle = 0.0f;

    // Get to DISARMED
    for (uint32_t i = 0; i < config::tasks::INIT_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::DISARMED);

    // Get to FAILSAFE
    inputs.rc_ok = false;
    for (uint32_t i = 0; i < config::tasks::RC_COUNTDOWN; ++i) {
        fsm.update_state(inputs);
    }
    ASSERT_EQ(fsm.getState(), SystemState::FAILSAFE);

    // Check that the state machine remains in FAILSAFE when only one of the inputs recovers
    inputs.rc_ok = true;
    inputs.imu_ok = false;
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::FAILSAFE);

    inputs.rc_ok = false;
    inputs.imu_ok = true;
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::FAILSAFE);

    // Check that the state machine transitions to DISARMED when both RC and IMU recover
    inputs.rc_ok = true;
    inputs.imu_ok = true;
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::DISARMED);
}