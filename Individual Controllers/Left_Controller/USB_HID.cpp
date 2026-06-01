#include "USB_HID.h"
#include <Arduino.h>
  
// ======================================================
// Combined report filler
// ======================================================
static void fill_gamepad_report(uint8_t* r, const JoystickState& s)
{
    auto put16 = [&](int16_t v, int i) {
        r[i]     = v & 0xFF;
        r[i + 1] = (v >> 8) & 0xFF;
    };

    // AXES (must already be -32768..32767)
    put16(s.x1, 0);
    put16(s.y1, 2);
    put16(s.x2, 4);
    put16(s.y2, 6);

    // BUTTONS (19 bits)
    uint32_t b = s.buttons & 0x7FFFF;

    r[8]  = b & 0xFF;
    r[9]  = (b >> 8) & 0xFF;
    r[10] = (b >> 16) & 0xFF;
}


void USBHID::initUSB() {
    
    TinyUSBDevice.setProductDescriptor("Ponsse Comforthandle Left");

    HIDgamepad.setReportDescriptor(gamepad_descriptor,
                                gamepad_descriptor_size);
    HIDgamepad.begin();

}


// ======================================================
// Send combined report
// ======================================================
void USBHID::send(const JoystickState& s)
{
    uint8_t report[11] = {0};
    
    fill_gamepad_report(report, s);

    HIDgamepad.sendReport(0, report, 11);
}
