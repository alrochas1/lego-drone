// lsm303d_accel.hpp
#pragma once
#include "i2c_device.hpp"
#include <drone_project/types/sensor_data.hpp>
#include "drone_project/config/project_config.hpp"

class LSM303D_Accel : public I2CDevice {
public:
    enum class Range { G_2, G_4, G_6, G_8, G_16 };
    enum class OutputRate { HZ_50, HZ_100, HZ_200, HZ_400 };
    
    struct Config { 
        Range range{Range::G_2};
        OutputRate output_rate{OutputRate::HZ_100};
    };

private:
    Config config_;
    float sensitivity_{0.0f};
        
    void calculate_sensitivity();
    bool set_ctrl_reg1();
    bool set_ctrl_reg2();

public:
    LSM303D_Accel(i2c_inst_t* i2c = i2c_default);
    
    bool initialize() override;
    bool initialize_config(const Config& config);
    
    Config get_config() const { return config_; }
    
    AccelData read_accel();
    
    static constexpr const char* get_name() { return "LSM303D_Accel"; }
};
