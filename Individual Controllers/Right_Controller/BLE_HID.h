#pragma once

#include <Arduino.h>
#include <NimBLEDevice.h>
#include "input_types.h"

class BLEGamepad {
public:
    void begin(const char* name = "Ponsse Comforthandle Right");
    void send(const JoystickState& js);
    static bool connected;

private:
    static NimBLECharacteristic* inputReport;
};
