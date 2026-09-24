#pragma once
#include <Arduino.h>

extern float lastValidTemp;
extern float lastValidHumidity;
extern bool dhtErrorActive;

void initAutomation();
void handleTemperatureAutomation();
void triggerImmediateTempCheck();
