// flight_controller.cpp
#include "types/sensor_data.hpp"
#include "types/comms_data.hpp"
#include "types/system_data.hpp"

#include "flight_controller.hpp"

// FIXME: Move
#include <algorithm>
#include <cmath>

namespace {
constexpr float GRAVITY = 9.80665f;
constexpr float COMPLEMENTARY_ALPHA = 0.98f;
constexpr float TWO_PI = 6.28318530718f;
}

// Update the flight controller with the latest system snapshot
// FIXME: Separate the estimator and the controller into different classes
RCCommand FlightController::update(const IMUData& imu, const RCCommand& rc)
{
    // Update the time delta based on the latest IMU timestamp
    update_dt(imu.gyro.timestamp_ms); // FIXME
    
    // Estimate the current state based on sensor data
    const Inclination current_state = estimate_state(imu);

    // Compute the desired rate based on the current state and desired RC command
    const RCCommand desired_rate = attitude_controller(current_state, rc);

    // Compute the control command based on the desired rate and sensor data
    const RCCommand control = rate_controller(desired_rate, imu);

    return control;
}


// Mix motor commands based on control inputs and throttle
float FlightController::update_dt(uint32_t timestamp)
{
    if (previous_timestamp_ != 0 &&
        timestamp > previous_timestamp_) {

        dt_ =
            static_cast<float>(
                timestamp - previous_timestamp_) *
            1e-6f;
    }

    previous_timestamp_ = timestamp;

    dt_ = std::clamp(dt_, 0.0005f, 0.01f);

    return dt_;
}


// Estimate the drone's attitude based on sensor data
Inclination FlightController::estimate_state(const IMUData& imu)
{
    const float ax = imu.accel.linear_acceleration.x;
    const float ay = imu.accel.linear_acceleration.y;
    const float az = imu.accel.linear_acceleration.z;

    const float gx = imu.gyro.angular_velocity.x;
    const float gy = imu.gyro.angular_velocity.y;
    const float gz = imu.gyro.angular_velocity.z;

    previous_timestamp_ = imu.gyro.timestamp_ms; // FIXME

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
            (attitude_.roll + gx * dt_) +
            (1.0f - COMPLEMENTARY_ALPHA) *
            roll_acc;

        attitude_.pitch =
            COMPLEMENTARY_ALPHA *
            (attitude_.pitch + gy * dt_) +
            (1.0f - COMPLEMENTARY_ALPHA) *
            pitch_acc;

    } else {
        attitude_.roll += gx * dt_;
        attitude_.pitch += gy * dt_;
    }

    attitude_.yaw += gz * dt_;

    if (attitude_.yaw > TWO_PI)
        attitude_.yaw -= TWO_PI;

    if (attitude_.yaw < -TWO_PI)
        attitude_.yaw += TWO_PI;

    return attitude_;
}


// Attitude controller
RCCommand FlightController::attitude_controller(
    const Inclination& current_state,
    const RCCommand& desired_state)
{
    RCCommand desired_rate{};

    constexpr float MAX_ANGLE =
        30.0f * 3.14159265359f / 180.0f;

    constexpr float MAX_YAW_RATE =
        180.0f * 3.14159265359f / 180.0f;

    constexpr float ATTITUDE_KP = 4.0f;

    const float desired_roll =
        desired_state.roll * MAX_ANGLE;

    const float desired_pitch =
        desired_state.pitch * MAX_ANGLE;

    desired_rate.roll =
        ATTITUDE_KP *
        (desired_roll - current_state.roll);

    desired_rate.pitch =
        ATTITUDE_KP *
        (desired_pitch - current_state.pitch);

    desired_rate.yaw =
        desired_state.yaw * MAX_YAW_RATE;

    desired_rate.roll =
        std::clamp(desired_rate.roll,
                   -MAX_YAW_RATE,
                   MAX_YAW_RATE);

    desired_rate.pitch =
        std::clamp(desired_rate.pitch,
                   -MAX_YAW_RATE,
                   MAX_YAW_RATE);

    desired_rate.throttle = desired_state.throttle;

    return desired_rate;
}


// Rate controller
RCCommand FlightController::rate_controller(
    const RCCommand& desired_rate,
    const IMUData& imu)
{
    RCCommand control{};

    constexpr float RATE_KP = 0.08f;
    constexpr float RATE_KI = 0.02f;

    constexpr float INTEGRAL_LIMIT = 1.0f;
    constexpr float OUTPUT_LIMIT = 1.0f;

    static RCCommand integral{};

    const float error_roll =
        desired_rate.roll - imu.gyro.angular_velocity.x;

    const float error_pitch =
        desired_rate.pitch - imu.gyro.angular_velocity.y;

    const float error_yaw =
        desired_rate.yaw - imu.gyro.angular_velocity.z;

    integral.roll   += error_roll   * dt_;
    integral.pitch  += error_pitch  * dt_;
    integral.yaw    += error_yaw    * dt_;

    integral.roll =
        std::clamp(integral.roll,
                   -INTEGRAL_LIMIT,
                   INTEGRAL_LIMIT);

    integral.pitch =
        std::clamp(integral.pitch,
                   -INTEGRAL_LIMIT,
                   INTEGRAL_LIMIT);

    integral.yaw =
        std::clamp(integral.yaw,
                   -INTEGRAL_LIMIT,
                   INTEGRAL_LIMIT);

    control.roll =
        RATE_KP * error_roll +
        RATE_KI * integral.roll;

    control.pitch =
        RATE_KP * error_pitch +
        RATE_KI * integral.pitch;

    control.yaw =
        RATE_KP * error_yaw +
        RATE_KI * integral.yaw;

    control.roll =
        std::clamp(control.roll,
                   -OUTPUT_LIMIT,
                   OUTPUT_LIMIT);

    control.pitch =
        std::clamp(control.pitch,
                   -OUTPUT_LIMIT,
                   OUTPUT_LIMIT);

    control.yaw =
        std::clamp(control.yaw,
                   -OUTPUT_LIMIT,
                   OUTPUT_LIMIT);

    control.throttle = desired_rate.throttle;

    return control;
}
