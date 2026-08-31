// freeRTOS_data.hpp

#pragma once
#include "FreeRTOS.h"
#include "queue.h"

struct SystemQueues {
    
    // System queue for system monitor and logging
    QueueHandle_t snapshot_queue;

    // Imput Queues
    QueueHandle_t rc_data;
    QueueHandle_t imu_data;

    QueueHandle_t rc_status;
    QueueHandle_t imu_status;
        
    // Output Queues
    QueueHandle_t motor_queue;

    // Logging queue
    QueueHandle_t log_queue;
};