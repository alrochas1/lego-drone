#include "drivers/lsm303d_accel.hpp"
#include <cstdio>
#include <array>

using namespace config;

LSM303D_Accel::LSM303D_Accel(i2c_inst_t* i2c) 
    : I2CDevice(i2c, imu::ACCEL_ADDRESS) {
}

bool LSM303D_Accel::initialize() {
    return initialize_config(config_);
}

bool LSM303D_Accel::initialize_config(const Config& config) {
    config_ = config;
    calculate_sensitivity();
    
    printf("[ACCEL] Initializing %s...\n", get_name());
    
    if (!verify_connection(imu::accel::WHO_AM_I_REG, imu::accel::WHO_AM_I_VALUE)) {
        printf("[ACCEL] %s not found\n", get_name());
        return false;
    }
    
    if (!set_ctrl_reg1() || !set_ctrl_reg2()) {
        printf("[ACCEL] Configuration failed\n");
        return false;
    }
    
    sleep_ms(10);
    initialized_ = true;
    
    printf("[ACCEL] %s ready - Range: %d g\n", get_name(),
           config_.range == Range::G_2 ? 2 :
           config_.range == Range::G_4 ? 4 :
           config_.range == Range::G_6 ? 6 :
           config_.range == Range::G_8 ? 8 : 16);
    
    return true;
}

void LSM303D_Accel::calculate_sensitivity() {
    switch (config_.range) {
        case Range::G_2:  sensitivity_ = 0.061f / 1000.0f; break; // mg/LSB to g/LSB
        case Range::G_4:  sensitivity_ = 0.122f / 1000.0f; break;
        case Range::G_6:  sensitivity_ = 0.183f / 1000.0f; break;
        case Range::G_8:  sensitivity_ = 0.244f / 1000.0f; break;
        case Range::G_16: sensitivity_ = 0.732f / 1000.0f; break;
    }
}

bool LSM303D_Accel::set_ctrl_reg1() {
    uint8_t ctrl_reg1 = 0x00;
    
    // Data rate (ODR)
    switch (config_.output_rate) {
        case OutputRate::HZ_50:  ctrl_reg1 |= (0b0100 << 4); break;  // 50Hz
        case OutputRate::HZ_100: ctrl_reg1 |= (0b0101 << 4); break;  // 100Hz
        case OutputRate::HZ_200: ctrl_reg1 |= (0b0110 << 4); break;  // 200Hz
        case OutputRate::HZ_400: ctrl_reg1 |= (0b0111 << 4); break;  // 400Hz
    }
    
    // Enable all axes (X, Y, Z)
    ctrl_reg1 |= 0x07;
    
    return write_register(0x20, ctrl_reg1) == Result::SUCCESS;
}

bool LSM303D_Accel::set_ctrl_reg2() {
    uint8_t ctrl_reg2 = 0x00;
    
    // Full scale selection
    switch (config_.range) {
        case Range::G_2:  ctrl_reg2 |= (0b000 << 3); break;  // ±2g
        case Range::G_4:  ctrl_reg2 |= (0b001 << 3); break;  // ±4g
        case Range::G_6:  ctrl_reg2 |= (0b010 << 3); break;  // ±6g
        case Range::G_8:  ctrl_reg2 |= (0b011 << 3); break;  // ±8g
        case Range::G_16: ctrl_reg2 |= (0b100 << 3); break;  // ±16g
    }
    
    // Anti-aliasing filter bandwidth
    ctrl_reg2 |= 0x00; // 50Hz bandwidth
    
    return write_register(0x21, ctrl_reg2) == Result::SUCCESS;
}

AccelData LSM303D_Accel::read_accel() {
    if (!initialized_) {
        return AccelData();
    }
    
    std::array<uint8_t, 6> buffer;
    
    // 0x28 = OUT_X_L_A, 0x80 = auto-increment
    if (read_registers(0x28 | 0x80, buffer) != Result::SUCCESS) {
        printf("[ACCEL] Failed to read data registers\n");
        return AccelData();
    }
    
    // Convert data (little endian, 2's complement)
    int16_t raw_x = static_cast<int16_t>(buffer[0]) | (static_cast<int16_t>(buffer[1]) << 8);
    int16_t raw_y = static_cast<int16_t>(buffer[2]) | (static_cast<int16_t>(buffer[3]) << 8);
    int16_t raw_z = static_cast<int16_t>(buffer[4]) | (static_cast<int16_t>(buffer[5]) << 8);
    
    // sensitivity_ is in g/LSB, multiply by 9.80665 for m/s²
    Vector3f linear_acceleration(
        static_cast<float>(raw_x) * sensitivity_ * 9.80665f,
        static_cast<float>(raw_y) * sensitivity_ * 9.80665f,
        static_cast<float>(raw_z) * sensitivity_ * 9.80665f
    );
    
    return AccelData(linear_acceleration, to_ms_since_boot(get_absolute_time()));
}
