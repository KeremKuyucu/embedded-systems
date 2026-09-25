#include "button.h"
#include "config.h"
#include "rf_controller.h"
#include "storage.h"
#include "automation.h"

bool manualOverrideActive = false;
static bool savedRfStateBeforeOverride = false;

static int lastRawReading = HIGH;
static int stableButtonState = HIGH;
static unsigned long lastDebounceTime = 0;

void initButton() {
#if BUTTON_ACTIVE_LEVEL == LOW
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lastRawReading = HIGH;
  stableButtonState = HIGH;
#else
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  lastRawReading = LOW;
  stableButtonState = LOW;
#endif

  Serial.printf("[BUTON]: D%d pini manuel kombi kontrolu icin baslatildi (Aktif Seviye: %s).\r\n",
                BUTTON_PIN, (BUTTON_ACTIVE_LEVEL == LOW) ? "LOW (GND)" : "HIGH (3.3V)");
}

void toggleManualOverride() {
  manualOverrideActive = !manualOverrideActive;

  if (manualOverrideActive) {
    savedRfStateBeforeOverride = rfState;
    Serial.println("\r\n========================================================");
    Serial.println("[MANUEL MOD]: D23 BUTONUNA BASILDI -> KOMBI DOGRUDAN ACILDI!");
    Serial.printf("[MANUEL MOD]: Cihaz durumu ve derece yoksayiliyor. (Onceki Durum: %s)\r\n", 
                  savedRfStateBeforeOverride ? "ACIK" : "KAPALI");
    Serial.println("========================================================");
    
    // Kombiyi gecikmesiz ve sartlara bakilmaksizin dogrudan calistir
    setRfState(true, true);
  } else {
    Serial.println("\r\n========================================================");
    Serial.println("[MANUEL MOD]: D23 BUTONUNA TEKRAR BASILDI -> ESKI HALINE DONULUYOR!");
    Serial.println("[MANUEL MOD]: Otomatik derece ve termostat kontrolune geri gecildi.");
    Serial.println("========================================================");

    // Otomatik termostat durumuna ve hedefe gore kombi durumunu yeniden ayarla
    if (!devicePowerState || strcmp(thermostatMode, "OFF") == 0) {
      Serial.println("[MANUEL MOD]: Cihaz/Mod kapali -> Kombi KAPATILIYOR.");
      setRfState(false, true);
    } else if (lastValidTemp > 0.0f && lastValidTemp >= targetTemperature) {
      Serial.printf("[MANUEL MOD]: Sicaklik (%.1f C) >= Hedef (%.1f C) -> Kombi KAPATILIYOR.\r\n", 
                    lastValidTemp, targetTemperature);
      setRfState(false, true);
    } else if (lastValidTemp > 0.0f && lastValidTemp <= (targetTemperature - hysteresis)) {
      Serial.printf("[MANUEL MOD]: Sicaklik (%.1f C) <= Alt Esik (%.1f C) -> Kombi ACIK tutuluyor.\r\n", 
                    lastValidTemp, (targetTemperature - hysteresis));
      setRfState(true, true);
    } else {
      // Histerezis araliginda (olu bolge): Butona basilmadan onceki duruma geri don
      Serial.printf("[MANUEL MOD]: Sicaklik histerezis bandinda (%.1f C). Onceki durum geri yukleniyor: %s\r\n",
                    lastValidTemp, savedRfStateBeforeOverride ? "ACIK" : "KAPALI");
      setRfState(savedRfStateBeforeOverride, true);
    }

    triggerImmediateTempCheck();
  }
}

void handleButton() {
  int rawReading = digitalRead(BUTTON_PIN);

  if (rawReading != lastRawReading) {
    lastDebounceTime = millis();
    lastRawReading = rawReading;
  }

  if ((millis() - lastDebounceTime) >= BUTTON_DEBOUNCE_MS) {
    if (rawReading != stableButtonState) {
      stableButtonState = rawReading;

      // Butona basilma (kenar tetikleme) ani
      if (stableButtonState == BUTTON_ACTIVE_LEVEL) {
        toggleManualOverride();
      }
    }
  }
}
