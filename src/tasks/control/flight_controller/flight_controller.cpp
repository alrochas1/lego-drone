// flight_controller.cpp
#include "flight_controller.hpp"

#include <algorithm>
#include <cmath>

ControlOutput FlightController::update(
    const IMUData& imu,
    const RCCommand& rc)
{
    // Update the time delta based on the latest IMU timestamp
    const float dt = update_dt(imu.gyro.timestamp_ms); // FIXME
    
    // Estimate the current state based on sensor data
    const Attitude current_state = attitude_estimator_.update(imu, dt);

    // Compute the desired rate based on the current state and desired RC command
    const RateCommand desired_rate = attitude_controller_.update(current_state, rc);

    // Compute the control command based on the desired rate and sensor data
    const ControlOutput control = rate_controller_.update(imu, desired_rate, rc.throttle, dt);

    return control;
}

// Update the time delta based on the latest IMU timestamp
float FlightController::update_dt(uint32_t timestamp)
{
    float dt = 0;

    // Get the time difference between the current and previous timestamps
    if (previous_timestamp_ != 0 &&
        timestamp > previous_timestamp_) {

        dt =
            static_cast<float>(
                timestamp - previous_timestamp_) *
            1e-3f;
    }

    previous_timestamp_ = timestamp;

    dt = std::clamp(dt, 0.001f, 0.02f);

    return dt;
}