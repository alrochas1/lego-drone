#include "drivers/comms/wifi_driver.hpp"

#if BOARD_W
#include "pico/cyw43_arch.h"

#include "lwip/inet.h"
#include "lwip/sockets.h"

#undef connect
#undef send
#endif

#include <cstdio>
#include <cstring>

WifiDriver::WifiDriver(
    const char* ssid,
    const char* password,
    const char* remote_ip,
    uint16_t remote_port
)
    : ssid_(ssid),
      password_(password),
      remote_ip_(remote_ip),
      remote_port_(remote_port),
      initialized_(false),
      connected_(false),
      socket_(-1) {
}

WifiDriver::~WifiDriver() {

    #if BOARD_W

    if (socket_ >= 0) {
        lwip_close(socket_);
        socket_ = -1;
    }

    if (initialized_) {
        cyw43_arch_deinit();
    }

    #endif
}


bool WifiDriver::init() {

    #if BOARD_W

    if (cyw43_arch_init() != 0) {
        printf("[WIFI] Init failed\n");
        return false;
    }

    cyw43_arch_enable_sta_mode();

    initialized_ = true;

    printf("[WIFI] Initialized\n");

    return true;

    #else

    printf("[WIFI] WARNING: WiFi support is disabled.\n");

    return false;

        #endif
}

bool WifiDriver::connect(uint32_t timeout_ms) {

    #if BOARD_W

    if (!initialized_) {
        return false;
    }

    printf("[WIFI] Connecting to %s...\n", ssid_);

    const int err = cyw43_arch_wifi_connect_timeout_ms(
        ssid_,
        password_,
        CYW43_AUTH_WPA2_AES_PSK,
        timeout_ms
    );

    if (err != 0) {
        printf("[WIFI] Connection failed: %d\n", err);
        connected_ = false;
        return false;
    }

    connected_ = true;

    printf("[WIFI] Connected\n");

    socket_ = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_ < 0) {
        printf("[WIFI] Failed to create UDP socket\n");
        connected_ = false;
        return false;
    }

    printf(
        "[WIFI] UDP configured: %s:%u\n",
        remote_ip_,
        static_cast<unsigned>(remote_port_)
    );

    return true;

    #else

    (void)timeout_ms;
    return false;

    #endif
}

bool WifiDriver::is_connected() const {
    return connected_;
}

bool WifiDriver::send(const void* data, uint16_t length) {

    #if BOARD_W

    if (!connected_ || socket_ < 0) {
        return false;
    }

    sockaddr_in destination{};
    destination.sin_family = AF_INET;
    destination.sin_port = htons(remote_port_);

    if (inet_aton(remote_ip_, &destination.sin_addr) == 0) {
        printf("[WIFI] Invalid destination IP: %s\n", remote_ip_);
        return false;
    }

    const int result = sendto(
        socket_,
        data,
        length,
        0,
        reinterpret_cast<sockaddr*>(&destination),
        sizeof(destination)
    );

    if (result < 0) {
        printf("[WIFI] UDP send failed\n");
        return false;
    }

    return result == length;

    #else

    (void)data;
    (void)length;

    return false;

    #endif
}

bool WifiDriver::send(const char* data) {

    if (data == nullptr) {
        return false;
    }

    return send(data, static_cast<uint16_t>(strlen(data)));

}