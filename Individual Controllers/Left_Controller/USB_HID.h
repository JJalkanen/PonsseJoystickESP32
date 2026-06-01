#pragma once

#include "input_types.h"
#include <Adafruit_TinyUSB.h>

class USBHID {
public:
    void initUSB();
    void send(const JoystickState& s);
    
private:
  //Adafruit HID gamepad object
  Adafruit_USBD_HID HIDgamepad;
};
