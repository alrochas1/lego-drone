// i2c_device.cpp
#include "drivers/bus/i2c_device.hpp"
#include "config/project_config.hpp"
#include "config/hardware_config.hpp"

#include <cstdio>
#include <set>
#include <array>

using namespace config;

I2CDevice::I2CDevice(i2c_inst_t* i2c, uint8_t address) 
    : i2c_port_(i2c), device_address_(address) {
    
    // Keep track of which i2c ports were initialized so we don't re-init
    // the same port multiple times. This supports using i2c0 and i2c1
    // independently if needed.
    static std::set<i2c_inst_t*> initialized_ports;
    if (initialized_ports.find(i2c_port_) == initialized_ports.end()) {
        i2c_init(i2c_port_, i2c::BAUD_RATE);
        gpio_set_function(pins::I2C_SDA, GPIO_FUNC_I2C);    // TODO: Move pin declaration
        gpio_set_function(pins::I2C_SCL, GPIO_FUNC_I2C);
        gpio_pull_up(pins::I2C_SDA);
        gpio_pull_up(pins::I2C_SCL);
        initialized_ports.insert(i2c_port_);

        printf("I2C with address 0x%02X initialized on port %p at %lu Hz\n", device_address_, (void*)i2c_port_, i2c::BAUD_RATE);
    }
}

I2CDevice::Result I2CDevice::write_register(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};

    absolute_time_t timeout = make_timeout_time_ms(i2c::TIMEOUT_MS);
    
    const int result = i2c_write_blocking_until(i2c_port_, device_address_, 
                                               buffer, sizeof(buffer), false, timeout);
    
    if (result != static_cast<int>(sizeof(buffer))) {
        printf("[I2C] WRITE FAILED: reg=0x%02X, value=0x%02X, addr=0x%02X, port=%p, ret=%d\n",
               reg, value, device_address_, (void*)i2c_port_, result);
        return Result::WRITE_FAILED;
    }

    // Optional debug: successful write
    // printf("[I2C] WRITE OK: reg=0x%02X, addr=0x%02X, port=%p, bytes=%d\n",
    //        reg, device_address_, (void*)i2c_port_, result);

    return Result::SUCCESS;
}

I2CDevice::Result I2CDevice::write_registers(uint8_t start_reg, 
                                            const uint8_t* data, 
                                            size_t length) {
    if (data == nullptr || length == 0) {
        return Result::INVALID_PARAM;
    }
    
    std::array<uint8_t, 32> buffer;
    if (length + 1 > buffer.size()) {
        printf("I2C write too large: %zu bytes\n", length);
        return Result::INVALID_PARAM;
    }
    buffer[0] = start_reg;
    
    for (size_t i = 0; i < length; ++i) {
        buffer[i + 1] = data[i];
    }

    absolute_time_t timeout = make_timeout_time_ms(i2c::TIMEOUT_MS);
    
    const int result = i2c_write_blocking_until(i2c_port_, device_address_, 
                                               buffer.data(), length + 1, false, timeout);
    
    
    if (result != static_cast<int>(length + 1)) {
        printf("I2C multi-write failed: start_reg=0x%02X, length=%zu, result=%d\n",
               start_reg, length, result);
        return Result::WRITE_FAILED;
    }
    
    return Result::SUCCESS;
}


// TODO: Error handle
uint8_t I2CDevice::read_register(uint8_t reg) {
    uint8_t value = 0;
    
    absolute_time_t timeout = make_timeout_time_ms(i2c::TIMEOUT_MS);
    
    int ret = i2c_write_blocking_until(i2c_port_, device_address_, &reg, 1, true, timeout);
    if (ret != 1) {
        printf("[I2C] READ PRE-WRITE FAILED: reg=0x%02X, addr=0x%02X, port=%p, ret=%d\n",
               reg, device_address_, (void*)i2c_port_, ret);
        return 0;
    }
    
    ret = i2c_read_blocking_until(i2c_port_, device_address_, &value, 1, false, timeout);
    if (ret != 1) {
        printf("[I2C] READ FAILED: reg=0x%02X, addr=0x%02X, port=%p, ret=%d\n",
               reg, device_address_, (void*)i2c_port_, ret);
        return 0;
    }

    printf("Read value 0x%02X from register 0x%02X\n", value, reg);
    
    return value;
}

I2CDevice::Result I2CDevice::read_registers(uint8_t start_reg, 
                                           uint8_t* buffer, 
                                           size_t length) {
    if (buffer == nullptr || length == 0) {
        return Result::INVALID_PARAM;
    }

    absolute_time_t timeout = make_timeout_time_ms(i2c::TIMEOUT_MS);
    
    // Write start register
    int ret = i2c_write_blocking_until(i2c_port_, device_address_, &start_reg, 1, true, timeout);
    if (ret != 1) {
        printf("[I2C] MULTI-READ PRE-WRITE FAILED: start_reg=0x%02X, addr=0x%02X, port=%p, ret=%d\n",
               start_reg, device_address_, (void*)i2c_port_, ret);
        return Result::WRITE_FAILED;
    }
    
    // Read multiple registers
    ret = i2c_read_blocking_until(i2c_port_, device_address_, buffer, length, false, timeout);
    if (ret != static_cast<int>(length)) {
        printf("[I2C] MULTI-READ FAILED: start_reg=0x%02X, addr=0x%02X, port=%p, expected=%zu, got=%d\n",
               start_reg, device_address_, (void*)i2c_port_, length, ret);
        return Result::READ_FAILED;
    }
    
    return Result::SUCCESS;
}

bool I2CDevice::verify_connection(uint8_t who_am_i_reg, uint8_t expected_id) {
    auto who_am_i = read_register(who_am_i_reg);
    
    if (who_am_i != expected_id) {
        printf("[I2C] WHO_AM_I mismatch: expected 0x%02X, got 0x%02X\n", 
               expected_id, who_am_i);
        return false;
    }
    
    printf("[I2C] Device connected: WHO_AM_I = 0x%02X\n", who_am_i);
    return true;
}