#pragma once

#include <Arduino.h>
#include "gamepad.h"

// Forward declare modules (you implement these)
#include "USB_HID.h"
#include "CANSender.h"
#include "MicroROSSender.h"
#include "BLE_HID.h"
#include <Preferences.h>

class SystemManager {
public:
    enum Mode {
        MODE_USB = 0,
        MODE_CAN = 1,
        MODE_WIFI = 2,
        MODE_BLE = 3,
        MODE_WIFI_CONFIG = 4,
        MODE_NONE = 5
    };

    void begin();
    void update();
    void saveMode();
    void loadMode();
    void dispatch(const JoystickState& state);

    Mode getActiveMode() const { return active_mode; }

private:

    private:

    Preferences prefs;

    // -------------------------
    // Modules
    // -------------------------
    USBHID usb;
    CANSender can;
    MicroROSSender wifi;
    BLEGamepad ble;

    // -------------------------
    // State
    // -------------------------
    Mode active_mode = MODE_NONE;

    // -------------------------
    // Timing
    // -------------------------
    unsigned long last_eval_time = 0;
    static constexpr unsigned long EVAL_INTERVAL_MS = 10;

    // -------------------------
    // Mode switching
    // -------------------------
    unsigned long combo_start_time = 0;
    bool combo_active = false;
    bool combo_triggered = false;

    static constexpr unsigned long HOLD_TIME_MS = 5000;

    // -------------------------
    // Internal logic
    // -------------------------
    void evaluateMode(const JoystickState& state);

    bool buttonsPressed(
        uint32_t buttons,
        std::initializer_list<int> btns
    );
};
