#include "systemManager.h"

// -------------------------
// INIT
// -------------------------
void SystemManager::begin()
{
    usb.initUSB();
    can.begin();
    wifi.begin();
    ble.begin();
    
    prefs.begin("joystick", true);
    int saved = prefs.getInt("mode", MODE_USB);
    if (saved < MODE_USB || saved > MODE_NONE) {
        saved = MODE_USB;
    }
    active_mode = (Mode)saved;
    prefs.end();
}

void SystemManager::update() {
  switch(active_mode){
    case MODE_WIFI:
        wifi.update();
        break;
     case MODE_USB:
     case MODE_BLE:
     case MODE_CAN:
     default:
        break;
}
}

void SystemManager::saveMode()
{
    prefs.begin("joystick", false);
    prefs.putInt("mode", (int)active_mode);
    prefs.end();
}



bool SystemManager::buttonsPressed(
    uint32_t buttons,
    std::initializer_list<int> btns)
{
    for (int b : btns) {

        if (!(buttons & (1UL << b))) {
            return false;
        }
    }

    return true;
}

void SystemManager::evaluateMode(const JoystickState& state)
{
    unsigned long now = millis();

    // -------------------------------------------------
    // MODE COMBINATIONS
    // -------------------------------------------------

    bool usb_combo  = buttonsPressed(state.buttons, {0, 15});
    bool ble_combo  = buttonsPressed(state.buttons, {0, 14});
    bool wifi_combo = buttonsPressed(state.buttons, {0, 13});
    bool can_combo  = buttonsPressed(state.buttons, {0, 16});
    bool wifi_manager_combo = buttonsPressed(state.buttons, {0, 17});

    Mode requested_mode = MODE_NONE;

    if (usb_combo) {
        requested_mode = MODE_USB;
    }
    else if (ble_combo) {
        requested_mode = MODE_BLE;
    }
    else if (wifi_combo) {
        requested_mode = MODE_WIFI;
    }
    else if (can_combo) {
        requested_mode = MODE_CAN;
    }
    else if (wifi_manager_combo) {
      if (active_mode == MODE_WIFI){
        requested_mode = MODE_WIFI_CONFIG;
      }
    }

    // -------------------------------------------------
    // HOLD DETECTION
    // -------------------------------------------------

    if (requested_mode != MODE_NONE) {

        if (!combo_active) {

            combo_active = true;
            combo_triggered = false;
            combo_start_time = now;
        }

        // Held long enough
        if (!combo_triggered &&
            (now - combo_start_time >= HOLD_TIME_MS))
        {
            combo_triggered = true;

            active_mode = requested_mode;

            if (active_mode == MODE_WIFI_CONFIG) {
               wifi.openConfigPortal();
               active_mode = MODE_WIFI;
            }
            
            if (active_mode != MODE_WIFI_CONFIG) {
              saveMode();
            }
        }

    } else {

        // Reset state after release
        combo_active = false;
        combo_triggered = false;
    }
}

// -------------------------
// DISPATCH JOYSTICK DATA
// -------------------------
void SystemManager::dispatch(const JoystickState& state)
{
    evaluateMode(state);

    Serial.print("mode");
    Serial.println(active_mode);
    
    switch (active_mode)
    {
        case MODE_USB:
            usb.send(state);
            break;

        case MODE_CAN:
            can.send(state);
            break;

        case MODE_WIFI:
            wifi.send(state);
            break;

        case MODE_BLE:
            ble.send(state);
            break;

        default:
            break;
    }
}
    
