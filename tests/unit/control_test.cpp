#include <gtest/gtest.h>

#include "config/project_config.hpp"
#include "types/system_data.hpp"

#include "flight_controller.hpp"
#include "motor_mixer.hpp"

TEST(ControlTest, ZeroThrottleProducesZeroControl)
{
    FlightController   controller{};
    MotorMixer         mixer{};

    IMUData imu{};
    RCCommand  rc{};

    uint32_t timestamp_ms = 0;

    auto control = controller.update(imu, rc);
    auto motor_commands = mixer.mix_motors(control);

    EXPECT_FLOAT_EQ(motor_commands.m1, 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.m2, 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.m3, 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.m4, 0.0f);

    // Check that a non-zero IMU reading produces zero control outputs with throttle at zero
    imu.gyro.angular_velocity.x = 0.1f;
    imu.gyro.angular_velocity.y = 0.2f;
    imu.gyro.angular_velocity.z = 0.3f;

    imu.accel.linear_acceleration.x = 0.1f;
    imu.accel.linear_acceleration.y = 0.2f;
    imu.accel.linear_acceleration.z = 9.81f;

    imu.gyro.timestamp_ms += 10; // FIX this

    control = controller.update(imu, rc);
    motor_commands = mixer.mix_motors(control);

    EXPECT_FLOAT_EQ(motor_commands.m1, 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.m2, 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.m3, 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.m4, 0.0f);

    // Check that a non-zero RC input produces zero control outputs with throttle at zero
    rc.throttle = 0.0f;
    rc.roll = 0.5f;
    rc.pitch = -0.5f;
    rc.yaw = 0.2f;

    imu.gyro.timestamp_ms += 10; // FIX this

    control = controller.update(imu, rc);
    motor_commands = mixer.mix_motors(control);

    EXPECT_FLOAT_EQ(motor_commands.m1, 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.m2, 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.m3, 0.0f);
    EXPECT_FLOAT_EQ(motor_commands.m4, 0.0f);
}