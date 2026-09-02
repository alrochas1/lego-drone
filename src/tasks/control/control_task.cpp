// control_task.cpp
#include "tasks/control/control_task.hpp"

using namespace config;

ControlTask::ControlTask(
    QueueHandle_t imu_q, 
    QueueHandle_t rc_q, 
    QueueHandle_t motor_q,
    IFlightController* controller)
    : Task("CONTROL", tasks::CONTROL_STACK_SIZE, tasks::CONTROL_PRIORITY),
      imu_queue_(imu_q),
      rc_queue_(rc_q),
      motor_queue_(motor_q),
      controller_(
          controller != nullptr
              ? controller
              : &default_controller_) {
    
    printf("[CONTROL] Task created\n");
}


void ControlTask::run() {

    IMUData imu_data{};
    RCCommand rc_data{};
    MotorCommands motor_commands{};
    
    printf("[CONTROL] Task started - Update interval: %lu ms\n", tasks::CONTROL_UPDATE_MS);
    
    while (true && controller_ != nullptr) {

        // Get the latest IMU and RC data from the queues
        const bool imu_ready = xQueueReceive(imu_queue_, &imu_data, 0) == pdPASS;
        const bool rc_ready  = xQueueReceive(rc_queue_,  &rc_data,  0) == pdPASS;

        // Save the last valid RC command for use in control calculations
        if(rc_ready) {
            last_rc_ = rc_data;
        }

        // If we have valid IMU data, proceed with control calculations
        if (imu_ready) {
            
            // Estimate the current state and compute control commands
            ControlOutput control = controller_->update(imu_data, last_rc_);
            
            // Convert control outputs into motor commands
            motor_commands = motor_mixer_.mix_motors(control);
            
            // Send motor commands to the motor task
            xQueueOverwrite(motor_queue_, &motor_commands);
        }
        
        delay(tasks::CONTROL_UPDATE_MS);
    }

    // In case the controller is null, we can log an error and halt the task
    printf("[CONTROL] ERROR: Flight controller is not initialized. Halting control task.\n");
    while (true) {
        delay(tasks::CONTROL_UPDATE_MS); // Delay to prevent busy waiting
    }
}
