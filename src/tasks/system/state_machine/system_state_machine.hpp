// system_state_machine.hpp

#pragma once

#include "types/system_data.hpp"

class SystemStateMachine
{
public:
    SystemStateMachine();

    void update_state(const SystemInputs& inputs);

    SystemState getState() const;

private:
    SystemState state_;

    uint32_t init_count_;
    uint32_t imu_fail_count_;
    uint32_t rc_fail_count_;
    uint32_t throttle_low_count_;
};