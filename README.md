# Lego Drone with Raspberry Pi Pico
Personal project to build a quadcopter using a LEGO Technic custom frame, a Raspberry Pi Pico (RP2040) and FreeRTOS,
as a way to explore embedded software architecture and flight control using intentionally non-optimal hardware.

## Features
* FreeRTOS running on RP2040
* CMake-based build system
* Modular task architecture
* Queue-based inter-task communication
* Centralized configuration

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

Then export the Pico SDK path:
```
export PICO_SDK_PATH=/route_to/pico-sdk
```

## 3. Build the project

Open the project root folder.

Select the toolchain `GCC 10.3.1 arm-none-eabi`

Press the **Build** button at the bottom of VSCode.

This will:

- Configure CMake
- Compile the project
- Generate the firmware file

After a successful build, the output file will be located at `./build/src/freeRTOS_Lego_Drone.uf2`

## 4. Flash the firmware

1. Hold the **BOOTSEL** button on the Raspberry Pi Pico.
2. Connect it to your PC via USB.
3. A new USB mass storage device will appear.
4. Drag and drop the `.uf2` file into it.

The Pico will reboot automatically and run the firmware.