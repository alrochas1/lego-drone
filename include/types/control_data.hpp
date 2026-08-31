// control_data.hpp

#pragma once

constexpr uint8_t N_MOTORS = 4;

struct PIDController {
    // Placeholder for PID controller implementation
};

struct Inclination {
    float roll;      // Rotation around X-axis
    float pitch;     // Rotation around Y-axis
    float yaw;       // Rotation around Z-axis
};

struct DroneController {
    PIDController roll;
    PIDController pitch;
    PIDController yaw;
    PIDController altitude;
};

struct MotorCommands {
    uint8_t  num_motors = N_MOTORS;  // Number of motors
    uint16_t motor[N_MOTORS];        // Motor power (0-1023)
};