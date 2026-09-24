#include "leds.h"
#include "config.h"

void initLeds() {
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  ledcAttach(LED_BLUE, BLUE_PWM_FREQ, BLUE_PWM_RES);
#else
  ledcSetup(BLUE_PWM_CHANNEL, BLUE_PWM_FREQ, BLUE_PWM_RES);
  ledcAttachPin(LED_BLUE, BLUE_PWM_CHANNEL);
#endif
  setBlueLedBrightness(0);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
}

void setBlueLedBrightness(uint8_t duty) {
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  ledcWrite(LED_BLUE, duty);
#else
  ledcWrite(BLUE_PWM_CHANNEL, duty);
#endif
}

void setGreenLed(bool state) {
  digitalWrite(LED_GREEN, state ? HIGH : LOW);
}

void setRedLed(bool state) {
  digitalWrite(LED_RED, state ? HIGH : LOW);
}
