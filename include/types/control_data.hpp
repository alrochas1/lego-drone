// control_data.hpp

#pragma once

#include <cstdint>

constexpr uint8_t N_MOTORS  = 4;
constexpr float   MOTOR_MAX = 1023.0f;

// Constants for control calculations
constexpr float RATE_KP = 0.08f;
constexpr float RATE_KI = 0.02f;

constexpr float INTEGRAL_LIMIT = 1.0f;
constexpr float OUTPUT_LIMIT   = 1.0f;

constexpr float MAX_ANGLE =
    30.0f * 3.14159265359f / 180.0f;

constexpr float MAX_YAW_RATE =
    180.0f * 3.14159265359f / 180.0f;

constexpr float ATTITUDE_KP = 4.0f;

constexpr float GRAVITY             = 9.80665f;         // Move to general constants header
constexpr float COMPLEMENTARY_ALPHA = 0.98f;
constexpr float TWO_PI              = 6.28318530718f;   // Move to general constants header


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