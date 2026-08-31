// comms_data.hpp

#pragma once

#include <cstdint>

struct RCCommand {
    bool  valid;      // ON/OFF
    float throttle;
    float roll;
    float pitch;
    float yaw;
};


struct RCStatus {
    bool  valid;      // ON/OFF
    float throttle;
};


// This is for testing the RC input without the actual RC controller
struct IREdge {
    uint32_t time;
    bool level;
};

