// project_config.hpp

#pragma once
#include <cstdint>

namespace config {

// I2C configuration
namespace i2c {
    constexpr uint32_t BAUD_RATE = 400000; // 400 kHz
    constexpr uint32_t TIMEOUT_MS = 100;
} // namespace i2c

// IMU addresses
namespace imu {
    constexpr uint8_t GYRO_ADDRESS = 0x69;      // L3GD20
    constexpr uint8_t ACCEL_ADDRESS = 0x1E;     // LSM303D
    
    namespace gyro{
        constexpr uint8_t WHO_AM_I_VALUE = 0xD3;   // WHO_AM_I value for L3GD20
        constexpr uint8_t WHO_AM_I_REG = 0x0F;     // WHO_AM_I register for L3GD20
    }
    namespace accel{
        constexpr uint8_t WHO_AM_I_VALUE = 0x3C;   // WHO_AM_I value for LSM303D accel
        constexpr uint8_t WHO_AM_I_REG = 0x0F;     // WHO_AM_I register for LSM303D accel
    }
    namespace mag{
        constexpr uint8_t WHO_AM_I_VALUE = 0xD3;   // WHO_AM_I value for LSM303D mag (revisar)
        constexpr uint8_t WHO_AM_I_REG = 0x0F;     // WHO_AM_I register for LSM303D mag
    }
} // namespace imu

// Task configuration
namespace tasks {
    constexpr uint32_t SYSTEM_STACK_SIZE    = 256;
    constexpr uint32_t LED_STACK_SIZE       = 256;
    constexpr uint32_t LOG_STACK_SIZE       = 1024;
    constexpr uint32_t IMU_STACK_SIZE       = 1024;
    constexpr uint32_t IR_STACK_SIZE        = 1024;
    constexpr uint32_t RC_STACK_SIZE        = 1024;
    constexpr uint32_t CONTROL_STACK_SIZE   = 1024;
    constexpr uint32_t MOTOR_STACK_SIZE     = 1024;
    
    
    constexpr uint8_t SYSTEM_PRIORITY   = 1;
    constexpr uint8_t LED_PRIORITY      = 1;
    constexpr uint8_t LOG_PRIORITY      = 1;
    constexpr uint8_t IMU_PRIORITY      = 3;
    constexpr uint8_t IR_PRIORITY       = 2;
    constexpr uint8_t RC_PRIORITY       = 2;
    constexpr uint8_t CONTROL_PRIORITY  = 4;
    constexpr uint8_t MOTOR_PRIORITY    = 3;


    constexpr uint32_t SYSTEM_UPDATE_MS     = 50;   // 20 Hz
    constexpr uint32_t LED_BLINK_MS         = 500;  // 2 Hz
    constexpr uint32_t LOG_PRINT_MS         = 500;  // 2 Hz
    constexpr uint32_t IMU_SAMPLE_MS        = 10;   // 100 Hz
    constexpr uint32_t IR_UPDATE_MS         = 20;   // 50 Hz
    constexpr uint32_t RC_UPDATE_MS         = 20;   // 50 Hz
    constexpr uint32_t CONTROL_UPDATE_MS    = 10;   // 100 Hz
    constexpr uint32_t MOTOR_UPDATE_MS      = 10;   // 100 Hz


    // Timeout constants for state machine countdowns
    constexpr uint32_t IMU_TIMEOUT_MS           = 2000;  // 2 seconds
    constexpr uint32_t RC_TIMEOUT_MS            = 1000;  // 1 second
    constexpr uint32_t INIT_TIMEOUT_MS          = 5000;  // 5 seconds
    constexpr uint32_t THROTTLE_LOW_TIMEOUT_MS  = 2000;  // 2 seconds

    constexpr uint32_t IMU_COUNTDOWN            = IMU_TIMEOUT_MS/SYSTEM_UPDATE_MS;
    constexpr uint32_t RC_COUNTDOWN             = RC_TIMEOUT_MS/SYSTEM_UPDATE_MS;
    constexpr uint32_t INIT_COUNTDOWN           = INIT_TIMEOUT_MS/SYSTEM_UPDATE_MS;
    constexpr uint32_t THROTTLE_LOW_COUNTDOWN   = THROTTLE_LOW_TIMEOUT_MS/SYSTEM_UPDATE_MS;
} // namespace tasks

// Queue configuration
namespace queues {
    constexpr uint8_t SYSTEM_QUEUE_LENGTH = 1;
    constexpr uint8_t SENSOR_QUEUE_LENGTH = 10;
    constexpr uint8_t LOG_QUEUE_LENGTH    = 10; // TODO: Check
    // constexpr uint8_t MAX_QUEUE_WAIT_MS = portMAX_DELAY;
} // namespace queues

} // namespace config