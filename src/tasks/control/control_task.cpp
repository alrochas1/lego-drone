// control_task.cpp
#include "tasks/control/control_task.hpp"

using namespace config;

ControlTask::ControlTask(QueueHandle_t imu_data_q, QueueHandle_t rc_data_q, QueueHandle_t motor_q)
    : Task("CONTROL", tasks::CONTROL_STACK_SIZE, tasks::CONTROL_PRIORITY),
      imu_data_queue_(imu_data_q),
      rc_data_queue_(rc_data_q),
      motor_queue_(motor_q) {
    
    printf("[CONTROL] Task created\n");
}

Inclination ControlTask::estimate_state(const IMUData& imu) {
    Inclination inclination{};
    
    // TODO: Implement inclination estimation from IMU
    
    inclination.roll = 0.0f;   // Placeholder
    inclination.pitch = 0.0f;  // Placeholder
    inclination.yaw = 0.0f;    // Placeholder
    
    return inclination;
}

MotorCommands ControlTask::get_motor_commands(const Inclination& current_state,
                                              const RCCommand& desired_state) {
    MotorCommands commands{};

    if (!desired_state.valid) {
        commands.m1 = 0;
        commands.m2 = 0;
        commands.m3 = 0;
        commands.m4 = 0;
        return commands;
    }

    commands = get_pid_commands(current_state, desired_state);
    return commands;
}

MotorCommands ControlTask::get_pid_commands(const Inclination& current_state, const RCCommand& desired_state) {
    MotorCommands commands{};

    // TODO: Implement PID control logic
    // Placeholder
    if(desired_state.throttle > 0.1f) {
        commands.m1 = static_cast<uint16_t>(1023*0.3); // Example: 30% throttle
        commands.m3 = static_cast<uint16_t>(1023*0.3); // Example: 30% throttle
    } else {
        commands.m1 = 0; // Motors off
        commands.m3 = 0; // Motors off
    }
    commands.m2 = 0;  // Placeholder
    commands.m4 = 0;  // Placeholder
    
    return commands;
}

void ControlTask::run() {
    IMUData imu_data{};
    RCCommand rc_command{};
    MotorCommands motor_commands{};
    
    printf("[CONTROL] Task started - Update interval: %lu ms\n", tasks::CONTROL_UPDATE_MS);
    
    while (true) {
        bool has_imu = xQueueReceive(imu_data_queue_, &imu_data, 0) == pdPASS;
        bool has_rc = xQueueReceive(rc_data_queue_, &rc_command, 0) == pdPASS;

        if (has_imu && has_rc) {
            Inclination current_state = estimate_state(imu_data);
            motor_commands = get_motor_commands(current_state, rc_command);
            xQueueOverwrite(motor_queue_, &motor_commands);
        }
        
        delay(tasks::CONTROL_UPDATE_MS);
    }
}
