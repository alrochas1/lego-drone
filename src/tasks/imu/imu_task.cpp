// imu_task.cpp --> Both accel and gyro tasks
#include "tasks/imu/imu_task.hpp"
#include <cstdio>

using namespace config;

IMUTask::IMUTask(QueueHandle_t data_queue, QueueHandle_t status_queue) 
    : I2CSensorTask("IMU", tasks::IMU_STACK_SIZE, tasks::IMU_PRIORITY, data_queue, status_queue)
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

void IMUTask::process_gyro_data(IMUData *sensor_data) {
    auto gyro_data = gyro_.read_gyro();

    // Rotate data to body frame
    gyro_data.angular_velocity = 
        transform_to_body_frame(
            gyro_data.angular_velocity,
            imu_orientation);

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

void IMUTask::process_accel_data(IMUData *sensor_data) {
    auto accel_data = accel_.read_accel();

    // Rotate data to body frame
    accel_data.linear_acceleration = 
        transform_to_body_frame(
            accel_data.linear_acceleration,
            imu_orientation);

    sensor_data->accel = accel_data;   
}

// -------

Vector3f IMUTask::transform_to_body_frame(
    const Vector3f& sensor,
    const IMUOrientation& orientation)
{
    auto get_axis = [&](SensorAxis axis) {
        switch (axis) {
            case SensorAxis::X: return sensor.x;
            case SensorAxis::Y: return sensor.y;
            case SensorAxis::Z: return sensor.z;
        }

        return 0.0f;
    };

    return {
        get_axis(orientation.forward.source) * orientation.forward.sign,
        get_axis(orientation.right.source)   * orientation.right.sign,
        get_axis(orientation.down.source)    * orientation.down.sign
    };
}

void IMUTask::run() {
    printf("[IMU] Task started - Sample rate: %lu ms\n", tasks::IMU_SAMPLE_MS);
    
    if (!initialize_gyro()) {
        printf("[IMU] WARNING: Gyroscope starting in degraded mode\n");
    }

    if (!initialize_accel()) {
        printf("[IMU] WARNING: Accelerometer starting in degraded mode\n");
    }
    
    while (true) {
        IMUData     sensor_data{};  // reset
        IMUStatus   status_data{};  // reset

        process_gyro_data(&sensor_data);
        process_accel_data(&sensor_data);

        sensor_data.sequence_number++;  // TODO: Change

        // Fill status data
        status_data.valid = sensor_data.is_complete();
        status_data.sequence_number = sensor_data.sequence_number;
        // TODO: Add timestamp to status data

        // Send data to other tasks
        // TODO: Add error handling
        xQueueOverwrite(data_queue_,     &sensor_data);
        xQueueOverwrite(status_queue_,   &status_data);

        delay(tasks::IMU_SAMPLE_MS);
    }
}
