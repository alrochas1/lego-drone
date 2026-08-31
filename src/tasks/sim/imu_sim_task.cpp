// imu_sim_task.cpp
#include "tasks/sim/imu_sim_task.hpp"

using namespace config;

IMUSimTask::IMUSimTask(QueueHandle_t data_queue, QueueHandle_t status_queue)
    : Task("IMUSim", tasks::IMU_STACK_SIZE, tasks::IMU_PRIORITY),
      data_queue_(data_queue), status_queue_(status_queue) {

    printf("[IMUSim] Task created\n");
}

void IMUSimTask::run() {

    printf("[IMUSim] Task started - Sample rate: %lu ms\n", tasks::IMU_SAMPLE_MS);

    // static uint32_t seq = 0;

    while (true) {

        IMUData     data{};
        IMUStatus   status{};

        auto timestamp_ms = to_ms_since_boot(get_absolute_time());
        // data.sequence_number = seq++;    // TODO: Check

        // ---- GYRO ----
        Vector3f angular_velocity;
        angular_velocity.x = 0.0f;
        angular_velocity.y = 0.0f;
        angular_velocity.z = 1.5f;
        data.gyro = GyroData(angular_velocity, timestamp_ms);

        // ---- ACCEL ----
        Vector3f linear_acceleration;
        linear_acceleration.x = 0.0f;
        linear_acceleration.y = 0.0f;
        linear_acceleration.z = 9.81f;
        data.accel = AccelData(linear_acceleration, timestamp_ms);

        xQueueSend(data_queue_,     &data, 0);
        xQueueSend(status_queue_,   &status, 0);
        delay(tasks::IMU_SAMPLE_MS);
    }
}