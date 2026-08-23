// system_state_task.cpp
#include "tasks/common/task_wrapper.hpp"

#include "drone_project/config/project_config.hpp"
#include "drone_project/types/system_data.hpp"
#include "drone_project/types/comms_data.hpp"

using namespace config;

class SystemStateTask : public Task {
private:

    void update_state(SystemState* state, const SystemInputs& in);

    QueueHandle_t imu_queue_;
    QueueHandle_t rc_queue_;
    QueueHandle_t state_queue_;

    SystemState state_;

    // Counters for state machine countdowns
    uint32_t init_count_;
    uint32_t imu_fail_count_;
    uint32_t rc_fail_count_;
    uint32_t throttle_low_count_;

public:
    SystemStateTask(QueueHandle_t imu_q,
                    QueueHandle_t rc_q,
                    QueueHandle_t state_q);

protected:
    void run() override;
};