#include <gtest/gtest.h>

#include "drone_project/config/project_config.hpp"
#include "system_state_machine.hpp"


// Helper functions for testing the SystemStateMachine
namespace {

// Returns a SystemInputs object with all inputs set to healthy values
SystemInputs healthyInputs()
{
    SystemInputs inputs{};
    inputs.imu_ok   = true;
    inputs.rc_ok = true;
    return inputs;
}

// Expects the state machine to remain in the expected state for a given number of cycles
void expectStateForCycles(
    SystemStateMachine& fsm,
    const SystemInputs& inputs,
    uint32_t cycles,
    SystemState expectedState)
{
    for (uint32_t cycle = 0; cycle < cycles; ++cycle) {
        fsm.update_state(inputs);
        EXPECT_EQ(fsm.getState(), expectedState);
    }
}

// Expects the state machine to transition from one state to another after a given number of cycles
void expectTimedTransition(
    SystemStateMachine& fsm,
    const SystemInputs& inputs,
    uint32_t countdown,
    SystemState stateBeforeTransition,
    SystemState stateAfterTransition)
{
    expectStateForCycles(fsm, inputs, countdown - 1, stateBeforeTransition);
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), stateAfterTransition);
}

// Helper function to reach the DISARMED state from INIT
void reachDisarmed(SystemStateMachine& fsm, SystemInputs& inputs)
{
    expectStateForCycles(
        fsm, inputs, config::tasks::INIT_COUNTDOWN - 1, SystemState::INIT);
    fsm.update_state(inputs);
    ASSERT_EQ(fsm.getState(), SystemState::DISARMED);
}

// Helper function to reach the ARMED state from DISARMED
void reachArmed(SystemStateMachine& fsm, SystemInputs& inputs)
{
    reachDisarmed(fsm, inputs);
    inputs.throttle = 0.0f;
    expectStateForCycles(
        fsm,
        inputs,
        config::tasks::THROTTLE_LOW_COUNTDOWN - 1,
        SystemState::DISARMED);
    fsm.update_state(inputs);
    ASSERT_EQ(fsm.getState(), SystemState::ARMED);
}

} // namespace


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

    SystemInputs inputs{};
    inputs.imu_ok = true;

    expectTimedTransition(
        fsm,
        inputs,
        config::tasks::INIT_COUNTDOWN,
        SystemState::INIT,
        SystemState::DISARMED);
}


// Test that the state machine transitions from INIT to ERROR
TEST(SystemStateMachineTest, InitToError)
{
    SystemStateMachine fsm;

    // Check that the state machine starts in INIT state
    EXPECT_EQ(fsm.getState(), SystemState::INIT);

    SystemInputs inputs{};
    expectTimedTransition(
        fsm,
        inputs,
        config::tasks::IMU_COUNTDOWN,
        SystemState::INIT,
        SystemState::ERROR);
}


// Test that the state machine transitions from INIT to ERROR only when IMU fails consecutively
TEST(SystemStateMachineTest, InitToErrorConsecutiveCycles)
{
    SystemStateMachine fsm;

    SystemInputs inputs{};
    inputs.imu_ok = false;

    expectStateForCycles(
        fsm, inputs, config::tasks::IMU_COUNTDOWN - 1, SystemState::INIT);

    // A good IMU reading should reset the counter and keep the state in INIT
    inputs.imu_ok = true;
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::INIT);

    // New errors should start counting again
    inputs.imu_ok = false;
    expectTimedTransition(
        fsm,
        inputs,
        config::tasks::IMU_COUNTDOWN,
        SystemState::INIT,
        SystemState::ERROR);
}


// ##################################################
// ################ DISARMED TESTS ##################
// ##################################################

// Test that the state machine transitions from DISARMED to ARMED when throttle is low
TEST(SystemStateMachineTest, DisarmedToArmed)
{
    SystemStateMachine fsm;
    SystemInputs inputs = healthyInputs();
    reachDisarmed(fsm, inputs);

    // Check that the state machine remains in DISARMED when RC is not ok
    inputs.rc_ok    = false;
    expectStateForCycles(
        fsm, inputs, config::tasks::RC_COUNTDOWN - 1, SystemState::DISARMED);

    // Check that the state machine remains in DISARMED until RC is ok
    inputs.rc_ok    = true;
    expectStateForCycles(
        fsm, inputs, config::tasks::RC_COUNTDOWN - 1, SystemState::DISARMED);

    // Set throttle high and ensure it stays in DISARMED
    inputs.throttle = 1.0f;
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::DISARMED);

    // Check that the state machine transitions to ARMED when throttle is low
    inputs.throttle = 0.0f;
    expectTimedTransition(
        fsm,
        inputs,
        config::tasks::THROTTLE_LOW_COUNTDOWN,
        SystemState::DISARMED,
        SystemState::ARMED);
}


