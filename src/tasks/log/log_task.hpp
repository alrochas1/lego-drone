// log_task.hpp
#pragma once
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include <queue.h>

#include "tasks/common/task_wrapper.hpp"
#include <config/project_config.hpp>
#include <types/sensor_data.hpp>
#include <types/system_data.hpp>
#include <types/control_data.hpp>


class LogTask : public Task {
private:
    const char* state_to_string(SystemState state);
    
    QueueHandle_t snapshot_queue_;
    QueueHandle_t motor_queue_;
    
public:
    LogTask(QueueHandle_t snapshot_queue, QueueHandle_t motor_queue);

    void run() override;
}; 

