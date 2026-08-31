// motor_driver.cpp --> This is for controlling a motor using a custom driver (SI2300 + 1N4148). 
#include "motor_driver.hpp"

MotorDriver::MotorDriver(uint pwm_pin) 
    : pwm_pin_(pwm_pin) {
    slice_num_ = pwm_gpio_to_slice_num(pwm_pin_);
    channel_ = pwm_gpio_to_channel(pwm_pin_);
}

void MotorDriver::init() {
    gpio_set_function(pwm_pin_, GPIO_FUNC_PWM);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 6.25f);          // 125MHz / 6.25 = 20 MHz
    pwm_config_set_wrap(&config, 1000);             // 20 MHz / 1000 = 20 kHz
    pwm_init(slice_num_, &config, true);            // start enabled
    set_power(0);                                   // Start with motors off
}

void MotorDriver::set_power(uint16_t value) {
    if (value > 1000) value = 1000;
    pwm_set_gpio_level(pwm_pin_, value);
}