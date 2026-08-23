// task_wrapper.hpp
#pragma once
#include "FreeRTOS.h"
#include <queue.h>
#include "task.h"
#include <functional>
#include <string>

class Task {
private:
    TaskHandle_t handle = nullptr;
    std::string name;
    uint32_t stack_depth;
    UBaseType_t priority;
    
    static void task_function(void* params) {
        auto* task = static_cast<Task*>(params);
        task->run();
    }
    
protected:
    virtual void run() = 0;
    
public:
    Task(const std::string& task_name, uint32_t stack_size, UBaseType_t task_priority)
        : name(task_name), stack_depth(stack_size), priority(task_priority) {}
    
    virtual ~Task() {
        if (handle) {
            vTaskDelete(handle);
        }
    }
    
    bool start(void* params = nullptr) {
        return xTaskCreate(task_function, name.c_str(), stack_depth, this, priority, &handle) == pdPASS;
    }
    
    void delay(uint32_t ms) {
        TickType_t last = xTaskGetTickCount();
        vTaskDelayUntil(&last, pdMS_TO_TICKS(ms));
    }
};


class I2CSensorTask : public Task{
protected:
    QueueHandle_t data_queue_;
    // bool initialized_{false};

    // virtual bool initialize_sensor() = 0;

public:
    I2CSensorTask(const std::string& name, uint32_t stack_size, UBaseType_t priority, QueueHandle_t data_queue)
        : Task(name, stack_size, priority), data_queue_(data_queue) {}

};