# Changelog

All notable changes to this project will be documented in this file.

## Current Status: Development

- [ ] Suitable for flight

### Software Arquitecture
- [x] RP2040 + FreeRTOS Architecture
- [x] IMU L3GD20 + LSM303D Driver
- [x] PWM Motor Driver
- [ ] ELRS RC Driver
- [x] Safety State Machine
- [x] Flight Controller with Cascaded PID

### Testing
- [x] Continuous Integration (CI)
- [x] Firmware Test
- [x] Unit Testing for State Machine
- [x] Unit Testing for Control

### Hardware
- [ ] Main PCB for electronics
- [x] Prototype motor-driver boards 
- [ ] PCB for Motor Driver


## [0.2.0] - 2026-08-26

### Added

#### Testing and Automation
- Added GoogleTest and CTest infrastructure. Added GitHub Actions workflow
- First implementation of units test
- First implementation of a firmware test
- `run_tests.sh` script for configuring, building and running the test suite
- `build_firmware.sh` script for reproducible firmware builds, including a clean-build option
- Documentation and state diagram for the system state machine

#### Project Structure
- Reorganized task and driver headers and sources by responsibility
- Separate hardware pin configuration
- Separate system task for state management and snapshot publication
- Updated the CMake configuration to support the reorganized source tree and standalone tests


### Known Limitations

- IR task is not yet implemented (commented out in `main.cpp`)
- RC driver is not yet implemented
- RC input pins are not yet configured for a physical receiver
- Magnetometer support is incomplete in the LSM303D driver
- Flight control and PID stabilization are not yet implemented


## [0.1.0] - 2026-06-02

### Added

#### Core Infrastructure
- FreeRTOS integration with Raspberry Pi Pico
- CMake-based build system with Pico SDK support
- Task scheduler with configurable stack sizes and priorities
- Queue-based inter-task communication system
- Centralized configuration management

#### Hardware Drivers
- Raspberry Pi Pico (RP2040)
- I2C device abstraction layer with error handling
- L3GD20 3-axis gyroscope driver
- LSM303D 3-axis accelerometer driver
- Motor driver with PWM support (4 motor channels, tested with a 8520 motor and custom drivers)
- USB detection support

#### System Tasks
- **System Task**: Core system monitoring and state management
- **LED Task**: Status indicator control
- **Log Task**: System logging and debug output via USB serial
- **IMU Task**: Inertial Measurement Unit sensor reading and processing
- **Motor Task**: Motor command execution and PWM control
- **Control Task**: Flight control logic and stabilization (not implemented)
- **RC Simulator Task**: Remote control input simulation for testing (no hardware required)
- **IMU Simulator Task**: IMU sensor simulation for testing (no hardware required)


#### Documentation
- README.md with compilation and flashing instructions
- Inline code documentation and comments


### Known Limitations

- IR task not yet implemented (commented out in main.cpp)
- RC driver is not yet implemented
- RC input pins not yet configured
- Magnetometer support incomplete in LSM303D driver
- Flight control and PID stabilization are not yet implemented