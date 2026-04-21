#pragma once

#include "input_types.h"
#include <Arduino.h>

class GamepadManager
{
public:
    void update(const JoystickState& Input);

    const JoystickState& getGamepad() const;

private:
    JoystickState MyGamepad;
};
