#include "rf_controller.h"
#include "config.h"
#include "leds.h"
#include "watchdog.h"
#include <pgmspace.h>

bool rfState = false;
static unsigned long lastRfChangeTime = 0;
static unsigned long lastRfSyncTime = 0;

// ============================================================
// DOĞRULANMIŞ AÇMA PAKETİ (Paket 127)
// ============================================================
static const uint16_t pktOpen[] PROGMEM = {
  396, 464, 368, 876, 396, 2540, 372, 456, 884, 460, 372, 460, 372, 872, 372, 876, 400, 456, 372, 460, 
  788, 1704, 404, 872, 1204, 456, 376, 868, 408, 2528, 380, 452, 888, 452, 380, 452, 380, 868, 376, 868, 
  408, 452, 380, 448, 796, 1696, 412, 864, 1212, 452, 380, 864, 408, 2528, 384, 448, 896, 448, 384, 448, 
  384, 860, 384, 864, 412, 448, 380, 448, 800, 1696, 408, 864, 1216, 448, 380, 868, 408, 2524, 384, 448, 
  896, 444, 384, 448, 384, 864, 384, 860, 412, 448, 384, 444, 800, 1692, 416, 860, 1216, 448, 384, 860, 
  412, 2524, 384, 448, 896, 444, 388, 444, 384, 864, 388, 856, 412, 448, 384, 444, 804, 1692, 412, 864, 
  1216, 444, 384, 860, 416, 2520, 388, 444, 904, 444, 388, 444, 384, 860, 388, 860, 412, 444, 388, 444, 
  804, 1688, 416, 860, 1220, 448, 380, 860, 416, 2520, 388, 440, 960
};

// ============================================================
// DOĞRULANMIŞ KAPATMA PAKETİ (Paket 55)
// ============================================================
static const uint16_t pktClose[] PROGMEM = {
  124, 448, 892, 452, 380, 452, 380, 864, 380, 868, 408, 448, 380, 452, 796, 1696, 412, 1276, 1216, 1280, 
  408, 864, 384, 860, 800, 448, 896, 444, 388, 444, 384, 864, 384, 860, 412, 448, 384, 448, 800, 1692, 
  412, 1276, 1220, 1272, 416, 860, 388, 856, 804, 444, 904, 444, 388, 440, 388, 860, 388, 856, 416, 444, 
  388, 444, 800, 1692, 416, 1272, 1220, 1276, 412, 860, 388, 856, 808, 440, 900, 444, 388, 440, 392, 856, 
  388, 860, 416, 440, 388, 444, 804, 1688, 416, 1272, 1224, 1272, 416, 856, 392, 856, 804, 440, 900, 444, 
  388, 444, 388, 856, 388, 860, 416, 440, 392, 440, 804, 1688, 420, 1272, 1220, 1272, 416, 856, 392, 856, 
  804, 444, 904, 444, 388, 440, 392, 856, 388, 856, 420, 440, 388, 444, 804, 1688, 420, 1268, 1224, 1272, 
  416, 856, 392, 856, 804, 440
};

void initRf() {
  pinMode(RF_PIN, OUTPUT);
  digitalWrite(RF_PIN, LOW);
}

void sendRawSignal(const uint16_t* signal, size_t length, int repeats) {
  for (int r = 0; r < repeats; r++) {
    bool state = true;
    for (size_t i = 0; i < length; i++) {
      uint16_t duration = pgm_read_word(&signal[i]);
      digitalWrite(RF_PIN, state ? HIGH : LOW);
      delayMicroseconds(duration);
      state = !state;
    }
    digitalWrite(RF_PIN, LOW);
    feedWatchdog();
    delay(10);
  }
}

void setRfState(bool state, bool force) {
  if (rfState == state) return;

  unsigned long currentMillis = millis();
  if (!force && (currentMillis - lastRfChangeTime < MIN_RF_CHANGE_INTERVAL_MS) && lastRfChangeTime != 0) {
    Serial.printf("[RF]: Degisim engellendi (Kombi kisa dongu korumasi: %lu sn beklemede)\r\n", 
                  (MIN_RF_CHANGE_INTERVAL_MS - (currentMillis - lastRfChangeTime)) / 1000);
    return;
  }

  rfState = state;
  lastRfChangeTime = currentMillis;
  lastRfSyncTime = currentMillis;
  setGreenLed(state);

  if (state) {
    sendRawSignal(pktOpen, sizeof(pktOpen) / sizeof(pktOpen[0]));
    Serial.println("[RF]: ACMA Sinyali Gonderildi (YESIL LED: ON)");
  } else {
    sendRawSignal(pktClose, sizeof(pktClose) / sizeof(pktClose[0]));
    Serial.println("[RF]: KAPATMA Sinyali Gonderildi (YESIL LED: OFF)");
  }
}

void handleRfPeriodicSync() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastRfSyncTime >= RF_SYNC_INTERVAL_MS) {
    lastRfSyncTime = currentMillis;
    if (rfState) {
      sendRawSignal(pktOpen, sizeof(pktOpen) / sizeof(pktOpen[0]));
      Serial.println("[RF-SYNC]: Periyodik ACMA Sinyali Tekrarlandi (Keep-Alive)");
    } else {
      sendRawSignal(pktClose, sizeof(pktClose) / sizeof(pktClose[0]));
      Serial.println("[RF-SYNC]: Periyodik KAPATMA Sinyali Tekrarlandi (Keep-Alive)");
    }
  }
}
