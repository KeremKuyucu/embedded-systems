#include "automation.h"
#include "config.h"
#include "leds.h"
#include "storage.h"
#include "rf_controller.h"
#include "network_manager.h"
#include "button.h"
#include "watchdog.h"
#include <DHT.h>

float lastValidTemp = 0.0f;
float lastValidHumidity = 0.0f;
bool dhtErrorActive = false;

static int dhtFailCount = 0;
static int dhtGlitchCount = 0;
static unsigned long lastTempCheck = 0;
static unsigned long lastCloudReport = 0;

static DHT dht(DHTPIN, DHTTYPE);

void initAutomation() {
  dht.begin();
  lastTempCheck = millis(); // Sensörün açılışta voltajının oturması için 3 sn süre tanı
}

void triggerImmediateTempCheck() {
  lastTempCheck = 0;
}

void handleTemperatureAutomation() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastTempCheck < TEMP_CHECK_INTERVAL_MS) return;
  lastTempCheck = currentMillis;

  feedWatchdog();
  float currentTemp = dht.readTemperature();
  float currentHumidity = dht.readHumidity();
  feedWatchdog();

  if (isnan(currentTemp) || isnan(currentHumidity)) {
    dhtFailCount++;
    Serial.printf("[HATA]: DHT11 verisi okunamadi! (Ardisik Hata: %d/%d)\r\n", 
                  dhtFailCount, DHT_MAX_CONSECUTIVE_FAILS);

    if (dhtFailCount >= DHT_MAX_CONSECUTIVE_FAILS) {
      setRedLed(true);

      if (rfState && !manualOverrideActive) {
        Serial.println("[GUVENLIK]: Kalici sensor arizasi nedeniyle kombi KAPATILIYOR!");
        setRfState(false, true);
      } else if (rfState && manualOverrideActive) {
        Serial.println("[GUVENLIK]: Sensor arizali fakat D23 Manuel Mod devrede oldugu icin kombi ACIK tutuluyor!");
      }

      if (!dhtErrorActive) {
        dhtErrorActive = true;
        Serial.println("[HATA]: DHT11 Sensor Arizasi Aktif!");
      }
    }
    return;
  }

  // 3B: Ani Okuma Zıplama Filtresi (Glitch Filter)
  if (lastValidTemp > 0.0f && abs(currentTemp - lastValidTemp) > 3.0f) {
    dhtGlitchCount++;
    Serial.printf("[DHT11-FILTRE]: Ani sicaklik sicramasi tespit edildi (Eski: %.1f C, Yeni: %.1f C | Filtre: %d/%d)\r\n", 
                  lastValidTemp, currentTemp, dhtGlitchCount, DHT_MAX_GLITCH_FAILS);
    if (dhtGlitchCount <= DHT_MAX_GLITCH_FAILS) {
      return; // Tekil hatalı okumayı pas geç
    }
  }
  dhtGlitchCount = 0;

  dhtFailCount = 0;
  lastValidTemp = currentTemp;
  lastValidHumidity = currentHumidity;

  if (dhtErrorActive) {
    dhtErrorActive = false;
    setRedLed(false);
    Serial.println("[BILGI]: DHT11 sensoru tekrar normale dondu.");
  } else {
    setRedLed(false);
  }

  Serial.printf("[DHT11]: Sicaklik: %.1f C | Nem: %.1f %% | Hedef: %.1f C (±%.1f) | Mod: %s | Cihaz: %s%s\r\n",
                currentTemp,
                currentHumidity,
                targetTemperature,
                hysteresis,
                thermostatMode,
                devicePowerState ? "ACIK" : "KAPALI",
                manualOverrideActive ? " | MANUEL MOD: ACIK" : "");

  // Bulut Raporlama (Rate-limit koruması: 60 saniyede bir gönderilir)
  if (sinricConnected && (currentMillis - lastCloudReport >= CLOUD_REPORT_INTERVAL_MS)) {
    lastCloudReport = currentMillis;
    sendTemperatureToCloud(currentTemp, currentHumidity);
  }

  // D23 Buton Manuel Modu Aktifse:
  // Cihazın güç durumu veya derece ne olursa olsun kombi açık tutulur, otomasyon müdahale etmez.
  if (manualOverrideActive) {
    if (!rfState) {
      setRfState(true, true);
    }
    return;
  }

  if (!devicePowerState || strcmp(thermostatMode, "OFF") == 0) {
    if (rfState) setRfState(false, true);
    return;
  }

  if (strcmp(thermostatMode, "HEAT") == 0 || strcmp(thermostatMode, "AUTO") == 0) {
    float lowerThreshold = targetTemperature - hysteresis;
    float upperThreshold = targetTemperature;

    if (currentTemp <= lowerThreshold && !rfState) {
      Serial.printf("[Otomasyon]: Sicaklik (%.1f C) alt esik altinda (<= %.1f C) -> CALISTIRILIYOR\r\n", 
                    currentTemp, lowerThreshold);
      setRfState(true);
    } else if (currentTemp >= upperThreshold && rfState) {
      Serial.printf("[Otomasyon]: Sicaklik (%.1f C) hedefe ulasti (>= %.1f C) -> DURDURULUYOR\r\n", 
                    currentTemp, upperThreshold);
      setRfState(false);
    }
  }
}
