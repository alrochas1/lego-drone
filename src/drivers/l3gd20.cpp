// l3gd20.cpp
#include "drivers/l3gd20.hpp"
#include <cstdio>
#include <array>

using namespace config;

L3GD20::L3GD20(i2c_inst_t* i2c) 
    : I2CDevice(i2c, imu::GYRO_ADDRESS) {
    calculate_sensitivity();
}

bool L3GD20::initialize() {
    return initialize_config(config_);
}

bool L3GD20::initialize_config(const Config& config) {
    config_ = config;
    
    printf("[GYRO] Initializing %s...\n", get_name());
    
    if (!verify_connection(imu::gyro::WHO_AM_I_REG, imu::gyro::WHO_AM_I_VALUE)) {
        printf("[GYRO] %s not found or communication failed\n", get_name());
        // return false; // IMPROVE: Allow degraded mode
    }
    
    // Configure the device
    if (!set_ctrl_reg1() || !set_ctrl_reg4()) {
        printf("[GYRO] %s configuration failed\n", get_name());
        return false;
    }
    
    sleep_ms(10); 
    

    printf("[GYRO] %s initialized successfully\n", get_name());
    printf("[GYRO] Range: %d dps, Rate: configured, Samples: 0\n", 
           config_.range == Range::DPS_250 ? 250 : 
           config_.range == Range::DPS_500 ? 500 : 2000);
    
    initialized_ = true;
    return true;
}


void L3GD20::calculate_sensitivity() {
    switch (config_.range) {
        case Range::DPS_250:
            sensitivity_dps_digit_ = 0.00875F; // mdps/digit to dps/digit
            break;
        case Range::DPS_500:
            sensitivity_dps_digit_ = 0.01750F;
            break;
        case Range::DPS_2000:
            sensitivity_dps_digit_ = 0.07000F;
            break;
    }
}

bool L3GD20::set_ctrl_reg1() {
    uint8_t ctrl_reg1 = 0x0F; // Power on, enable XYZ axes
    
    // // Add data rate
    // switch (config_.output_rate) {
    //     case OutputRate::HZ_95:  ctrl_reg1 |= (0b00 << 6); break;
    //     case OutputRate::HZ_190: ctrl_reg1 |= (0b01 << 6); break;
    //     case OutputRate::HZ_380: ctrl_reg1 |= (0b10 << 6); break;
    //     case OutputRate::HZ_760: ctrl_reg1 |= (0b11 << 6); break;
    // }
    
    return write_register(0x20, ctrl_reg1) == Result::SUCCESS;
}

bool L3GD20::set_ctrl_reg4() {
    uint8_t ctrl_reg4 = 0x00; // Continuos update, little endian
    
    // Añadir rango
    switch (config_.range) {
        case Range::DPS_250:  ctrl_reg4 |= (0b00 << 4); break;
        case Range::DPS_500:  ctrl_reg4 |= (0b01 << 4); break;
        case Range::DPS_2000: ctrl_reg4 |= (0b10 << 4); break;
    }
    
    return write_register(0x23, ctrl_reg4) == Result::SUCCESS;
}


GyroData L3GD20::read_gyro() {
    
    std::array<uint8_t, 6> buffer;
    
    // Read data registers (0x28-0x2D) with auto-increment (0x80)
    if (read_registers(0x28 | 0x80, buffer) != Result::SUCCESS) {
        printf("[GYRO] Failed to read %s gyro data registers\n", get_name());
        return GyroData();
    }
    
    // Convert data (little endian)
    const int16_t raw_x = static_cast<int16_t>(buffer[0]) | (static_cast<int16_t>(buffer[1]) << 8);
    const int16_t raw_y = static_cast<int16_t>(buffer[2]) | (static_cast<int16_t>(buffer[3]) << 8);
    const int16_t raw_z = static_cast<int16_t>(buffer[4]) | (static_cast<int16_t>(buffer[5]) << 8);
    
    // Sensitivity
    Vector3f angular_velocity(
        static_cast<float>(raw_x) * sensitivity_dps_digit_,
        static_cast<float>(raw_y) * sensitivity_dps_digit_,
        static_cast<float>(raw_z) * sensitivity_dps_digit_
    );
    
    GyroData data(angular_velocity, to_ms_since_boot(get_absolute_time()));
    return data;
}