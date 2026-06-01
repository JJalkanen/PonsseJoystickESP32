#include "sim_input.h"
#include <Arduino.h>

static uint16_t val = 0;
static bool dir = true;
static uint8_t currentButton = 0;

void initSimInput() {
    val = 0;
    currentButton = 0;
}

static void updateVal() {
    if (dir) val++;
    else val--;

    if (val >= 255) dir = false;
    if (val == 0) dir = true;
}

static uint32_t getNextButton() {
    uint32_t btnMask = 1UL << currentButton; // set current button
    currentButton = (currentButton + 1) % 27; // cycle 0..18
    return btnMask;
}

JoystickState getSim() {
    updateVal();
    uint16_t adc_val;      // 12-bit equivalent
    adc_val = ((uint16_t)val << 4) | (val >> 4);  
    
    JoystickState s{};
    s.x1 = adc_val;
    s.y1 = 4095-adc_val;
    s.x2 = adc_val/2;
    s.y2 = 4095 - (adc_val/2);
    s.buttons = getNextButton();
    return s;
}
