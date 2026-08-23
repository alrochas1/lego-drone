// hardware_config.hpp

#pragma once
#include <cstdint>
#include "pico/stdlib.h"

namespace config {

// Pins configuration
namespace pins {
    constexpr uint8_t LED = PICO_DEFAULT_LED_PIN;
    constexpr uint8_t USB_PIN = 24; // USB detection pin
    constexpr uint8_t I2C_SDA = 26;
    constexpr uint8_t I2C_SCL = 27;
    constexpr uint8_t IR_PIN = 16;
    constexpr uint8_t M1_PIN = 2;
    constexpr uint8_t M2_PIN = 3;
    constexpr uint8_t M3_PIN = 4;
    constexpr uint8_t M4_PIN = 5;
    // TODO: update motor pins configuration
    // TODO: Add IR pins configuration
    // TODO: Add RC pins configuration
} // namespace pins

}