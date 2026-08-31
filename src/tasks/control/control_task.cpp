// control_task.cpp
#include "tasks/control/control_task.hpp"

using namespace config;

ControlTask::ControlTask(QueueHandle_t imu_q, QueueHandle_t rc_q, QueueHandle_t motor_q)
    : Task("CONTROL", tasks::CONTROL_STACK_SIZE, tasks::CONTROL_PRIORITY),
      imu_queue_(imu_q),
      rc_queue_(rc_q),
      motor_queue_(motor_q) {
    
    printf("[CONTROL] Task created\n");
}


void ControlTask::run() {

    IMUData imu_data{};
    RCCommand rc_data{};
    MotorCommands motor_commands{};
    
    printf("[CONTROL] Task started - Update interval: %lu ms\n", tasks::CONTROL_UPDATE_MS);
    
    while (true) {

        // Get the latest IMU and RC data from the queues
        const bool imu_ready = xQueueReceive(imu_queue_, &imu_data, 0) == pdPASS;
        const bool rc_ready  = xQueueReceive(rc_queue_,  &rc_data,  0) == pdPASS;

        // Save the last valid RC command for use in control calculations
        if(rc_ready) {
            last_rc_ = rc_data;
        }

        if (imu_ready) {
            
            // Estimate the current state and compute control commands
            RCCommand control = flight_controller_.update(imu_data, last_rc_);
            
            // Calculate motor commands using PID controller
            motor_commands = motor_mixer_.mix_motors(control);
            
            // Send motor commands to the motor task
            xQueueOverwrite(motor_queue_, &motor_commands);
        }
        
        delay(tasks::CONTROL_UPDATE_MS);
    }
}
