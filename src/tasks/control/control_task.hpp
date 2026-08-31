// control_task.hpp
#pragma once
#include "tasks/common/task_wrapper.hpp"
#include "config/project_config.hpp"

#include "types/system_data.hpp"
#include "types/comms_data.hpp"
#include "types/control_data.hpp"

#include "flight_controller/flight_controller.hpp"
#include "motor_mixer/motor_mixer.hpp"

class ControlTask : public Task {
private:
    QueueHandle_t imu_queue_;
    QueueHandle_t rc_queue_;
    QueueHandle_t motor_queue_;
    
    FlightController    flight_controller_;
    MotorMixer          motor_mixer_;

    RCCommand last_rc_ = {};

public:
    ControlTask(QueueHandle_t imu_q, QueueHandle_t rc_q, QueueHandle_t motor_q);

protected:
    void run() override;
};
