#include <gtest/gtest.h>

#include "config/project_config.hpp"
#include "types/system_data.hpp"

#include "flight_controller.hpp"
#include "motor_mixer.hpp"


// Helper functions for testing the FlightController and MotorMixer
namespace {

constexpr float TEST_GRAVITY = 9.80665f;
constexpr uint32_t TEST_TIMESTEP_MS = 10;
constexpr int SETTLE_UPDATES = 10;

struct ControlTestRig {
    FlightController controller{};
    MotorMixer mixer{};
    IMUData imu{};
    RCCommand rc{};

    ControlOutput update()
    {
        return controller.update(imu, rc);
    }

    MotorCommands update_and_mix()
    {
        return mixer.mix_motors(update());
    }

    void advance_time(uint32_t milliseconds = TEST_TIMESTEP_MS)
    {
        imu.gyro.timestamp_ms += milliseconds;
    }

    MotorCommands run_settled(ControlOutput& control)
    {
        MotorCommands motors{};
        for (int update_number = 0; update_number < SETTLE_UPDATES; ++update_number) {
            advance_time();
            control = update();
            motors = mixer.mix_motors(control);
        }
        return motors;
    }
};

void set_level_imu(ControlTestRig& rig)
{
    rig.imu.accel.linear_acceleration = Vector3f{0.0f, 0.0f, -TEST_GRAVITY};
}

void expect_all_motors_zero(const MotorCommands& motors)
{
    for (uint8_t motor = 0; motor < N_MOTORS; ++motor) {
        EXPECT_FLOAT_EQ(motors.motor[motor], 0.0f);
    }
}

void expect_all_motors_equal(const MotorCommands& motors)
{
    for (uint8_t motor = 1; motor < N_MOTORS; ++motor) {
        EXPECT_FLOAT_EQ(motors.motor[0], motors.motor[motor]);
    }
}

void expect_all_motors_positive(const MotorCommands& motors)
{
    for (uint8_t motor = 0; motor < N_MOTORS; ++motor) {
        EXPECT_GT(motors.motor[motor], 0.0f);
    }
}

void expect_roll_response(const ControlOutput& control, const MotorCommands& motors)
{
    EXPECT_GT(control.roll, 0.0f);
    EXPECT_GT(motors.motor[0], motors.motor[1]);
    EXPECT_GT(motors.motor[2], motors.motor[3]);
}

void expect_pitch_response(const ControlOutput& control, const MotorCommands& motors)
{
    EXPECT_LT(control.pitch, 0.0f);
    EXPECT_GT(motors.motor[2], motors.motor[0]);
    EXPECT_GT(motors.motor[3], motors.motor[1]);
}

void expect_yaw_response(const ControlOutput& control, const MotorCommands& motors)
{
    EXPECT_GT(control.yaw, 0.0f);
    EXPECT_GT(motors.motor[1], motors.motor[0]);
    EXPECT_GT(motors.motor[2], motors.motor[3]);
}


} // namespace


// ##################################################
// ################# CONTROL TESTS ##################
// ##################################################

// Test that the flight controller produces zero control outputs when throttle is zero
TEST(ControlTest, ZeroThrottleProducesZeroControl)
{
    ControlTestRig rig{};

    // Set up the test rig with zero throttle and level IMU data
    expect_all_motors_zero(rig.update_and_mix());

    // Set up the test rig with zero throttle but non-zero IMU data
    rig.imu.gyro.angular_velocity = Vector3f{0.1f, 0.2f, 0.3f};
    set_level_imu(rig);
    rig.advance_time();
    expect_all_motors_zero(rig.update_and_mix());

    rig.rc.roll = 0.5f;
    rig.rc.pitch = -0.5f;
    rig.rc.yaw = 0.2f;
    rig.advance_time();
    expect_all_motors_zero(rig.update_and_mix());
}


// Test that the flight controller produces non-zero control outputs when throttle is non-zero
TEST(ControlTest, NonZeroThrottleProducesNonZeroControl)
{
    ControlTestRig rig{};
    rig.rc.throttle = 0.5f;
    rig.imu.gyro.angular_velocity = Vector3f{0.1f, 0.2f, 0.3f};
    set_level_imu(rig);

    ControlOutput control{};
    const MotorCommands motors = rig.run_settled(control);

    EXPECT_NE(control.throttle, 0.0f);
    EXPECT_NE(control.roll, 0.0f);
    EXPECT_NE(control.pitch, 0.0f);
    EXPECT_NE(control.yaw, 0.0f);
    expect_all_motors_positive(motors);
}


// Test that the flight controller produces equal motor commands when throttle is non-zero but IMU data is zero
TEST(ControlTest, ZeroIMUProducesEqualMotorCommands)
{
    ControlTestRig rig{};
    rig.rc.throttle = 0.5f;
    set_level_imu(rig);

    ControlOutput control{};
    const MotorCommands motors = rig.run_settled(control);

    EXPECT_NE(control.throttle, 0.0f);
    expect_all_motors_equal(motors);

    const float expected_motor_command = MOTOR_MAX * rig.rc.throttle;
    for (uint8_t motor = 0; motor < N_MOTORS; ++motor) {
        EXPECT_NEAR(motors.motor[motor], expected_motor_command, 1.0f);
    }
}


// Test that the flight controller preserves throttle when mixing motor commands
TEST(ControlTest, ThrottleIsPreserved)
{
    ControlTestRig rig{};
    rig.rc.throttle = 0.5f;
    set_level_imu(rig);

    const float expected = rig.rc.throttle * MOTOR_MAX;
    for (int update_number = 0; update_number < SETTLE_UPDATES; ++update_number) {
        const MotorCommands motors = rig.update_and_mix();
        const float mean = (motors.motor[0] + motors.motor[1] + motors.motor[2] + motors.motor[3]) / 4.0f;

        EXPECT_NEAR(mean, expected, 2.0f);
        rig.advance_time();
    }
}


// Test that the flight controller produces roll responses when corresponding RC commands are given
TEST(ControlTest, RollCommandProducesRollMotorCommands)
{
    ControlTestRig rig{};
    rig.rc.throttle = 0.2f;
    rig.rc.roll = 0.5f;
    set_level_imu(rig);

    const ControlOutput control = rig.update();
    const MotorCommands motors = rig.mixer.mix_motors(control);

    expect_roll_response(control, motors);
}


// Test that the flight controller produces pitch responses when corresponding RC commands are given
TEST(ControlTest, PitchCommandProducesPitchMotorCommands)
{
    ControlTestRig rig{};
    rig.rc.throttle = 0.2f;
    rig.rc.pitch = -0.5f;
    set_level_imu(rig);

    const ControlOutput control = rig.update();
    const MotorCommands motors = rig.mixer.mix_motors(control);

    expect_pitch_response(control, motors);
}


// Test that the flight controller produces yaw responses when corresponding RC commands are given
TEST(ControlTest, YawCommandProducesYawMotorCommands)
{
    ControlTestRig rig{};
    rig.rc.throttle = 0.2f;
    rig.rc.yaw = 0.5f;
    set_level_imu(rig);

    const ControlOutput control = rig.update();
    const MotorCommands motors = rig.mixer.mix_motors(control);

    expect_yaw_response(control, motors);
}
