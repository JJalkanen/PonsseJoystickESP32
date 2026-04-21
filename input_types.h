#pragma once

#include <stdint.h>
#include <cstddef>

// TODO: implement 2 joystick states, one for left, and 1 for right. BLE and wifi then combine these, and USB sends them seperately
struct JoystickState {
    uint16_t x1 = 0;
    uint16_t y1 = 0;
    uint16_t x2 = 0;
    uint16_t y2 = 0;

    uint32_t buttons = 0;  // up to 32 buttons
};

static const uint32_t BUTTON_MASK_19 = 0x7FFFFFF;

extern const uint8_t gamepad_descriptor[];

extern const size_t gamepad_descriptor_size;
