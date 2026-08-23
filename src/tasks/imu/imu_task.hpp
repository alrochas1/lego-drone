// imu_task.hpp
#pragma once
#include "FreeRTOS.h"
#include <queue.h>

#include <drone_project/types/sensor_data.hpp>
#include "drone_project/config/project_config.hpp"

#include "tasks/common/task_wrapper.hpp"
#include <drivers/l3gd20.hpp>
#include <drivers/lsm303d_accel.hpp>


class IMUTask : public I2CSensorTask {
private:
    L3GD20 gyro_;
    LSM303D_Accel accel_;
    // Add mag if needed
    
    bool initialize_gyro();
    bool initialize_accel();
    void process_gyro_data(SensorData *sensor_data);
    void process_accel_data(SensorData *sensor_data);
    // void handle_read_error();

public:
    explicit IMUTask(QueueHandle_t data_queue);
    
    IMUTask(const IMUTask&) = delete;
    IMUTask& operator=(const IMUTask&) = delete;
    
    void run() override;
    
};
