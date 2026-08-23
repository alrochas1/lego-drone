// control_task.cpp
#include "tasks/control/control_task.hpp"

using namespace config;

ControlTask::ControlTask(QueueHandle_t snapshot_q, QueueHandle_t motor_q)
    : Task("CONTROL", tasks::CONTROL_STACK_SIZE, tasks::CONTROL_PRIORITY),
      snapshot_queue_(snapshot_q),
      motor_queue_(motor_q) {
    
    printf("[CONTROL] Task created\n");
}

Inclination ControlTask::estimate_state(const SensorData& imu) {
    Inclination inclination{};
    
    // TODO: Implement inclination estimation from IMU
    
    inclination.roll = 0.0f;   // Placeholder
    inclination.pitch = 0.0f;  // Placeholder
    inclination.yaw = 0.0f;    // Placeholder
    
    return inclination;
}

MotorCommands ControlTask::get_motor_commands(const Inclination& current_state,
                                                   const SystemSnapshot& state) {
    MotorCommands commands{};

    bool validState = state.state == SystemState::ARMED || state.state == SystemState::FLIGHT;
    if(!validState) {
        // TODO: Improve this
        commands.m1 = 0;
        commands.m2 = 0;
        commands.m3 = 0;
        commands.m4 = 0;
        return commands; // Motors off if not armed or in flight
    }
    
    // Get PID commands from control logic
    commands = get_pid_commands(current_state, state.rc);
    
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
    SystemSnapshot snapshot{};
    MotorCommands motor_commands{};
    
    printf("[CONTROL] Task started - Update interval: %lu ms\n", tasks::CONTROL_UPDATE_MS);
    
    while (true) {
        if (xQueuePeek(snapshot_queue_, &snapshot, 0) == pdPASS) {
            
            // Estimate the drone's inclination state from IMU data
            Inclination current_state = estimate_state(snapshot.imu);
            
            // Calculate motor commands using PID controller
            motor_commands = get_motor_commands(current_state, snapshot);
            
            // Send motor commands to the motor task
            xQueueOverwrite(motor_queue_, &motor_commands);
        }
        
        delay(tasks::CONTROL_UPDATE_MS);
    }
}
