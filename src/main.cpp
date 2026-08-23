// main.cpp
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "drone_project/config/project_config.hpp"
#include "drone_project/types/system_data.hpp"

#include "tasks/system/system_task.hpp"

#include "tasks/led/led_task.hpp"
#include "tasks/log/log_task.hpp"

#include "tasks/imu/imu_task.hpp"
#include "tasks/ir/ir_task.hpp"

#include "tasks/motor/motor_task.hpp"
#include "tasks/control/control_task.hpp"

// Sim tasks (for testing without hardware)
#include "tasks/sim/rc_sim_task.hpp"
#include "tasks/sim/imu_sim_task.hpp"


using namespace config;

// Create the different queues used by the system
SystemQueues create_queues() {
    SystemQueues queues;

    // System queue for system monitor and logging
    queues.snapshot_queue = xQueueCreate(queues::SYSTEM_QUEUE_LENGTH, sizeof(SystemSnapshot));

    // Imput Queues
    queues.rc_queue     = xQueueCreate(queues::SYSTEM_QUEUE_LENGTH, sizeof(RCCommand));
    queues.imu_queue    = xQueueCreate(queues::SENSOR_QUEUE_LENGTH, sizeof(SensorData));
    
    // Control output queue
    queues.motor_queue  = xQueueCreate(queues::SYSTEM_QUEUE_LENGTH, sizeof(MotorCommands));

    // Add error handling for queue creation
    if (!queues.snapshot_queue ||
        !queues.rc_queue ||
        !queues.imu_queue ||
        !queues.motor_queue) {
    printf("QUEUE INIT FAILED\n");
    while(true);
}
    
    return queues;
}


void common_main() {
    stdio_init_all();

    // Wait for the USB serial connection
    sleep_ms(2500);
    printf("\n=== System Starting ===\n");
    printf("Compiled: %s %s\n", __DATE__, __TIME__);
}


int start_tasks(bool success) {

    if (!success) {
        printf("ERROR: Failed to start one or more tasks\n");
        return 1;
    }
    
    printf("All tasks started successfully\n");
    printf("System running...\n");
    
    // Start scheduler
    vTaskStartScheduler();
    
    // We should never get here
    while (true) {
        tight_loop_contents();
    }
    
    return 0;
}


int drone_main() {

    // For testing individual tasks without the full setup
    RunMode running_mode = RunMode::IMU_SIM;
    
    common_main();

    
    // Create queues
    SystemQueues queues = create_queues();


    // Create tasks (TODO: Add structure)
    SystemStateTask system_state_task(queues.imu_queue, queues.rc_queue, queues.snapshot_queue);
    LedTask led_task(queues.snapshot_queue);
    LogTask log_task(queues.snapshot_queue, queues.motor_queue);

    // Imput tasks (TODO: Improve this)
    Task* imu_task = nullptr;
    if (running_mode == RunMode::IMU_SIM || running_mode == RunMode::SIMULATION) {
        imu_task = new IMUSimTask(queues.imu_queue);
    } 
    else {
        imu_task = new IMUTask(queues.imu_queue);
    }   

    Task* rc_task = nullptr;
    if (running_mode == RunMode::RC_SIM || running_mode == RunMode::SIMULATION) {
        rc_task = new RCSimTask(queues.rc_queue);
    } else {
        rc_task = new IRTask(pins::IR_PIN, queues.rc_queue);
    }

    // Control task
    ControlTask control_task(queues.snapshot_queue, queues.motor_queue);
    MotorTask motor_task(queues.motor_queue);


    // TODO: Implement
    // Start tasks
    bool success = led_task.start() && 
                   log_task.start() &&
                   system_state_task.start() &&
                   imu_task->start() &&
                   rc_task->start() &&
                   control_task.start() &&
                   motor_task.start();
    return start_tasks(success);
}


// ########################
// ######### MAIN #########
// ########################

int main() {
    return drone_main();
}
