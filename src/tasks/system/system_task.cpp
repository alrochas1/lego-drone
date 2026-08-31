// system_task.cpp
#include "system_task.hpp"
#include "config/hardware_config.hpp"

#define SIM_BATTERY 0 // For testing without battery (USB mode). Be careful if the motors are connected!

using namespace config;


SystemStateTask::SystemStateTask(QueueHandle_t imu_q,
                                 QueueHandle_t rc_q,
                                 QueueHandle_t state_q)
    : Task("STATE", 512, 3),
      imu_queue_(imu_q),
      rc_queue_(rc_q),
      state_queue_(state_q)
      {
        printf("[SYSTEM STATE] Task created\n");
      }


void SystemStateTask::run() {

    SystemInputs    inputs{};
    IMUStatus       imu{};
    RCStatus        rc{};
    SystemSnapshot  snap{};

    printf("[SYSTEM STATE] Task started - Update interval: %lu ms\n", tasks::SYSTEM_UPDATE_MS);

    while (true) {

        snap.timestamp_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;

        // USB
        inputs.usb_connected = gpio_get(pins::USB_PIN);
        #if SIM_BATTERY
        inputs.usb_connected = false; // TESTING
        #endif

        // IMU
        if (xQueueReceive(imu_queue_, &imu, 0) == pdPASS) {
            inputs.imu_ok = imu.valid;
            snap.imu = imu;
        }

        // RC
        if (xQueueReceive(rc_queue_, &rc, 0) == pdPASS) {
            inputs.rc_ok    = rc.valid;
            inputs.throttle = rc.throttle;

            snap.rc = rc;
        } else {
            inputs.rc_ok = false;
        }

        // FSM
        state_machine_.update_state(inputs);
        snap.state = state_machine_.getState();

        xQueueOverwrite(state_queue_, &snap);

        delay(tasks::SYSTEM_UPDATE_MS);
    }
}

