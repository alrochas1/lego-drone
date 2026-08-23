// control_task.hpp
#pragma once
#include "tasks/common/task_wrapper.hpp"
#include "drone_project/config/project_config.hpp"

#include "drone_project/types/system_data.hpp"
#include "drone_project/types/comms_data.hpp"
#include "drone_project/types/control_data.hpp"



class ControlTask : public Task {
private:
    QueueHandle_t snapshot_queue_;
    QueueHandle_t motor_queue_;
    
    // TODO: PID controllers for each axis (placeholder for future implementation)
    DroneController pid_controllers_;
    
    // TODO: Implement inclination estimation logic
    Inclination estimate_state(const SensorData& imu);
    
    // TODO: Implement PID control logic
    MotorCommands get_motor_commands(const Inclination& current_state, 
                                        const SystemSnapshot& state);

    MotorCommands get_pid_commands(const Inclination& current_state, 
                                        const RCCommand& desired_state);

public:
    ControlTask(QueueHandle_t snapshot_q, QueueHandle_t motor_q);

protected:
    void run() override;
};
