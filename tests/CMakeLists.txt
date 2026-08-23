cmake_minimum_required(VERSION 3.16)

project(drone_tests LANGUAGES CXX)

include(FetchContent)

# TODO: Improve this
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(system_tests_state_machine
    unit/system_test_state_machine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/system/state_machine/system_state_machine.cpp
)

target_include_directories(system_tests_state_machine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tasks/system/state_machine
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(system_tests_state_machine
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(system_tests_state_machine)