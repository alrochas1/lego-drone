// log_task.cpp
#include "tasks/log/log_task.hpp"
#include "config/project_config.hpp"
#include <cstdio>

using namespace config;

LogTask::LogTask(QueueHandle_t snapshot_queue, QueueHandle_t motor_queue)
    : Task("LOG", tasks::LOG_STACK_SIZE, tasks::LOG_PRIORITY),
      snapshot_queue_(snapshot_queue),
      motor_queue_(motor_queue) {

    printf("[LOG] Task created\n");
}

void LogTask::run() {

    SystemSnapshot snap;

    while (true) {

        printf("\n=== SYSTEM SNAPSHOT ===\n");
        if (xQueuePeek(snapshot_queue_, &snap, 0) == pdPASS) {

            printf("[TS] %lu ms\n", snap.timestamp_ms);

            // STATE
            printf("[STATE] %s\n", state_to_string(snap.state));

            // RC
            printf("[RC] Valid:%s T:%.2f R:%.2f P:%.2f Y:%.2f\n",
                snap.rc.valid ? "OK" : "FAIL",
                snap.rc.throttle,
                snap.rc.roll,
                snap.rc.pitch,
                snap.rc.yaw);

            // IMU
            printf("[GYRO] Valid:%s | %.2f %.2f %.2f\n",
                snap.imu.has_gyro() ? "OK" : "FAIL",
                snap.imu.gyro.angular_velocity.x,
                snap.imu.gyro.angular_velocity.y,
                snap.imu.gyro.angular_velocity.z);

            printf("[ACCEL] Valid:%s | %.2f %.2f %.2f\n",
                snap.imu.has_accel() ? "OK" : "FAIL",
                snap.imu.accel.linear_acceleration.x,
                snap.imu.accel.linear_acceleration.y,
                snap.imu.accel.linear_acceleration.z);
        }

        // Log motor commands
        MotorCommands motor_cmds;
        if (xQueuePeek(motor_queue_, &motor_cmds, 0) == pdPASS) {
            printf("[MOTORS] M1=%u M2=%u M3=%u M4=%u\n",
                motor_cmds.m1, motor_cmds.m2, motor_cmds.m3, motor_cmds.m4);
        }

        printf("=======================\n");
        
        delay(tasks::LOG_PRINT_MS);  // 2 Hz logging
    }
}

const char* LogTask::state_to_string(SystemState state) {
    switch (state) {
        case SystemState::INIT:      return "INIT";
        case SystemState::USB:       return "USB";
        case SystemState::DISARMED:  return "DISARMED";
        case SystemState::ARMED:     return "ARMED";
        case SystemState::FLIGHT:    return "FLIGHT";
        case SystemState::FAILSAFE:  return "FAILSAFE";
        case SystemState::ERROR:     return "ERROR";
        default:                     return "UNKNOWN";
    }
}