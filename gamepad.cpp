#include "gamepad.h"


int32_t palmXc = 1850;
int32_t palmXmin = 350;
int32_t palmXmax = 3500;

int32_t palmYc = 1850;
int32_t palmYmin = 350;
int32_t palmYmax = 3500;

int32_t thumbXc = 1850;
int32_t thumbXmin = 350;
int32_t thumbXmax = 3500;

int32_t thumbYc = 1850;
int32_t thumbYmin = 350;
int32_t thumbYmax = 3500;

void GamepadManager::update(const JoystickState& Input)
{
    auto processAxis = [](int32_t v, int32_t center, int32_t min, int32_t max)
    {

      int32_t delta = v - center;

      // ✅ DEADZONE
      if (abs(delta) < 80)   // tweak this (50–150)
        return (int16_t)0;
        
      int32_t out;

      if (v >= center)
        out = (v - center) * 32767 / (max - center);
      else
        out = (v - center) * 32768 / (center - min);

      if (out > 32767) out = 32767;
      if (out < -32768) out = -32768;

      return (int16_t)out;
    };
    MyGamepad.x1 = processAxis(Input.x1, palmXc, palmXmin, palmXmax);
    MyGamepad.y1 = processAxis(Input.y1, palmYc, palmYmin, palmYmax);
    MyGamepad.x2 = processAxis(Input.x2, thumbXc, thumbXmin, thumbXmax);
    MyGamepad.y2 = processAxis(Input.y2, thumbYc, thumbYmin, thumbYmax);

    MyGamepad.buttons = Input.buttons;
}

const JoystickState& GamepadManager::getGamepad() const
{
    return MyGamepad;
}
