#pragma once

#include <Arduino.h>
#include "driver/twai.h"
#include "input_types.h"

class CANSender
{
public:
    void begin();
    void send(const JoystickState& s);

private:

    unsigned long last_tx = 0;
    
    void sendAxes(const JoystickState& s);
    void sendButtons(const JoystickState& s);

    uint32_t makeJ1939Id(uint32_t pgn);

    const uint8_t sourceAddr = 0x23;
    const uint8_t priority   = 6;
};
