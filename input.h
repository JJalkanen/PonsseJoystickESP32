#pragma once
#include <stdint.h>
#include "input_types.h"

class input
{
public:
    void input_init();
    void input_update(JoystickState& state);
};
