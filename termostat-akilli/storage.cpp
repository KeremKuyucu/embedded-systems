#include "storage.h"
#include "config.h"
#include <Preferences.h>

float targetTemperature = 22.0f;
float hysteresis = 0.5f;
bool devicePowerState = true;
char thermostatMode[16] = "HEAT";

static Preferences preferences;
static bool settingsDirty = false;
static unsigned long lastSettingChangeTime = 0;

void loadSettingsFromNVS() {
  if (!preferences.begin("thermostat", true)) {
    Serial.println("[NVS]: Namespace acilamadi, varsayilan ayarlar kullaniliyor.");
    return;
  }
  targetTemperature = preferences.getFloat("targetTemp", 22.0f);
  hysteresis = preferences.getFloat("hysteresis", 0.5f);
  devicePowerState = preferences.getBool("powerState", true);
  String modeStr = preferences.getString("mode", "HEAT");
  strncpy(thermostatMode, modeStr.c_str(), sizeof(thermostatMode) - 1);
  thermostatMode[sizeof(thermostatMode) - 1] = '\0';
  preferences.end();

  Serial.printf("[NVS]: Ayarlar yuklendi -> Hedef: %.1f C | Histerezis: %.1f C | Guc: %s | Mod: %s\r\n",
                targetTemperature, hysteresis, devicePowerState ? "ACIK" : "KAPALI", thermostatMode);
}

void saveSettingsToNVS() {
  if (!preferences.begin("thermostat", false)) {
    Serial.println("[NVS]: Yazma icin namespace acilamadi!");
    return;
  }
  preferences.putFloat("targetTemp", targetTemperature);
  preferences.putFloat("hysteresis", hysteresis);
  preferences.putBool("powerState", devicePowerState);
  preferences.putString("mode", thermostatMode);
  preferences.end();
  settingsDirty = false;
  Serial.println("[NVS]: Ayarlar Flash bellege (NVS) basariyla kaydedildi.");
}

void markSettingsChanged() {
  settingsDirty = true;
  lastSettingChangeTime = millis();
}

void handleNvsDebounce() {
  if (settingsDirty && (millis() - lastSettingChangeTime >= NVS_SAVE_DEBOUNCE_MS)) {
    saveSettingsToNVS();
  }
}
