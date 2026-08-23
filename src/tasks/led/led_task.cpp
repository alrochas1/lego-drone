// led_task.cpp
#include "tasks/led/led_task.hpp"
#include "drone_project/config/project_config.hpp"
#include <cstdio>

using namespace config;

LedTask::LedTask(QueueHandle_t snapshot_queue) 
    : Task("LED", tasks::LED_STACK_SIZE, tasks::LED_PRIORITY)
    , led_pin(pins::LED)
    , snapshot_queue_(snapshot_queue) {
    
    printf("[LED] Task created (pin: %d)\n", led_pin);
}

void LedTask::run() {
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    
    printf("[LED] Task started - Blink interval: %lu ms\n", tasks::LED_BLINK_MS);
    
    while (true) {
        SystemSnapshot snap;
        uint32_t blink_ms = tasks::LED_BLINK_MS; // default
        
        if (xQueuePeek(snapshot_queue_, &snap, 0) == pdPASS) {
            blink_ms = get_blink_interval(snap.state);
        }
        
        gpio_put(led_pin, state);
        state = !state;
                
        delay(blink_ms);
    }
}

uint32_t LedTask::get_blink_interval(SystemState state) {
    switch (state) {
        // TODO: Move numbers to config
        case SystemState::INIT:      return 2000;  
        case SystemState::USB:       return 2000; 
        case SystemState::DISARMED:  return 1000;  
        case SystemState::ARMED:     return 500;  
        case SystemState::FLIGHT:    return 250;  
        case SystemState::FAILSAFE:  return 100;  
        case SystemState::ERROR:     return 5000; 
        default:                     return tasks::LED_BLINK_MS;  
    }
}