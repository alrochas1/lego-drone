// i2c_device.hpp
#pragma once
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <cstdint>
#include <array>

class I2CDevice {
public:
    enum class Result {
        SUCCESS,
        WRITE_FAILED,
        READ_FAILED,
        TIMEOUT,
        INVALID_PARAM
    };

protected:
    i2c_inst_t* i2c_port_;
    const uint8_t device_address_;
    bool initialized_{false};
    
    Result write_register(uint8_t reg, uint8_t value);
    Result write_registers(uint8_t start_reg, const uint8_t* data, size_t length);
    
    uint8_t read_register(uint8_t reg);
    Result read_registers(uint8_t start_reg, uint8_t* buffer, size_t length);

    template<size_t N>
    Result read_registers(uint8_t start_reg, std::array<uint8_t, N>& buffer) {
        return read_registers(start_reg, buffer.data(), N);
    }
    
    bool verify_connection(uint8_t who_am_i_reg, uint8_t expected_id);

public:
    I2CDevice(i2c_inst_t* i2c, uint8_t address);
    virtual ~I2CDevice() = default;
    
    // No copiable
    I2CDevice(const I2CDevice&) = delete;
    I2CDevice& operator=(const I2CDevice&) = delete;
    
    
    virtual bool initialize() = 0;
    bool is_initialized() const { return initialized_; }
    uint8_t get_address() const { return device_address_; }
};