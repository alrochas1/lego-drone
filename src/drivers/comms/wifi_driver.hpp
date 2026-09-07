#pragma once

#include <cstdint>

class WifiDriver {
public:
    WifiDriver(
        const char* ssid,
        const char* password,
        const char* remote_ip,
        uint16_t remote_port
    );

    ~WifiDriver();

    bool init();
    bool connect(uint32_t timeout_ms);
    bool is_connected() const;

    bool send(const void* data, uint16_t length);
    bool send(const char* data);

private:
    const char* ssid_;
    const char* password_;

    const char* remote_ip_;
    uint16_t    remote_port_;

    bool initialized_;
    bool connected_;

    int socket_;
};