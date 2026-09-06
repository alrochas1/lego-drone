# Search for all source and header files
file(GLOB_RECURSE SOURCES 
    "*.cpp" 
    "*.c"
)

# Create the wifi configuration file from the template
set(WIFI_CREDENTIALS_FILE
    ${CMAKE_SOURCE_DIR}/cmake/wifi_credentials.cmake
)

if(EXISTS ${WIFI_CREDENTIALS_FILE})
    include(${WIFI_CREDENTIALS_FILE})
else()
    message(WARNING "WiFi credentials file not found")
    set(WIFI_SSID "")
    set(WIFI_PASSWORD "")
    set(WIFI_REMOTE_IP "")
    set(WIFI_REMOTE_PORT "0")
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/include/config/wifi_config.hpp.in
    ${CMAKE_SOURCE_DIR}/include/config/wifi_config.hpp
    @ONLY
)

# Create executable
add_executable(${ProjectName}
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${FREERTOS_KERNEL_PATH}/include
    ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040
)

target_link_libraries(${ProjectName} 
    pico_stdlib 
    hardware_adc
    hardware_i2c
    hardware_pwm
    FreeRTOS-Kernel-Heap4 
    )

# Add WiFi support if BOARD_W is set to pico_w
if(BOARD_W)

    target_link_libraries(${ProjectName}
        pico_cyw43_arch_lwip_sys_freertos
    )

    target_compile_definitions(${ProjectName}
        PRIVATE BOARD_W
    )

endif()

pico_enable_stdio_usb(${ProjectName} 1) 
pico_enable_stdio_uart(${ProjectName} 0)

pico_add_extra_outputs(${ProjectName})