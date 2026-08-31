// motor_task.hpp
#pragma once

#include "config/project_config.hpp"
#include "config/hardware_config.hpp"
#include "types/control_data.hpp"

#include "drivers/actuators/motor_driver.hpp"
#include "tasks/common/task_wrapper.hpp"

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