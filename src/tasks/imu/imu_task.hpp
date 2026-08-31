// imu_task.hpp
#pragma once
#include "FreeRTOS.h"
#include <queue.h>

#include <types/sensor_data.hpp>
#include "config/project_config.hpp"

#include "tasks/common/task_wrapper.hpp"
#include <drivers/sensors/l3gd20.hpp>
#include <drivers/sensors/lsm303d_accel.hpp>


class IMUTask : public I2CSensorTask {
private:
    L3GD20 gyro_;
    LSM303D_Accel accel_;
    // Add mag if needed
    
    bool initialize_gyro();
    bool initialize_accel();
    void process_gyro_data(IMUData *sensor_data);
    void process_accel_data(IMUData *sensor_data);

    Vector3f transform_to_body_frame(
        const Vector3f& sensor,
        const IMUOrientation& orientation);

    // void handle_read_error();

public:
    IMUTask(QueueHandle_t data_queue, QueueHandle_t status_queue);

    IMUTask(const IMUTask&) = delete;
    IMUTask& operator=(const IMUTask&) = delete;
    
    void run() override;
    
};
