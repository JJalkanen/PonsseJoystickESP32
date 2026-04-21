#pragma once

#include "input_types.h"
#include <Adafruit_TinyUSB.h>


class USBHID {
public:
    void initUSB();
    void sendGamepad(const JoystickState& s);
    
private:
  //Adafruit HID gamepad object
  Adafruit_USBD_HID HIDgamepad;
};
