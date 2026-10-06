# ⚡ Embedded Systems & IoT Archive

Bu depo; bağımsız IoT çözümleri, RF tersine mühendislik çalışmaları, ESP32 projeleri ile lise/atölye döneminde **Deneyap Kart** ve **Arduino** üzerinde geliştirilmiş prototip ve kod örneklerini içerir.

---

## 🌟 Öne Çıkan Projeler (Featured Projects)

### 1. [🔥 Akıllı Oda Termostatı & Kombi Otomasyonu](termostat-akilli/)
ESP32 tabanlı, 433 MHz RF modülü ile evdeki kombiyi kablosuz kontrol eden modüler akıllı termostat sistemi.
* **Sesli Asistan & IoT:** [SinricPro](https://sinric.pro/) entegrasyonu sayesinde Alexa ve Google Home üzerinden sıcaklık takibi ve sesli kontrol.
* **Mimari & Kararlılık:**
  * Modüler C++ tasarımı (`automation`, `network_manager`, `rf_controller`, `storage`, `watchdog`).
  * Donanımsal **Watchdog Timer (WDT)** ve Brownout reset kurtarma mekanizması.
  * Histerezis eşikli dinamik sıcaklık kontrolü.
  * Flash bellek ömrünü koruyan **NVS Debounce** hafıza katmanı.
  * Periyodik RF senkronizasyonu ve Wi-Fi yeniden bağlanma yönetimi.

### 2. [📻 Kombi RF Sinyal Kontrolcüsü](kombi_sinyalleri/)
Mevcut kablosuz oda termostatının 433.92 MHz sinyal paketlerinin tersine mühendislikle çözümlenmesi ve mikrosaniye seviyesinde zamanlama dizileriyle (`PROGMEM`) yeniden üretilmesi.
* Seri monitör üzerinden doğrudan açma/kapama kontrolü.
* Akıllı termostat projesinin RF iletim temelini oluşturan prototip.

### 3. [💡 ESP32 Wi-Fi LED Kontrolü](Esp32_wifiled_kontrol/)
ESP32 üzerinde koşan gömülü web sunucusu ile yerel ağ üzerinden GPIO ve LED kontrolü demosu.

---

## 🛠️ Deneyap Kart & Atölye Çalışmaları (`Deneyap Kart/`)

Bu bölüm; Teknofest, atölye eğitimleri ve hızlı prototipleme seanslarında Deneyap Kart (ESP32 tabanlı) ve Arduino platformları için geliştirilmiş laboratuvar kodlarıdır. Projeler işlevlerine göre 4 ana kategoride düzenlenmiştir:

### 🤖 [Robotik & Hareketli Sistemler](Deneyap%20Kart/robotik/)
* **`bluetooth_araba/`** – Bluetooth üzerinden mobil uygulama ile yönlendirilen robot araba.
* **`cizgi_izleyen_robot/`** – Kızılötesi kontrast sensörlü otonom çizgi izleyici.
* **`haciyatmaz_tek_eksen/` & `haciyatmaz_cift_eksen/`** – İvmeölçer/jiroskop ile tek ve çift eksenli self-balancing denge robotu.
* **`joystick_servo_araba/`** – Çok kanallı joystick ve servo kontrollü mekanik araç.
* **`robot_kol_joystick/` & `robot_kol_joystick_tr/`** – 4 eksenli servo robot kol kontrolü.
* **`yangin_sondurucu_robot/` & `su_fiskirtan_robot/`** – Alev algılayıcı ve su pompası entegreli otonom yangın müdahale araçları.
* **`trafik_alev_robot/`** – Trafik ışığı ve alev sensörü kombinasyonlu güvenlik aracı.

### ⚙️ [Motor & Mekanizma Kontrolü](Deneyap%20Kart/motor_kontrol/)
* **`step_motor_joystick/`** – Step motorun joystick eksenleriyle hassas açı ve hız kontrolü.
* **`joystick_servo_2eksen/` & `joystick_servo_4kanal/`** – Analog potansiyometre ve joystick ile çoklu servo senkronizasyonu.
* **`joystick_servo_kontrol/`** – Farklı servo sürücü testleri ve kontrol algoritmaları.

### 📡 [IoT & Kablosuz Haberleşme](Deneyap%20Kart/iot_ve_kablosuz/)
* **`bluetooth_akilli_ev/` & `bluetooth_akilli_ev_v2/`** – Bluetooth üzerinden aydınlatma, röle ve çevre birimi kontrolü.
* **`thingspeak_sicaklik_nem/`** – Wi-Fi üzerinden ThingSpeak bulut IoT platformuna ortam verisi telemetrisi.
* **`wifi_led_kontrol/`** – Web arayüzü ile kablosuz durum kontrolü.

### 🌡️ [Sensör & Çevre Birimleri](Deneyap%20Kart/sensor_ve_cevre_birimleri/)
* **`dht11_sicaklik_nem/`** – Ortam sıcaklık ve bağıl nem ölçümü.
* **`keypad_3x4/`** – Matris tuş takımı ile şifreli giriş algoritması.
* **`rfid_kapi_kilidi/`** – RC522 RFID kart okuyucu ile yetkili geçiş ve servo kilit sistemi.
* **`mesafe_led_seviye/` & `mesafe_servo_buzzer/`** – Ultrasonik HC-SR04 ile dinamik park/bariyer uyarı sistemleri.
* **`pot_rgb_led/`** – Analog girişlerle PWM renk karışımı kontrolü.
* **`sensor_demo_komple/`** – Çoklu sensör girişlerinin eşzamanlı test kodu.

---

## 📂 Dizin Yapısı

```text
embedded-systems/
├── termostat-akilli/          # ESP32 + SinricPro + RF433 Akıllı Termostat
├── kombi_sinyalleri/          # 433 MHz RF Sinyal Tersine Mühendislik & Kontrol
├── Esp32_wifiled_kontrol/     # ESP32 Web Server LED Kontrol
├── libraries/                 # Projelerde kullanılan harici kütüphaneler
└── Deneyap Kart/              # Atölye ve Prototip Arşivi
    ├── robotik/               # Robot araba, denge robotu, robot kol, yangın robotu
    ├── motor_kontrol/         # Step ve servo motor kontrol algoritmaları
    ├── iot_ve_kablosuz/       # Bluetooth ev otomasyonu, ThingSpeak telemetri
    └── sensor_ve_cevre_birimleri/ # RFID, Keypad, DHT11, Mesafe ve RGB LED
```

---

## 🔧 Donanım & Teknolojiler
* **Mikrodenetleyiciler:** ESP32, Deneyap Kart (ESP32-WROVER), Arduino Uno / Nano.
* **Haberleşme Protokolleri:** 433.92 MHz RF (ASK/OOK), Wi-Fi (HTTP/WebSocket), BLE/Bluetooth Classic.
* **Yazılım & Çatılar:** C/C++, Arduino Framework, SinricPro SDK, ThingSpeak REST API.
