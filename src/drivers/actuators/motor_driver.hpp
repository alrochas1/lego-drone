// motor_driver.hpp --> This is for controlling a motor using a DRV8833. 
// It only uses one direction (the drone doesnt need to reverse)
#pragma once
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <cstdint>

class MotorDriver {
private:
    uint pwm_pin_;
    uint slice_num_;
    uint channel_;

public:
    MotorDriver(uint pwm_pin);
    void init();
    void set_power(uint16_t value); // 0-1000
};