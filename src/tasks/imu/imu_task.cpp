// imu_task.cpp --> Both accel and gyro tasks
#include "tasks/imu/imu_task.hpp"
#include <cstdio>

using namespace config;

IMUTask::IMUTask(QueueHandle_t data_queue) 
    : I2CSensorTask("IMU", tasks::IMU_STACK_SIZE, tasks::IMU_PRIORITY, data_queue)
    , gyro_(i2c1), accel_(i2c1) {
    
    printf("[IMU] Task created \n");
    printf("[IMU] Gyro initialized.  I2C device address: 0x%02X on port %p\n", gyro_.get_address(), (void*)i2c1);
    printf("[IMU] Accel initialized. I2C device address: 0x%02X on port %p\n", accel_.get_address(), (void*)i2c1);
}


// GYRO ----
bool IMUTask::initialize_gyro() {
    L3GD20::Config gyro_config;
    gyro_config.range = L3GD20::Range::DPS_500;
    gyro_config.output_rate = L3GD20::OutputRate::HZ_95;
    
    if (!gyro_.initialize_config(gyro_config)) {
        printf("[IMU] FAILED to initialize gyroscope\n");
        return false;
    }
        
    printf("[IMU] Gyroscope initialized successfully\n");
    return true;
}

void IMUTask::process_gyro_data(SensorData *sensor_data) {
    auto gyro_data = gyro_.read_gyro();

    sensor_data->gyro = gyro_data; 
}

// ACCEL ----
bool IMUTask::initialize_accel() {
    LSM303D_Accel::Config accel_config;
    accel_config.range = LSM303D_Accel::Range::G_2;
    accel_config.output_rate = LSM303D_Accel::OutputRate::HZ_100;
    
    if (!accel_.initialize_config(accel_config)) {
        printf("[IMU] FAILED to initialize accelerometer\n");
        return false;
    }
        
    printf("[IMU] Accelerometer initialized successfully\n");
    return true;
}

void IMUTask::process_accel_data(SensorData *sensor_data) {
    auto accel_data = accel_.read_accel();

    sensor_data->accel = accel_data;   
}

// -------

void IMUTask::run() {
    printf("[IMU] Task started - Sample rate: %lu ms\n", tasks::IMU_SAMPLE_MS);
    
    if (!initialize_gyro()) {
        printf("[IMU] WARNING: Gyroscope starting in degraded mode\n");
    }

    if (!initialize_accel()) {
        printf("[IMU] WARNING: Accelerometer starting in degraded mode\n");
    }
    
    while (true) {
        SensorData sensor_data{};  // reset

        process_gyro_data(&sensor_data);
        process_accel_data(&sensor_data);

        sensor_data.sequence_number++;  // TODO: Change

        // TODO: Add error handling
        xQueueSend(data_queue_, &sensor_data, 0);
        delay(tasks::IMU_SAMPLE_MS);
    }
}
