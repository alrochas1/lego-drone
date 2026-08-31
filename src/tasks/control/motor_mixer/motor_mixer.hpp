// motor_mixer.hpp
#pragma once

#include "types/comms_data.hpp"
#include "types/control_data.hpp"


class MotorMixer {
public:
    MotorCommands mix_motors(
        const RCCommand& control);
};