#pragma once
#include <Arduino.h>

// ============================================================================
// DURUM LED'LERİ VE PİN ŞEMASI (3 LED TEŞHİS MİMARİSİ)
// ============================================================================
// 1. MAVİ LED (GPIO 25 - LED_BLUE):
//    - Yanıp Sönüyor (Tam Güç) : Wi-Fi ağına bağlanmaya çalışıyor.
//    - Loş / Kısık Sabit Yanıyor: Wi-Fi ve Sinric Pro bulut bağlantısı başarılı (%6 parlaklık).
//    - Sönük                   : Ağ veya bulut bağlantısı kesildi / yok.
//
// 2. YEŞİL LED (GPIO 33 - LED_GREEN):
//    - Sabit Yanıyor           : Kombi / Isıtıcı devrede (RF Açma Sinyali Aktif).
//    - Sönük                   : Kombi kapalı (Hedefe ulaşıldı veya mod kapalı).
//
// 3. KIRMIZI LED (GPIO 32 - LED_RED):
//    - Sönük                   : Donanım sağlam, sensör düzgün çalışıyor.
//    - Sabit Yanıyor           : HATA! DHT11 okunamıyor (NaN / Kablo temassızlığı).
//
// DİĞER DONANIM PİNLERİ:
//    - GPIO 14                 : DHT11 Sıcaklık ve Nem Sensörü DATA Pini
//    - GPIO 26                 : FS1000A 433MHz RF Verici DATA Pini
// ============================================================================

// ==========================================
// KİMLİK BİLGİLERİ (env.h dosyasından okunur)
// ==========================================
#if __has_include("env.h")
  #include "env.h"
#elif __has_include("env.example.h")
  #include "env.example.h"
  #warning "env.h bulunamadi, varsayilan ornek sablon (env.example.h) yuklendi!"
#else
  #error "env.h veya env.example.h dosyasi bulunamadi!"
#endif

// ==========================================
// PİN TANIMLAMALARI & PWM AYARLARI
// ==========================================
#define RF_PIN                      26   // FS1000A RF Verici DATA Pini
#define DHTPIN                      14   // DHT11 Data Pini
#define DHTTYPE                     DHT11

// Durum LED Pinleri
#define LED_BLUE                    25   // Mavi: Wi-Fi & Bulut Bağlantı Durumu (PWM ile sürülür)
#define LED_GREEN                   33  // Yeşil: Kombi / Isıtma Devrede (RF Aktif)
#define LED_RED                     32   // Kırmızı: Donanım / Sensör Hatası

#define BLUE_PWM_CHANNEL            0
#define BLUE_PWM_FREQ               5000
#define BLUE_PWM_RES                8
#define BLUE_DIM_DUTY               15   // 0-255 arası parlaklık değeri (~%6 loş ışık)

// ==========================================
// WATCHDOG TIMER (WDT) AYARLARI
// ==========================================
#define WDT_TIMEOUT_SECONDS         20

// ==========================================
// ZAMANLAMA VE EŞİK DEĞERLERİ
// ==========================================
const unsigned long MIN_RF_CHANGE_INTERVAL_MS   = 60000;   // Kombi kısa döngü koruması: min 60 sn
const unsigned long RF_SYNC_INTERVAL_MS         = 300000;  // RF durum senkronizasyonu: 5 dk
const unsigned long WIFI_CHECK_INTERVAL_MS      = 10000;   // Wi-Fi kontrol aralığı: 10 sn
const unsigned long WIFI_RECONNECT_COOLDOWN_MS  = 60000;   // Manuel yeniden bağlantı beklemesi: 60 sn
const unsigned long NVS_SAVE_DEBOUNCE_MS        = 5000;    // NVS Flash kayıt debounce süresi: 5 sn
const unsigned long TEMP_CHECK_INTERVAL_MS      = 3000;    // Lokal sıcaklık kontrol aralığı: 3 sn
const unsigned long CLOUD_REPORT_INTERVAL_MS    = 60000;   // Sinric Cloud raporlama aralığı: 60 sn

const int DHT_MAX_CONSECUTIVE_FAILS             = 3;       // Peş peşe hata toleransı
const int DHT_MAX_GLITCH_FAILS                  = 2;       // Ani sıcaklık sıçraması toleransı
