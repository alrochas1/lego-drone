#include <gtest/gtest.h>

#include "config/project_config.hpp"
#include "types/system_data.hpp"

#include "flight_controller.hpp"
#include "motor_mixer.hpp"

const float GRAVITY = GRAVITY;

// Test that the flight controller produces zero control outputs when throttle is zero
TEST(ControlTest, ZeroThrottleProducesZeroControl)
{
    FlightController   controller{};
    MotorMixer         mixer{};

    IMUData     imu{};
    RCCommand   rc{};

    uint32_t timestamp_ms = 0;

    // Check that zero throttle and IMU produces zero control outputs
    auto control = controller.update(imu, rc);
    auto motor_commands = mixer.mix_motors(control);

    EXPECT_FLOAT_EQ(motor_commands.motor[0], 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.motor[1], 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.motor[2], 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.motor[3], 0.0f);

    // Check that a non-zero IMU reading produces zero control outputs with throttle at zero
    imu.gyro.angular_velocity.x = 0.1f;
    imu.gyro.angular_velocity.y = 0.2f;
    imu.gyro.angular_velocity.z = 0.3f;

    imu.accel.linear_acceleration.x = 0.1f;
    imu.accel.linear_acceleration.y = 0.2f;
    imu.accel.linear_acceleration.z = GRAVITY;

    imu.gyro.timestamp_ms += 10; // FIX this

    control = controller.update(imu, rc);
    motor_commands = mixer.mix_motors(control);

    EXPECT_FLOAT_EQ(motor_commands.motor[0], 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.motor[1], 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.motor[2], 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.motor[3], 0.0f);

    // Check that a non-zero RC input produces zero control outputs with throttle at zero
    rc.throttle =  0.0f;
    rc.roll     =  0.5f;
    rc.pitch    = -0.5f;
    rc.yaw      =  0.2f;

    imu.gyro.timestamp_ms += 10; // FIX this

    control = controller.update(imu, rc);
    motor_commands = mixer.mix_motors(control);

    EXPECT_FLOAT_EQ(motor_commands.motor[0], 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.motor[1], 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.motor[2], 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.motor[3], 0.0f);
}


// Test that the flight controller produces non-zero control outputs when throttle is non-zero
TEST(ControlTest, NonZeroThrottleProducesNonZeroControl)
{
    FlightController   controller{};
    MotorMixer         mixer{};

    IMUData     imu{};
    RCCommand   rc{};

    uint32_t timestamp_ms = 0;

    // Set a non-zero throttle and check that control outputs are non-zero
    rc.throttle = 0.5f;

    imu.gyro.angular_velocity.x = 0.1f;
    imu.gyro.angular_velocity.y = 0.2f;
    imu.gyro.angular_velocity.z = 0.3f;

    imu.accel.linear_acceleration.x = 0.1f;
    imu.accel.linear_acceleration.y = 0.2f;
    imu.accel.linear_acceleration.z = GRAVITY;

    RCCommand       control{};
    MotorCommands   motor_commands{};

    // Simulate multiple updates to allow the controller to stabilize
    for (int i = 0; i < 10; ++i) {      // Improve this
        imu.gyro.timestamp_ms += 10;    // FIX this

        control = controller.update(imu, rc);
        motor_commands = mixer.mix_motors(control);
    }

    EXPECT_NE(control.throttle, 0.0f);
    EXPECT_NE(control.roll,     0.0f);
    EXPECT_NE(control.pitch,    0.0f);
    EXPECT_NE(control.yaw,      0.0f);

    EXPECT_GT(motor_commands.motor[0], 0.0f);
    EXPECT_GT(motor_commands.motor[1], 0.0f);
    EXPECT_GT(motor_commands.motor[2], 0.0f);
    EXPECT_GT(motor_commands.motor[3], 0.0f);
}


