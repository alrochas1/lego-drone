// sensor_data.hpp

#pragma once
#include <cstdint>
#include <cmath>

struct Vector3f {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    
    constexpr Vector3f() = default;
    constexpr Vector3f(float x_val, float y_val, float z_val) 
        : x(x_val), y(y_val), z(z_val) {}
    
    bool is_valid() const {
        return !(std::isnan(x) || std::isnan(y) || std::isnan(z));
    }

    float magnitude() const {
        return std::sqrt(x*x + y*y + z*z);
    }
    
    Vector3f normalized() const {
        float mag = magnitude();
        if (mag > 0.0f) {
            return Vector3f(x/mag, y/mag, z/mag);
        }
        return *this;
    }
};

struct GyroData {
    Vector3f angular_velocity; // dps
    uint32_t timestamp_ms{0};
    bool valid{false};
    
    GyroData() = default;
    GyroData(Vector3f velocity, uint32_t ts) 
        : angular_velocity(velocity), timestamp_ms(ts), valid(velocity.is_valid()) {}
};

struct AccelData {
    Vector3f linear_acceleration; // m/s²
    uint32_t timestamp_ms{0};
    bool valid{false};
    
    AccelData() = default;
    AccelData(Vector3f acceleration, uint32_t ts)
        : linear_acceleration(acceleration), timestamp_ms(ts), valid(acceleration.is_valid()) {}
};

struct MagData {
    Vector3f magnetic_field; // uT
    uint32_t timestamp_ms{0};
    bool valid{false};
    
    MagData() = default;
    MagData(Vector3f field, uint32_t ts)
        : magnetic_field(field), timestamp_ms(ts), valid(field.is_valid()) {}
};

struct IMUData {
    GyroData gyro;
    AccelData accel; 
    // MagData mag; // TODO: Implement mag task (not used in drone_project)

    uint32_t sequence_number{0};   // TODO: Check
    
    bool has_gyro() const { return gyro.valid; }
    bool has_accel() const { return accel.valid; }
    bool has_mag() const { return false; } // Mag not implemented in drone_project

    bool is_complete() const {
        return has_gyro() && has_accel(); // && has_mag(); // Mag not implemented in drone_project
    }
    
    bool has_any_data() const {
        return has_gyro() || has_accel() || has_mag();
    }
};


// IMU status data for monitoring sensor health and data validity
struct IMUStatus {
    bool     valid{false};
    uint32_t sequence_number{0};
    uint32_t timestamp_ms{0};
};


// IMU configuration for sensor orientation and axis mapping
enum class SensorAxis {
    X,
    Y,
    Z
};

struct AxisMapping {
    SensorAxis source;
    int8_t sign;
};

struct IMUOrientation {
    AxisMapping forward;
    AxisMapping right;
    AxisMapping down;
};


// My IMU is rotated 90º around the Z axis, so the mapping is as follows:
constexpr IMUOrientation imu_orientation{
    .forward = {SensorAxis::Y, +1},
    .right   = {SensorAxis::X, -1},
    .down    = {SensorAxis::Z, +1}
};