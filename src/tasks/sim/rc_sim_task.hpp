// rc_sim_task.hpp
#pragma once
#include "tasks/common/task_wrapper.hpp"
#include "types/comms_data.hpp"
#include <queue.h>

class RCSimTask : public Task {
private:
    QueueHandle_t data_queue_;
    QueueHandle_t status_queue_;

    RCCommand last_cmd_;

    void sim();

    // decoder state
    bool started_ = false;
    uint32_t last_time_ = 0;

public:
    RCSimTask(QueueHandle_t data_queue, QueueHandle_t status_queue);

protected:
    void run() override;
};