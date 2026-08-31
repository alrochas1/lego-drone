// motor_task.cpp
#include "tasks/motor/motor_task.hpp"

using namespace config;

MotorTask::MotorTask(QueueHandle_t motor_queue)
    : Task("MOTOR", tasks::MOTOR_STACK_SIZE, tasks::MOTOR_PRIORITY),
    motor_queue_(motor_queue),
    m1_(pins::M1_PIN),
    m2_(pins::M2_PIN),
    m3_(pins::M3_PIN),
    m4_(pins::M4_PIN) {

    m1_.init();
    m2_.init();
    m3_.init();
    m4_.init();

    printf("[MOTOR] Task created \n");
}

void MotorTask::run() {
    MotorCommands commands{};

    printf("[MOTOR] Task started - Update interval: %lu ms\n", tasks::MOTOR_UPDATE_MS);

    while (true) {
        // Read motor commands from queue
        if (xQueuePeek(motor_queue_, &commands, 0) == pdPASS) {
            m1_.set_power(commands.motor[0]);
            m2_.set_power(commands.motor[1]);
            m3_.set_power(commands.motor[2]);
            m4_.set_power(commands.motor[3]);
        }

        delay(tasks::MOTOR_UPDATE_MS);
    }
}