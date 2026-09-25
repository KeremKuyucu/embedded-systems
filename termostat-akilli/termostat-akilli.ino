#include "config.h"
#include "watchdog.h"
#include "leds.h"
#include "storage.h"
#include "rf_controller.h"
#include "network_manager.h"
#include "automation.h"
#include "button.h"

void setup() {
  Serial.begin(115200);

  esp_reset_reason_t resetReason = esp_reset_reason();

  initWatchdog();
  feedWatchdog();

  loadSettingsFromNVS();
  initLeds();
  initRf();
  initButton();
  initAutomation();

  setupWiFi();

  if (resetReason == ESP_RST_TASK_WDT || resetReason == ESP_RST_WDT || 
      resetReason == ESP_RST_INT_WDT || resetReason == ESP_RST_PANIC ||
      resetReason == ESP_RST_BROWNOUT) {
    Serial.printf("[WDT/SISTEM]: UYARI - Cihaz reset sonrasi baslatildi! (Neden Kodu: %d)\r\n", (int)resetReason);
  }

  setupSinricPro();
  feedWatchdog();
}

void loop() {
  feedWatchdog();
  handleButton();
  handleWiFiRuntime();
  handleSinricPro();
  handleTemperatureAutomation();
  handleRfPeriodicSync();
  handleNvsDebounce();
  yield();
}
