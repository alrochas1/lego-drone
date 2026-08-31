cmake_minimum_required(VERSION 3.16)

# Define the test project
project(drone_tests LANGUAGES CXX)

# Use FetchContent to download and configure GoogleTest framework
include(FetchContent)

# Configure GoogleTest to be fetched from GitHub
# TODO: Improve this - consider using something more robust
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

# Download and make GoogleTest available
FetchContent_MakeAvailable(googletest)

# Enable CMake testing infrastructure
enable_testing()

# ============================================================================
# STATE MACHINE TESTS
# ============================================================================

# Create executable for state machine unit tests
# Includes the test source and the implementation file being tested
add_executable(system_tests_state_machine
    unit/system_test_state_machine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/system/state_machine/system_state_machine.cpp
)

# Add include directories for state machine tests
target_include_directories(system_tests_state_machine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/system/state_machine
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/control/flight_controller
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/control/motor_mixer
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Link state machine tests against GoogleTest main library
target_link_libraries(system_tests_state_machine
    GTest::gtest_main
)

# ============================================================================
# CONTROL TESTS
# ============================================================================

# Create executable for control system unit tests
# Includes the test source and the flight controller implementation being tested
add_executable(control_tests
    unit/control_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/control/flight_controller/flight_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/control/motor_mixer/motor_mixer.cpp
)

# Add include directories for control tests
target_include_directories(control_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/system/state_machine
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/control/flight_controller
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/control/motor_mixer
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Link control tests against GoogleTest main library
target_link_libraries(control_tests
    GTest::gtest_main
)

# ============================================================================
# TEST DISCOVERY
# ============================================================================

# Include GoogleTest CMake module for test discovery
include(GoogleTest)

# Automatically discover and register all Google Test tests
gtest_discover_tests(system_tests_state_machine)
gtest_discover_tests(control_tests)