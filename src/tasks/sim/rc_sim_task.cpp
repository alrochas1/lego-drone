// rc_sim_task.cpp
#include "tasks/sim/rc_sim_task.hpp"
#include "drone_project/config/project_config.hpp"

using namespace config;

RCSimTask::RCSimTask(QueueHandle_t queue)
    : Task("RCSim", tasks::RC_STACK_SIZE, tasks::RC_PRIORITY),
      rc_queue_(queue) {

    last_cmd_ = {
        .valid = false,
        .throttle = 0.0f,
        .roll = 0.0f,
        .pitch = 0.0f,
        .yaw = 0.0f
    };

    printf("[RCSim] Task created\n");
}

void RCSimTask::sim() {
    // Simulate RC input (for testing)
    last_cmd_.valid = true;
    last_cmd_.throttle = 0.5f; // 50% throttle
    last_cmd_.roll = 0.0f;
    last_cmd_.pitch = 0.0f;
    last_cmd_.yaw = 0.0f;
}

void RCSimTask::run() {

    printf("[RCSim] Task started\n");

    while (true) {

        sim();

        xQueueOverwrite(rc_queue_, &last_cmd_);

        delay(tasks::RC_UPDATE_MS);
    }
}