#pragma once
#include <Arduino.h>

extern bool rfState;

void initRf();
void sendRawSignal(const uint16_t* signal, size_t length, int repeats = 5);
void setRfState(bool state, bool force = false);
void handleRfPeriodicSync();
