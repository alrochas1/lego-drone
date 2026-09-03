// rate_controller.cpp

#include "rate_controller.hpp"

#include <algorithm>
#include <cmath>

ControlOutput RateController::update(
    const IMUData&     imu,
    const RateCommand& rate_command,
    const float        throttle,
    const float        dt)
{
    ControlOutput control{};

    static RateCommand integral{};

    const float error_roll =
        rate_command.roll - imu.gyro.angular_velocity.x;

    const float error_pitch =
        rate_command.pitch - imu.gyro.angular_velocity.y;

    const float error_yaw =
        rate_command.yaw - imu.gyro.angular_velocity.z;

    integral.roll   += error_roll   * dt;
    integral.pitch  += error_pitch  * dt;
    integral.yaw    += error_yaw    * dt;

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

    control.throttle = std::clamp(throttle, 0.0f, 1.0f);

    return control;
}