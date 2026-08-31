// flight_controller.hpp
#pragma once

#include "types/sensor_data.hpp"
#include "types/control_data.hpp"

class FlightController {
public:
    FlightController() = default;

    RCCommand update(const IMUData& imu, const RCCommand& rc);

private:
    Inclination estimate_state(const IMUData& imu);

    RCCommand attitude_controller(
        const Inclination& current_state,
        const RCCommand& desired_state);

    RCCommand rate_controller(
        const RCCommand& desired_rate,
        const IMUData& imu);

    MotorCommands mix_motors(
        const RCCommand& control,
        float throttle);

    float update_dt(uint32_t timestamp);

    float       dt_{0.002f};
    uint32_t    previous_timestamp_{0};
    Inclination attitude_{};
};