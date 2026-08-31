// motor_mixer.cpp
#include "motor_mixer.hpp"
#include <algorithm>


MotorCommands MotorMixer::mix_motors(
    const RCCommand& control)
{
    MotorCommands commands{};
    float m[4]{};

    auto throttle = control.throttle;

    if (throttle <= throttle_min_) {
        return commands;
    }

    m[0] =
        throttle +
        control.roll +
        control.pitch -
        control.yaw;

    m[1] =
        throttle -
        control.roll +
        control.pitch +
        control.yaw;

    m[2] =
        throttle +
        control.roll -
        control.pitch +
        control.yaw;

    m[3] =
        throttle -
        control.roll -
        control.pitch -
        control.yaw;

    m[0] = std::clamp(m[0], 0.0f, 1.0f);
    m[1] = std::clamp(m[1], 0.0f, 1.0f);
    m[2] = std::clamp(m[2], 0.0f, 1.0f);
    m[3] = std::clamp(m[3], 0.0f, 1.0f);

    for (int i = 0; i < 4; ++i) {
        commands.motor[i] = static_cast<uint16_t>(m[i] * 1023.0f);
    }

    return commands;
}