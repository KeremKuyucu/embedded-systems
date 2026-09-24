#include "network_manager.h"
#include "config.h"
#include "leds.h"
#include "watchdog.h"
#include "storage.h"
#include "rf_controller.h"
#include "automation.h"
#include <WiFi.h>
#include <SinricPro.h>
#include <SinricProThermostat.h>

bool sinricConnected = false;

static unsigned long lastWiFiCheck = 0;
static bool wasWiFiConnected = false;
static unsigned long lastWiFiReconnectAttempt = 0;

static bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("[SinricPro]: Termostat Gucu -> %s\r\n", state ? "ACIK" : "KAPALI");
  if (devicePowerState != state) {
    devicePowerState = state;
    markSettingsChanged();
  }
  if (!devicePowerState) {
    setRfState(false, true);
  } else {
    triggerImmediateTempCheck();
  }
  return true;
}

static bool onTargetTemperature(const String &deviceId, float &temp) {
  if (abs(targetTemperature - temp) > 0.01f) {
    targetTemperature = temp;
    markSettingsChanged();
  }
  Serial.printf("[SinricPro]: Yeni Hedef Sicaklik -> %.1f C\r\n", targetTemperature);
  triggerImmediateTempCheck();
  return true;
}

static bool onAdjustTargetTemperature(const String &deviceId, float &tempDelta) {
  targetTemperature += tempDelta;
  markSettingsChanged();
  Serial.printf("[SinricPro]: Hedef Sicaklik Degistirildi -> %.1f C\r\n", targetTemperature);
  triggerImmediateTempCheck();
  return true;
}

static bool onThermostatMode(const String &deviceId, String &mode) {
  mode.toUpperCase();
  if (mode == "COOL") {
    Serial.println("[SinricPro]: COOL modu desteklenmiyor, islem yoksayildi.");
    mode = thermostatMode;
    return false;
  }
  if (strcmp(thermostatMode, mode.c_str()) != 0) {
    strncpy(thermostatMode, mode.c_str(), sizeof(thermostatMode) - 1);
    thermostatMode[sizeof(thermostatMode) - 1] = '\0';
    markSettingsChanged();
  }
  Serial.printf("[SinricPro]: Mod Degisti -> %s\r\n", thermostatMode);
  
  if (strcmp(thermostatMode, "OFF") == 0) {
    setRfState(false, true);
  }
  triggerImmediateTempCheck();
  return true;
}

void setupWiFi() {
  Serial.printf("\r\n[WiFi]: %s agina baglaniliyor", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  unsigned long startAttemptTime = millis();
  bool blinkState = false;

  // 15 saniyelik zaman aşımı ile sonsuz döngü ve WDT çökmesi engellenir
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    feedWatchdog();
    blinkState = !blinkState;
    setBlueLedBrightness(blinkState ? 100 : 0);
    delay(300);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wasWiFiConnected = true;
    setBlueLedBrightness(BLUE_DIM_DUTY);
    Serial.printf("\n[WiFi]: Baglanti basarili! IP: %s\r\n", WiFi.localIP().toString().c_str());
  } else {
    setBlueLedBrightness(0);
    Serial.println("\n[WiFi]: Ilk baglanti basarisiz, loop icinde tekrar denenecek.");
  }
  feedWatchdog();
}

void handleWiFiRuntime() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastWiFiCheck < WIFI_CHECK_INTERVAL_MS) return;
  lastWiFiCheck = currentMillis;

  bool isConnected = (WiFi.status() == WL_CONNECTED);

  // Wi-Fi yeniden bağlandığında LED durumunu güncelle
  if (isConnected && !wasWiFiConnected) {
    wasWiFiConnected = true;
    Serial.printf("[WiFi]: Baglanti yeniden kuruldu! IP: %s\r\n", WiFi.localIP().toString().c_str());
    if (!sinricConnected) {
      setBlueLedBrightness(BLUE_DIM_DUTY);
    }
  }

  // Wi-Fi koptuğunda
  if (!isConnected) {
    if (wasWiFiConnected) {
      wasWiFiConnected = false;
      sinricConnected = false;
      setBlueLedBrightness(0);
      Serial.println("[WiFi]: Baglanti koptu!");
    }

    if (currentMillis - lastWiFiReconnectAttempt >= WIFI_RECONNECT_COOLDOWN_MS) {
      lastWiFiReconnectAttempt = currentMillis;
      Serial.println("[WiFi]: Otomatik baglanti basarisiz, manuel yeniden deneniyor...");
      WiFi.disconnect(false);
      delay(100);
      feedWatchdog();
      WiFi.begin(WIFI_SSID, WIFI_PASS);
    }
  }
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void setupSinricPro() {
  SinricProThermostat &myThermostat = SinricPro[THERMOSTAT_ID];
  
  myThermostat.onPowerState(onPowerState);
  myThermostat.onTargetTemperature(onTargetTemperature);
  myThermostat.onAdjustTargetTemperature(onAdjustTargetTemperature);
  myThermostat.onThermostatMode(onThermostatMode);

  SinricPro.onConnected([](){ 
    Serial.println("[SinricPro]: Baglanti kuruldu."); 
    sinricConnected = true;
    setBlueLedBrightness(BLUE_DIM_DUTY);
  });
  
  SinricPro.onDisconnected([](){ 
    Serial.println("[SinricPro]: Baglanti kesildi."); 
    sinricConnected = false;
    if (WiFi.status() == WL_CONNECTED) {
      setBlueLedBrightness(BLUE_DIM_DUTY);
    } else {
      setBlueLedBrightness(0);
    }
  });
  
  SinricPro.begin(APP_KEY, APP_SECRET);
  feedWatchdog();
}

void handleSinricPro() {
  SinricPro.handle();
}

void sendTemperatureToCloud(float temp, float humidity) {
  if (sinricConnected) {
    SinricProThermostat &myThermostat = SinricPro[THERMOSTAT_ID];
    myThermostat.sendTemperatureEvent(temp, humidity);
  }
}
