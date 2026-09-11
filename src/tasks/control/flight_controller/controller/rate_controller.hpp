// rate_controller.hpp

#pragma once

#include "types/sensor_data.hpp"
#include "types/control_data.hpp"

class RateController {
public:
    ControlOutput update(
        const IMUData&     imu,
        const RateCommand& rate_command,
        const float        throttle,
        const float        dt);
};