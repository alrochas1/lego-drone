// flight_controller.hpp
#pragma once

// #include "control_output.hpp"
#include "types/sensor_data.hpp"
#include "types/comms_data.hpp"

#include "tasks/common/control_interface.hpp"

#include "estimator/attitude_estimator.hpp"
#include "controller/attitude_controller.hpp"
#include "controller/rate_controller.hpp"

class FlightController : public IFlightController {
public:
    ControlOutput update(
        const IMUData& imu,
        const RCCommand& rc) override;

private:
    AttitudeEstimator   attitude_estimator_{};
    AttitudeController  attitude_controller_{};
    RateController      rate_controller_{};

    float update_dt(uint32_t timestamp);

    float       dt_{0.002f};
    uint32_t    previous_timestamp_{0};
};