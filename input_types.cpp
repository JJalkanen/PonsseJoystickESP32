#include "input_types.h"
#include <cstddef> 

const uint8_t gamepad_descriptor[] = {
    0x05, 0x01,        // Generic Desktop
    0x09, 0x05,        // Gamepad
    0xA1, 0x01,        // Application Collection

    // --------------------
    // 4 Axes (16-bit signed)
    // --------------------
    0x09, 0x30,        // X
    0x09, 0x31,        // Y
    0x09, 0x32,        // Z
    0x09, 0x33,        // Rx

    0x16, 0x00, 0x80,  // Logical Min (-32768)
    0x26, 0xFF, 0x7F,  // Logical Max (32767)
    0x75, 0x10,        // 16-bit
    0x95, 0x04,        // 4 reports
    0x81, 0x02,        // Input (Data,Var,Abs)

    // --------------------
    // 19 Buttons
    // --------------------
    0x05, 0x09,        // Button page
    0x19, 0x01,
    0x29, 0x13,        // 19 buttons
    0x15, 0x00,
    0x25, 0x01,
    0x75, 0x01,
    0x95, 0x13,
    0x81, 0x02,

    // padding (to align to byte boundary: 5 bits)
    0x75, 0x01,
    0x95, 0x05,
    0x81, 0x03,

    0xC0
};

const size_t gamepad_descriptor_size = sizeof(gamepad_descriptor);
