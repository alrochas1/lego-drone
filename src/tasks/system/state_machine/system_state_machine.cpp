// system_state_machine.cpp

#include "system_state_machine.hpp"
#include "drone_project/config/project_config.hpp"

using namespace config;

SystemStateMachine::SystemStateMachine()
    : state_(SystemState::INIT),
      init_count_(0),
      imu_fail_count_(0),
      rc_fail_count_(0),
      throttle_low_count_(0)
{
}


void SystemStateMachine::update_state(const SystemInputs& in) {
    
    switch (state_) {
    case SystemState::INIT:
        // Initial state: System is starting up
        // Transitions:
        // - To USB if USB is connected
        // - To DISARMED if IMU is OK during INIT_COUNTDOWN cycles
        // - To ERROR if IMU not OK after IMU_COUNTDOWN cycles
        // - Stay in INIT otherwise
        if (in.usb_connected)
        {
            state_ = SystemState::USB;
        } 
        else if (in.imu_ok) 
        {
            if(init_count_ > tasks::INIT_COUNTDOWN)
            {
                state_ = SystemState::DISARMED;
                init_count_ = 0;        // reset
                imu_fail_count_ = 0;    // reset
            }
            else
            {
                init_count_++;
                imu_fail_count_ = 0;    // reset
            }
        }
        else
        {
            if(imu_fail_count_ > tasks::IMU_COUNTDOWN)
            {
                state_ = SystemState::ERROR;
            }
            else
            {
                imu_fail_count_++;
                imu_fail_count_ = 0;    // reset
            }
        }
        // If no transition, stay in INIT
        break;

    case SystemState::USB:
        // USB mode: System is connected via USB, not on battery
        // No transition, stay in USB
        break;

    case SystemState::DISARMED:
        // Disarmed state: Motors are disabled, system is safe
        // Transitions:
        // - To ERROR if IMU fails for IMU_COUNTDOWN cycles
        // - To FAILSAFE if RC fails for RC_COUNTDOWN cycles
        // - To ARMED if throttle low
        // - Stay in DISARMED otherwise
        if (!in.imu_ok)
        {
            if (imu_fail_count_ > tasks::IMU_COUNTDOWN)
            {
                state_ = SystemState::ERROR;
                imu_fail_count_ = 0;  // reset
            }
            else
            {
                imu_fail_count_++;
            }
        }
        else
        {
            imu_fail_count_ = 0;  // reset when condition clears
        }
        
        if (!in.rc_ok)
        {
            if (rc_fail_count_ > tasks::RC_COUNTDOWN)
            {
                state_ = SystemState::FAILSAFE;
                rc_fail_count_ = 0;  // reset
            }
            else
            {
                rc_fail_count_++;
            }
        } 
        else
        {
            rc_fail_count_ = 0;  // reset when condition clears
        }
        
        if (in.throttle < 0.05f) {
            state_ = SystemState::ARMED;
        }
        // If no transition, stay in DISARMED
        break;

    case SystemState::ARMED:
        // Armed state: System is ready to fly, motors can be activated
        // Transitions:
        // - To FAILSAFE if RC fails for RC_COUNTDOWN cycles
        // - To ERROR if IMU fails for IMU_COUNTDOWN cycles
        // - To FLIGHT if throttle increased
        // - Stay in ARMED otherwise
        if (!in.rc_ok) 
        {
            if (rc_fail_count_ > tasks::RC_COUNTDOWN)
            {
                state_ = SystemState::FAILSAFE;
                rc_fail_count_ = 0;  // reset
            }
            else
            {
                rc_fail_count_++;
            }
        } 
        else 
        {
            rc_fail_count_ = 0;  // reset when condition clears
        }
        
        if (!in.imu_ok)
        {
            if (imu_fail_count_ > tasks::IMU_COUNTDOWN)
            {
                state_ = SystemState::ERROR;
                imu_fail_count_ = 0;  // reset
            } 
            else 
            {
                imu_fail_count_++;
            }
        } 
        else 
        {
            imu_fail_count_ = 0;  // reset when condition clears
        }
        
        if (in.throttle > 0.1f) 
        {
            state_ = SystemState::FLIGHT;
        } 
        // If no transition, stay in ARMED
        break;

    case SystemState::FLIGHT:
        // Flight state: Drone is actively flying
        // Transitions:
        // - To FAILSAFE if RC fails for RC_COUNTDOWN cycles
        // - To ERROR if IMU fails for IMU_COUNTDOWN cycles
        // - To ARMED if throttle lowered for THROTTLE_LOW_COUNTDOWN cycles
        // - Stay in FLIGHT otherwise
        if (!in.rc_ok) 
        {
            if (rc_fail_count_ > tasks::RC_COUNTDOWN) 
            {
                state_ = SystemState::FAILSAFE;
                rc_fail_count_ = 0;  // reset
            } 
            else 
            {
                rc_fail_count_++;
            }
        } 
        else 
        {
            rc_fail_count_ = 0;  // reset when condition clears
        }
        
        if (!in.imu_ok) 
        {
            if (imu_fail_count_ > tasks::IMU_COUNTDOWN) 
            {
                state_ = SystemState::ERROR;
                imu_fail_count_ = 0;  // reset
            } 
            else 
            {
                imu_fail_count_++;
            }
        } 
        else 
        {
            imu_fail_count_ = 0;  // reset when condition clears
        }
        
        if (in.throttle < 0.05f) 
        {
            if (throttle_low_count_ > tasks::THROTTLE_LOW_COUNTDOWN) 
            {
                state_ = SystemState::ARMED;
                throttle_low_count_ = 0;  // reset
            } 
            else 
            {
                throttle_low_count_++;
            }
        } 
        else 
        {
            throttle_low_count_ = 0;  // reset when condition clears
        }
        // If no transition, stay in FLIGHT
        break;

    case SystemState::FAILSAFE:
        // Failsafe state: Emergency mode, system attempts to land or stabilize
        // Transitions:
        // - To DISARMED if both RC and IMU recover
        // - Stay in FAILSAFE otherwise
        if (in.rc_ok && in.imu_ok) 
        {
            state_ = SystemState::DISARMED;
        }
        // If no transition, stay in FAILSAFE
        break;

    case SystemState::ERROR:
        // Error state: Critical failure, system is in safe mode
        // Transitions:
        // - Only manual reset can exit this state
        // - Stay in ERROR
        // No transitions implemented
        break;

    default:
        // Unknown state, set to ERROR for safety
        state_ = SystemState::ERROR;
        break;
    }
}


SystemState SystemStateMachine::getState() const{
    return state_;
}