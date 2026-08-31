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
            printf("[RC] Valid:%s T:%.2f \n",
                snap.rc.valid ? "OK" : "FAIL",
                snap.rc.throttle);

            // IMU
            printf("[GYRO] Valid:%s | t=%lu\n",
                snap.imu.valid ? "OK" : "FAIL",
                snap.imu.timestamp_ms);
            
            printf("[ACCEL] Valid:%s | t=%lu\n",
                snap.imu.valid ? "OK" : "FAIL",
                snap.imu.timestamp_ms);
        }

        // Log motor commands (this shouldnt be here, is for debugging)
        MotorCommands motor_cmds;
        if (xQueuePeek(motor_queue_, &motor_cmds, 0) == pdPASS) {
            printf("[MOTORS]");
            for (uint8_t i = 0; i < motor_cmds.num_motors; ++i) {
                printf(" M%u=%u", static_cast<unsigned>(i + 1), motor_cmds.motor[i]);
            }
            printf("\n");
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