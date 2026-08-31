// imu_sim_task.hpp
#pragma once
#include "FreeRTOS.h"
#include "pico/time.h"
#include <queue.h>

#include "tasks/common/task_wrapper.hpp"
#include "types/sensor_data.hpp"
#include "config/project_config.hpp"

class IMUSimTask : public Task {
private:
    QueueHandle_t data_queue_;
    QueueHandle_t status_queue_;

public:
    IMUSimTask(QueueHandle_t data_queue, QueueHandle_t status_queue);

    void run() override;
};