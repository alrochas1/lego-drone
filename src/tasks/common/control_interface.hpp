// control_interface.hpp
// This file defines the interface for flight controllers in the drone project.

#pragma once

#include "types/sensor_data.hpp"
#include "types/comms_data.hpp"
#include "types/control_data.hpp"

class IFlightController {
public:
    virtual ~IFlightController() = default;

    virtual ControlOutput update(
        const IMUData& imu,
        const RCCommand& rc) = 0;
};