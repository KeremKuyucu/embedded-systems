#pragma once
#include <Arduino.h>

extern bool sinricConnected;

void setupWiFi();
void handleWiFiRuntime();
bool isWiFiConnected();

void setupSinricPro();
void handleSinricPro();
void sendTemperatureToCloud(float temp, float humidity);
