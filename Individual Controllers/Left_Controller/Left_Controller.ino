#include <Arduino.h>
#include "sim_input.h"
#include "gamepad.h"
#include "USB_HID.h"
#include "input.h"
#include "CANSender.h"
#include "MicroROSSender.h"
#include "systemManager.h"
#include "BLE_HID.h"

input input;
JoystickState state;
GamepadManager gamepad;
SystemManager manager;

unsigned long previousMillis = 0;
const unsigned long interval = 10; // ms



void setup() {
  Serial.begin(115200);
  //Serial.println("Serial started!");
  input.input_init();
  //initSimInput();
  input.input_update(state);
  gamepad.init(state);
  manager.begin();
  delay(1000);
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        input.input_update(state);
        gamepad.update(state);

        const auto& gp = gamepad.getGamepad();

        manager.update();
        manager.dispatch(gp);
    }

    // Other non-blocking tasks can run here
}
