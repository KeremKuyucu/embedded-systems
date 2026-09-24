#include "watchdog.h"
#include "config.h"
#include <esp_task_wdt.h>

void initWatchdog() {
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = WDT_TIMEOUT_SECONDS * 1000,
    .idle_core_mask = 0,
    .trigger_panic = true
  };
  esp_task_wdt_reconfigure(&wdt_config);
#else
  esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true);
#endif
  esp_task_wdt_add(NULL);
  Serial.println("[WDT]: Watchdog Timer baslatildi.");
}

void feedWatchdog() {
  esp_task_wdt_reset();
}
