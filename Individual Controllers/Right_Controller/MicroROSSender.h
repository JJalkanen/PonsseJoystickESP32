#pragma once

#include <micro_ros_arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <sensor_msgs/msg/joy.h>

#include "gamepad.h"

#include <Preferences.h>

class MicroROSSender {
public:
    
    void begin();
    void update();
    void send(const JoystickState& state);
    void openConfigPortal();
    void saveWiFi(const String& ssid, const String& pass);
    bool loadWiFi(String &ssid, String &pass);

private:
    // ROS
    rcl_publisher_t pub;
    rcl_node_t node;
    rclc_support_t support;
    rcl_allocator_t allocator;

    sensor_msgs__msg__Joy msg;

    // buffers
    static constexpr int AXES = 4;
    static constexpr int BUTTONS = 19;

    float axes_buf[AXES];
    int32_t buttons_buf[BUTTONS];

    // WiFi / ROS state
    bool wifi_connected = false;
    bool wifi_ready = false;
    bool ros_ready = false;

    unsigned long last_wifi_attempt = 0;
    unsigned long last_ros_attempt = 0;

    static constexpr unsigned long WIFI_RETRY_MS = 3000;
    static constexpr unsigned long ROS_RETRY_MS = 3000;

    unsigned long wifi_ready_time = 0;

    bool wifi_manager_started = false;
    bool force_config_portal = false;
    bool ROS_transport_initialized = false;

    Preferences prefs;

    // credentials
    const char* agent_ip;
    uint16_t agent_port;
    String ssid;
    String password;
};
