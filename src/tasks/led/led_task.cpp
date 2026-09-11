// led_task.cpp
#include "tasks/led/led_task.hpp"
#include "config/project_config.hpp"
#include "config/hardware_config.hpp"
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
        case SystemState::INIT:      return led::LED_INIT_BLINK_MS;  
        case SystemState::USB:       return led::LED_USB_BLINK_MS; 
        case SystemState::DISARMED:  return led::LED_DISARMED_BLINK_MS;  
        case SystemState::ARMED:     return led::LED_ARMED_BLINK_MS;  
        case SystemState::FLIGHT:    return led::LED_FLIGHT_BLINK_MS;  
        case SystemState::FAILSAFE:  return led::LED_FAILSAFE_BLINK_MS;  
        case SystemState::ERROR:     return led::LED_ERROR_BLINK_MS; 

        default:                     return tasks::LED_BLINK_MS;  
    }
}