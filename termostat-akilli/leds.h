#pragma once
#include <Arduino.h>

void initLeds();
void setBlueLedBrightness(uint8_t duty);
void setGreenLed(bool state);
void setRedLed(bool state);