// Test that the state machine transitions from DISARMED to FAILSAFE when RC fails for enough cycles
TEST(SystemStateMachineTest, DisarmedToFailsafeAfterRcFailure)
{
    SystemStateMachine fsm;
    SystemInputs inputs = healthyInputs();
    reachDisarmed(fsm, inputs);

    // Check that the state machine transitions to FAILSAFE when RC fails for enough cycles, even if IMU is ok
    inputs.rc_ok = false;

    expectTimedTransition(
        fsm,
        inputs,
        config::tasks::RC_COUNTDOWN,
        SystemState::DISARMED,
        SystemState::FAILSAFE);
}


// Test that the state machine transitions from DISARMED to ERROR when IMU fails for enough cycles
TEST(SystemStateMachineTest, DisarmedToErrorAfterImuFailure)
{
    SystemStateMachine fsm;
    SystemInputs inputs = healthyInputs();
    reachDisarmed(fsm, inputs);

    // Check that the state machine transitions to ERROR when IMU fails for enough cycles, even if RC is ok
    inputs.imu_ok = false;

    expectTimedTransition(
        fsm,
        inputs,
        config::tasks::IMU_COUNTDOWN,
        SystemState::DISARMED,
        SystemState::ERROR);
}


// Check that the state machine transitions to ERROR when IMU fails for enough cycles, even if RC is ok
TEST(SystemStateMachineTest, DisarmedErrorHasPriority)
{
    SystemStateMachine fsm;
    SystemInputs inputs = healthyInputs();
    reachDisarmed(fsm, inputs);

    // Both IMU and RC fail simultaneously
    inputs.imu_ok   = false;
    inputs.rc_ok    = false;

    expectTimedTransition(
        fsm,
        inputs,
        config::tasks::IMU_COUNTDOWN,
        SystemState::DISARMED,
        SystemState::ERROR);
}


// Check that the state machine transitions to FAILSAFE when RC fails for enough cycles, even if IMU is ok
TEST(SystemStateMachineTest, DisarmedFailsafeHasPriority)
{
    SystemStateMachine fsm;
    SystemInputs inputs = healthyInputs();

    reachDisarmed(fsm, inputs);

    // Check that the state machine transitions to FAILSAFE when RC fails for enough cycles, even if throttle is low
    inputs.rc_ok = false;
    inputs.throttle = 0.0f;

    expectTimedTransition(
        fsm,
        inputs,
        config::tasks::RC_COUNTDOWN,
        SystemState::DISARMED,
        SystemState::FAILSAFE);
}


// ##################################################
// ############### ARMED TESTS ######################
// ##################################################

// Test that the state machine transitions from ARMED to FLIGHT when throttle is increased
TEST(SystemStateMachineTest, ArmedToFlight)
{
    SystemStateMachine fsm;
    SystemInputs inputs = healthyInputs();
    reachArmed(fsm, inputs);

    // Check that the state machine transitions to FLIGHT when throttle is increased
    inputs.throttle = 1.0f;
    fsm.update_state(inputs);
    EXPECT_EQ(fsm.getState(), SystemState::FLIGHT);
}


// Test that the state machine transitions from ARMED to FAILSAFE when RC fails for enough cycles
TEST(SystemStateMachineTest, ArmedToFailsafeAfterRcFailure)
{
    SystemStateMachine fsm;
    SystemInputs inputs = healthyInputs();
    reachArmed(fsm, inputs);

    // Check that the state machine transitions to FAILSAFE when RC fails for enough cycles, even if IMU is ok
    inputs.rc_ok = false;

    expectTimedTransition(
        fsm,
        inputs,
        config::tasks::RC_COUNTDOWN,
        SystemState::ARMED,
        SystemState::FAILSAFE);
}


// Test that the state machine transitions from ARMED to ERROR when IMU fails for enough cycles
TEST(SystemStateMachineTest, ArmedToErrorAfterImuFailure)
{
    SystemStateMachine fsm;
    SystemInputs inputs = healthyInputs();
    reachArmed(fsm, inputs);

    // Check that the state machine transitions to ERROR when IMU fails for enough cycles, even if RC is ok
    inputs.imu_ok = false;

    expectTimedTransition(
        fsm,
        inputs,
        config::tasks::IMU_COUNTDOWN,
        SystemState::ARMED,
        SystemState::ERROR);
}


// Test that the state machine transitions from ARMED to DISARMED when throttle is low
TEST(SystemStateMachineTest, ArmedToDisarmedWhenThrottleLow)
{
    SystemStateMachine fsm;
    SystemInputs inputs = healthyInputs();
    reachArmed(fsm, inputs);

    // Check that the state machine transitions to FAILSAFE when RC fails
    inputs.rc_ok = false;
    expectTimedTransition(
        fsm,
        inputs,
        config::tasks::RC_COUNTDOWN,
        SystemState::ARMED,
        SystemState::FAILSAFE);

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
    SystemInputs inputs = healthyInputs();
    reachDisarmed(fsm, inputs);

    // Get to FAILSAFE
    inputs.rc_ok = false;
    expectTimedTransition(
        fsm,
        inputs,
        config::tasks::RC_COUNTDOWN,
        SystemState::DISARMED,
        SystemState::FAILSAFE);

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