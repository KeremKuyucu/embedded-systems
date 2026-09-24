#pragma once
#include <Arduino.h>

// Global termostat durum değişkenleri
extern float targetTemperature;
extern float hysteresis;
extern bool devicePowerState;
extern char thermostatMode[16];

void loadSettingsFromNVS();
void saveSettingsToNVS();
void markSettingsChanged();
void handleNvsDebounce();
