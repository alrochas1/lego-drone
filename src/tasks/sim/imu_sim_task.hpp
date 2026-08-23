// imu_sim_task.hpp
#pragma once
#include "FreeRTOS.h"
#include "pico/time.h"
#include <queue.h>

#include "tasks/common/task_wrapper.hpp"
#include "drone_project/types/sensor_data.hpp"
#include "drone_project/config/project_config.hpp"

class IMUSimTask : public Task {
private:
    QueueHandle_t data_queue_;

public:
    explicit IMUSimTask(QueueHandle_t queue);

    void run() override;
};