# Lego Drone with Raspberry Pi Pico
Personal project to build a quadcopter using a LEGO Technic custom frame, a Raspberry Pi Pico (RP2040) and FreeRTOS,
as a way to explore embedded software architecture and flight control using intentionally non-optimal hardware.

This is an experimental project. The objective is not to build a competitive or production-grade flight controller, using prebuilt flight-controller hardware such as PX4 or a carbon-fiber frame.

This project is work in progress. More detailed documentation is available in the doc/ directory and the changelog.

<!-- <img src="doc/img/pcb/full_drone.png" alt="Drone Picture" width="600px" /> -->

## Features
* FreeRTOS running on RP2040
* Flight Controller based in a Cascaded PID and an Attitude estimator
* CMake-based build system
* Modular task architecture
* Queue-based inter-task communication
* Centralized configuration
* System state machine for initialization, safety and flight states
* Unit tests for system state transitions with GoogleTest, with automated workflows with GitHub Actions
* Simple Hardware simulation tasks for IMU and RC inputs


# How to compile

## 1. Install prerequisites (Linux)

Make sure you have:

- cmake
- gcc-arm-none-eabi
- git

## 2. Clone the Pico SDK

Clone the Pico SDK repo:

```
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init
```

Then export the Pico SDK path (or set it in your bash):
```
export PICO_SDK_PATH=/route_to/pico-sdk
```

## 3. Build the project

Open the project root folder.

Select the toolchain `GCC 10.3.1 arm-none-eabi`

Press the **Build** button at the bottom of VSCode, or run `./build_firmware.sh` from the project root.

This will:

- Configure CMake
- Compile the project
- Generate the firmware file

After a successful build, the output file will be located at `./build/src/freeRTOS_Lego_Drone.uf2`

## 4. Automated checks

GitHub Actions runs the unit tests for pushes to `main` and `devel`, and for pull requests.

### Run the tests

The test script configures a separate CMake build, downloads GoogleTest when needed, builds the unit-test target and
runs it with CTest:

```
./run_tests.sh
```

## 5. Flash the firmware

1. Hold the **BOOTSEL** button on the Raspberry Pi Pico.
2. Connect it to your PC via USB.
3. A new USB mass storage device will appear.
4. Drag and drop the `.uf2` file into it.

The Pico will reboot automatically and run the firmware.