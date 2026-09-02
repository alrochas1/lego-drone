// control_data.hpp

#pragma once

#include <cstdint>

constexpr uint8_t N_MOTORS  = 4;
constexpr float   MOTOR_MAX = 1023.0f;

// Constants for control calculations
namespace {
    constexpr float GRAVITY             = 9.80665f;
    constexpr float COMPLEMENTARY_ALPHA = 0.98f;
    constexpr float TWO_PI              = 6.28318530718f;
}

// Struct for attitude representation
struct Attitude {
    float roll;     // rad
    float pitch;    // rad
    float yaw;      // rad
};

// Struct for rate command representation
struct RateCommand {
    float roll{};
    float pitch{};
    float yaw{};
};

// Structs for control data output
struct ControlOutput {
    float throttle; // [0, 1]
    float roll;     // [-1, 1]
    float pitch;    // [-1, 1]
    float yaw;      // [-1, 1]
};

// Struct for motor commands
struct MotorCommands {
    uint8_t  num_motors = N_MOTORS;  // Number of motors
    uint16_t motor[N_MOTORS];        // Motor power (0-1023)
};