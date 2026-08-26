// motor_task.hpp
#pragma once
#include "tasks/common/task_wrapper.hpp"
#include "drivers/motor_driver.hpp"
#include "drone_project/config/project_config.hpp"
#include "drone_project/config/hardware_config.hpp"
#include "drone_project/types/control_data.hpp"

class MotorTask : public Task {
private:
    MotorDriver m1_;
    MotorDriver m2_;
    MotorDriver m3_;
    MotorDriver m4_;

    QueueHandle_t motor_queue_;

public:
    MotorTask(QueueHandle_t motor_queue);

protected:
    void run() override;
};