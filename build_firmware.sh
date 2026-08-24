#!/bin/bash

set -e

BUILD_DIR="build"

# Check if PICO_SDK_PATH is set
if [ -z "$PICO_SDK_PATH" ]; then
    echo "Error: PICO_SDK_PATH is not set"
    exit 1
fi

# Clean build directory if --clean argument is provided
if [[ "$1" == "--clean" ]]; then
    rm -rf "$BUILD_DIR"
fi

# Create build directory and run CMake
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$BUILD_DIR"


test -f build/src/freeRTOS_Lego_Drone.uf2