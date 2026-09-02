// attitude_controller.cpp

#include "attitude_controller.hpp"

#include <algorithm>
#include <cmath>

RateCommand AttitudeController::update(
    const Attitude&  current_state,
    const RCCommand& desired_state)
{
    RateCommand desired_rate{};

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

    return desired_rate;
}