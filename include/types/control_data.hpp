// control_data.hpp

#pragma once

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
    uint16_t m1;  // Motor 1 power (0-1023)
    uint16_t m2;  // Motor 2 power (0-1023)
    uint16_t m3;  // Motor 3 power (0-1023)
    uint16_t m4;  // Motor 4 power (0-1023)
};