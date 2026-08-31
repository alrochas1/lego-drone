// motor_mixer.cpp
#include "motor_mixer.hpp"
#include <algorithm>


MotorCommands MotorMixer::mix_motors(
    const RCCommand& control)
{
    MotorCommands commands{};

    auto throttle = control.throttle;

    float m1 =
        throttle +
        control.roll +
        control.pitch -
        control.yaw;

    float m2 =
        throttle -
        control.roll +
        control.pitch +
        control.yaw;

    float m3 =
        throttle +
        control.roll -
        control.pitch +
        control.yaw;

    float m4 =
        throttle -
        control.roll -
        control.pitch -
        control.yaw;

    m1 = std::clamp(m1, 0.0f, 1.0f);
    m2 = std::clamp(m2, 0.0f, 1.0f);
    m3 = std::clamp(m3, 0.0f, 1.0f);
    m4 = std::clamp(m4, 0.0f, 1.0f);

    commands.m1 =
        static_cast<uint16_t>(m1 * 1023.0f);

    commands.m2 =
        static_cast<uint16_t>(m2 * 1023.0f);

    commands.m3 =
        static_cast<uint16_t>(m3 * 1023.0f);

    commands.m4 =
        static_cast<uint16_t>(m4 * 1023.0f);

    return commands;
}