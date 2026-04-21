#include <Arduino.h>
#include "sim_input.h"
#include "gamepad.h"
#include "USB_HID.h"
#include "input.h"
#include "CANSender.h"

CANSender can;
GamepadManager gamepad;
USBHID usbHid;
input input;
JoystickState state;

unsigned long int previousMillis = 0;
const long interval = 1000;


void setup() {
  Serial.begin(115200);
  Serial.println("Serial started!");
  input.input_init();
  //initSimInput();
  usbHid.initUSB();
  can.begin();
}

void loop() {
  
    input.input_update(state);
    gamepad.update(state);
    const auto& gp = gamepad.getGamepad();
    usbHid.sendGamepad(gp);
    can.send(gp);
    Serial.println("looping");
    delay(100);
    
}
