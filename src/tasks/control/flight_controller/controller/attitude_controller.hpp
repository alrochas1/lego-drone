// attitude_controller.hpp

#pragma once

#include "types/sensor_data.hpp"
#include "types/control_data.hpp"
#include "types/comms_data.hpp"



class AttitudeController {
public:
    RateCommand update(
        const Attitude&  current_state,
        const RCCommand& desired_state);
};