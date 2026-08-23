#pragma once
#include "FreeRTOS.h"
#include "queue.h"
#include "drone_project/types/sensor_data.hpp"
#include "drone_project/types/comms_data.hpp"
#include "drone_project/types/system_data.hpp"

// SYSTEM STATES
enum class SystemState {
    INIT,
    USB,        // System is connected to USB instead of battery (for development/testing)
    DISARMED,   // System is on battery but not ready for flight (motors off) - default state after init with battery
    ARMED,      // System is ready for flight (can take off, motors on)
    FLIGHT,     // System is in flight (motors on, not in failsafe)
    FAILSAFE,
    ERROR
};

struct SystemInputs {
    bool usb_connected;
    bool imu_ok;
    bool rc_ok;

    float throttle;
};

enum class RunMode {
    FLIGHT,     // Normal operation with real sensors and RC
    IMU_SIM,    // Simulated IMU data, real RC input
    RC_SIM,     // Simulated RC input, real IMU data
    SIMULATION  // Fully simulated (for testing without hardware)
};


struct SystemQueues {
    
    // System queue for system monitor and logging
    QueueHandle_t snapshot_queue;

    // Imput Queues
    QueueHandle_t rc_queue;
    QueueHandle_t imu_queue;
        
    // Output Queues
    QueueHandle_t motor_queue;
};

struct SystemSnapshot {
    uint32_t timestamp_ms;
    SystemState state;
    SensorData imu;
    RCCommand rc;
};


