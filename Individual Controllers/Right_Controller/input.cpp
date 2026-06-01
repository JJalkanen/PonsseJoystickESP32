#include "input.h"
#include <Arduino.h>

// ----------------------------
// BUTTON MATRIX (MATCH WORKING SKETCH)
// ----------------------------

// POWER PINS: rows (outputs)
static const uint8_t rowPins[4] = {35, 36, 47, 48};

// INPUT PINS: columns (inputs)
static const uint8_t colPins[8] = {37, 38, 39, 40, 41, 42, 2, 1};

// Button mapping (UNCHANGED from working sketch)
static const int buttonMap[4][8] = {
  {6,   7,  8,  11, -1, 12, -1, -1},
  {0,   1,  2,   9,  3, 10,  4,  5},
  {13, -1, 14, 15, -1, 16, -1, -1},
  {17, -1, -1, -1, -1, 18, -1, -1}
};

/*
static const int buttonMap[4][8] = {
  {6,   7,  8,  11, -1, 12, -1, -1},
  {0,   1,  2,   9,  3, 10,  4,  5},
  {13, -1, 14, 15, -1, 16, -1, -1},
  {17, -1, -1, -1, -1, 18, -1, -1}
};
 */

// ----------------------------
// AXES
// ----------------------------
#define PIN_X1 4
#define PIN_Y1 5
#define PIN_X2 6
#define PIN_Y2 7
#define PIN_POT 8

static uint16_t read_axis(uint8_t pin)
{
  return analogRead(pin); // keep full 12-bit
}

void input::input_init()
{
    // Rows = outputs
    for (int i = 0; i < 4; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], LOW);
    }

    // Cols = inputs
    for (int i = 0; i < 8; i++) {
        pinMode(colPins[i], INPUT_PULLDOWN);
    }

    // ADC config (IMPORTANT)
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
}

void input::input_update(JoystickState& s)
{
    // ----------------------------
    // AXES
    // ----------------------------
    s.x1 = read_axis(PIN_X1);
    s.y1 = read_axis(PIN_Y1);
    s.x2 = read_axis(PIN_X2);
    s.y2 = read_axis(PIN_Y2);

    // ----------------------------
    // BUTTON MATRIX
    // ----------------------------
    bool pressed[19] = {false};

    for (int r = 0; r < 4; r++) {

        digitalWrite(rowPins[r], HIGH);
        delayMicroseconds(50);

        for (int c = 0; c < 8; c++) {

            if (digitalRead(colPins[c]) == HIGH) {
                int btn = buttonMap[r][c];
                if (btn != -1) {
                    pressed[btn] = true;
                }
            }
        }

        digitalWrite(rowPins[r], LOW);
    }

    // Pack into bitfield
    uint32_t buttons = 0;

    for (int i = 0; i <= 18; i++) {
        if (pressed[i]) {
            buttons |= (1UL << i);
        }
    }

    s.buttons = buttons;
}
