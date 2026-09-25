#pragma once
#include <Arduino.h>

extern bool manualOverrideActive;

void initButton();
void handleButton();
void toggleManualOverride();
