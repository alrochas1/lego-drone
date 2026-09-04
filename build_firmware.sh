#!/bin/bash

set -e

BUILD_DIR="build"

# Check if PICO_SDK_PATH is set
if [ -z "$PICO_SDK_PATH" ]; then
    echo "Error: PICO_SDK_PATH is not set"
    exit 1
fi

# Process optional arguments
BOARD_W=1
BOARD="pico_w"
for arg in "$@"; do
    case "$arg" in
        --clean)
            rm -rf "$BUILD_DIR"
            ;;
        --no-wifi)
            BOARD_W=0
            BOARD="pico"
            ;;
        *)
            echo "Error: unknown argument: $arg"
            exit 1
            ;;
    esac
done

# Create build directory and run CMake
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DPICO_BOARD="$BOARD" -DBOARD_W="$BOARD_W"
cmake --build "$BUILD_DIR"


test -f build/src/freeRTOS_Lego_Drone.uf2