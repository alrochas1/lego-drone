// attitude_estimator.hpp

#pragma once

#include "types/control_data.hpp"
#include "types/sensor_data.hpp"


class AttitudeEstimator {
public:
    Attitude update(const IMUData& imu, const float dt);

private:
    Attitude attitude_{};
};