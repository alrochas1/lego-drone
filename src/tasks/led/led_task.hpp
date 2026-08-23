// led_task.hpp
#pragma once
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include <queue.h>

#include "tasks/common/task_wrapper.hpp"
#include <drone_project/config/project_config.hpp>
#include <drone_project/types/system_data.hpp>


class LedTask : public Task {
private:
    uint8_t led_pin;
    bool state = false;
    QueueHandle_t snapshot_queue_;
    
    uint32_t get_blink_interval(SystemState state);
    
protected:
    void run() override;
    
public:
    LedTask(QueueHandle_t snapshot_queue);
}; 