// Test that the flight controller estimates roll inclination correctly based on IMU data
TEST(ControlTest, RollInputProducesRollMotorCommands)
{
    FlightController   controller{};
    MotorMixer         mixer{};

    IMUData     imu{};
    RCCommand   rc{};

    rc.throttle = 0.1f;
    rc.roll     = 0.5f;

    imu.accel.linear_acceleration.x = 0.0f;
    imu.accel.linear_acceleration.y = 0.0f;
    imu.accel.linear_acceleration.z = GRAVITY;

    RCCommand control    = controller.update(imu, rc);
    MotorCommands motors = mixer.mix_motors(control);

    // Check that the roll control output is positive when the RC roll input is positive
    EXPECT_GT(control.roll, 0.0f);

    // Check the expected motor relationship.
    EXPECT_GT(motors.motor[0], motors.motor[1]);
    EXPECT_GT(motors.motor[2], motors.motor[3]);
}


// Test that the flight controller estimates pitch inclination correctly based on IMU data
TEST(ControlTest, PitchInputProducesPitchMotorCommands)
{
    FlightController   controller{};
    MotorMixer         mixer{};

    IMUData     imu{};
    RCCommand   rc{};

    rc.throttle = 0.1f;
    rc.pitch    = -0.5f;

    imu.accel.linear_acceleration.x = 0.0f;
    imu.accel.linear_acceleration.y = 0.0f;
    imu.accel.linear_acceleration.z = GRAVITY;

    RCCommand control    = controller.update(imu, rc);
    MotorCommands motors = mixer.mix_motors(control);

    // Check that the pitch control output is negative when the RC pitch input is negative
    EXPECT_LT(control.pitch, 0.0f);

    // Check the expected motor relationship.
    EXPECT_GT(motors.motor[2], motors.motor[0]);
    EXPECT_GT(motors.motor[3], motors.motor[1]);
}


// Test that the flight controller estimates yaw inclination correctly based on IMU data
TEST(ControlTest, YawInputProducesYawMotorCommands)
{
    FlightController   controller{};
    MotorMixer         mixer{};

    IMUData     imu{};
    RCCommand   rc{};

    rc.throttle = 0.1f;
    rc.yaw      = 0.5f;

    imu.gyro.angular_velocity.x = 0.0f;
    imu.gyro.angular_velocity.y = 0.0f;
    imu.gyro.angular_velocity.z = 0.1f;

    RCCommand control    = controller.update(imu, rc);
    MotorCommands motors = mixer.mix_motors(control);

    // Check that the yaw control output is positive when the RC yaw input is positive
    EXPECT_GT(control.yaw, 0.0f);

    // Check the expected motor relationship.
    EXPECT_GT(motors.motor[1], motors.motor[0]);
    EXPECT_GT(motors.motor[2], motors.motor[3]);
}


// Test that a positive roll input produces a correction in the expected direction
// FIXME: I need a better simulator for this
// TEST(ControlTest, RollPerturbationProducesRollCorrection)
// {
//     FlightController controller{};
//     MotorMixer mixer{};

//     IMUData  imu{};
//     RCCommand rc{};

//     const float ROLL = 30 * M_PI / 180; // rad

//     rc.throttle = 0.2f;

//     imu.accel.linear_acceleration.x = GRAVITY * std::sin(ROLL); // Simulate a roll perturbation
//     imu.accel.linear_acceleration.y = 0.0f;
//     imu.accel.linear_acceleration.z = GRAVITY * std::cos(ROLL); // Simulate a roll perturbation

//     imu.gyro.timestamp_ms += 10; // FIX this

//     // Simulate multiple updates to allow the controller to stabilize
//     RCCommand control    = controller.update(imu, rc);
//     MotorCommands motors = mixer.mix_motors(control);

//     // Expect a positive roll correction
//     EXPECT_GT(control.roll, 0.0f);

//     // Check the expected motor relationship.
//     EXPECT_GT(motors.motor[0], motors.motor[1]);
//     EXPECT_GT(motors.motor[2], motors.motor[3]);
// }