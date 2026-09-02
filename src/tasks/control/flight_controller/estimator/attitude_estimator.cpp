// attitude_estimator.cpp

#include "attitude_estimator.hpp"


Attitude AttitudeEstimator::update(const IMUData& imu, const float dt)
{
    const float ax = imu.accel.linear_acceleration.x;
    const float ay = imu.accel.linear_acceleration.y;
    const float az = imu.accel.linear_acceleration.z;

    const float gx = imu.gyro.angular_velocity.x;
    const float gy = imu.gyro.angular_velocity.y;
    const float gz = imu.gyro.angular_velocity.z;

    const float accel_norm =
        std::sqrt(ax * ax + ay * ay + az * az);

    if (accel_norm > 0.7f * GRAVITY &&
        accel_norm < 1.3f * GRAVITY) {

        const float roll_acc =
            std::atan2(ay, az);

        const float pitch_acc =
            std::atan2(
                -ax,
                std::sqrt(ay * ay + az * az));

        attitude_.roll =
            COMPLEMENTARY_ALPHA *
            (attitude_.roll + gx * dt) +
            (1.0f - COMPLEMENTARY_ALPHA) *
            roll_acc;

        attitude_.pitch =
            COMPLEMENTARY_ALPHA *
            (attitude_.pitch + gy * dt) +
            (1.0f - COMPLEMENTARY_ALPHA) *
            pitch_acc;

    } else {
        attitude_.roll += gx * dt;
        attitude_.pitch += gy * dt;
    }

    attitude_.yaw += gz * dt;

    if (attitude_.yaw > TWO_PI)
        attitude_.yaw -= TWO_PI;

    if (attitude_.yaw < -TWO_PI)
        attitude_.yaw += TWO_PI;

    return attitude_;
}