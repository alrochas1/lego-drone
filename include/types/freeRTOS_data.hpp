// freeRTOS_data.hpp

#pragma once
#include "FreeRTOS.h"
#include "queue.h"

struct SystemQueues {
    
    // System queue for system monitor and logging
    QueueHandle_t snapshot_queue;

    // Imput Queues
    QueueHandle_t rc_queue;
    QueueHandle_t imu_queue;
        
    // Output Queues
    QueueHandle_t motor_queue;
};