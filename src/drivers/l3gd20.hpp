// l3gd20.hpp
#pragma once
#include "i2c_device.hpp"
#include <drone_project/types/sensor_data.hpp>
#include "drone_project/config/project_config.hpp"


class L3GD20 : public I2CDevice {
public:
    enum class Range { DPS_250, DPS_500, DPS_2000 };
    enum class OutputRate { HZ_95, HZ_190, HZ_380, HZ_760 };
    
    struct Config {
        Range range{Range::DPS_250};
        OutputRate output_rate{OutputRate::HZ_95};
    };

private:
    Config config_;
    float sensitivity_dps_digit_{0.0f};
        
    void calculate_sensitivity();
    bool set_ctrl_reg1();
    bool set_ctrl_reg4();

public:
    L3GD20(i2c_inst_t* i2c = i2c_default);
    
    bool initialize() override;
    bool initialize_config(const Config& config);
    
    Config get_config() const { return config_; }
    
    GyroData read_gyro();
    
    static constexpr const char* get_name() { return "L3GD20"; }
};