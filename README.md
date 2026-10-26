# Praktikum IoT ESP8266

Kumpulan sketch Arduino untuk Praktikum Pemrograman IoT menggunakan board ESP8266 (NodeMCU / Wemos D1 mini).

> ⭐ Jika repository ini membantu, tolong beri **star** ya!

## Daftar Praktikum

| No  | Folder                                            | Tanggal     | Isi                                                                                   |
| --- | ------------------------------------------------- | ----------- | ------------------------------------------------------------------------------------- |
| 01  | [01-led-blink](01-led-blink/)                     | 28 Sep 2026 | Pola kedip LED pada GPIO 13 (LED shield)                                              |
| 02  | [02-digital-read-serial](02-digital-read-serial/) | 5 Okt 2026  | Membaca push button di D2 dan menampilkan ke Serial Monitor                           |
| 03  | [03-dht11-monitoring](03-dht11-monitoring/)       | 12 Okt 2026 | Monitoring suhu & kelembapan DHT11 (D4) dengan indikator LED merah (D5) dan biru (D6) |
| 04  | [04-buzzer-off](04-buzzer-off/)                   | 19 Okt 2026 | Mematikan buzzer di D5                                                                |
| 05  | [05-web-led-control](05-web-led-control/)         | 19 Okt 2026 | Kontrol LED bawaan ESP8266 lewat web server + halaman web terpisah                    |
| 06  | [06-web-sensor-monitoring](06-web-sensor-monitoring/) | 26 Okt 2026 | Web server monitoring DHT11 (D4) + analog A0, kontrol LED bawaan & LED D6           |

## Struktur Folder

```
praktikum-iot-esp8266/
├── 01-led-blink/
├── 02-digital-read-serial/
├── 03-dht11-monitoring/
├── 04-buzzer-off/
├── 05-web-led-control/
│   ├── 05-web-led-control.ino   # web server ESP8266 (/on, /off, /status)
│   └── web/
│       ├── index.html           # halaman kontrol LED dari laptop
│       └── script.js            # memanggil ESP8266 lewat fetch
├── 06-web-sensor-monitoring/
│   └── 06-web-sensor-monitoring.ino  # web server (/, /data, /led1/on, ...), HTML di PROGMEM
├── libraries/                   # library Arduino yang dipakai
├── .env.example                 # contoh SSID & password Wi-Fi (salin jadi .env)
└── generate-secrets.ps1         # membuat secrets.h dari .env
```

## Library

Library yang dipakai ada di folder [libraries/](libraries/):

- `DHT_sensor_library` + `Adafruit_Unified_Sensor` — sensor DHT11 (praktikum 03)
- `DHT11`
- `LiquidCrystal`, `LiquidCrystal_I2C` — LCD

Salin isi folder `libraries/` ke `Documents/Arduino/libraries/` jika belum terpasang.

## Cara Menjalankan

1. Pasang board ESP8266 di Arduino IDE (Boards Manager → `esp8266`).
2. Buka file `.ino` di salah satu folder praktikum, pilih board dan port, lalu Upload.
3. Untuk praktikum 05 dan 06:
   - Salin `.env.example` menjadi `.env`, lalu isi `WIFI_SSID` dan `WIFI_PASSWORD`.
   - Jalankan `powershell -ExecutionPolicy Bypass -File .\generate-secrets.ps1` dari folder utama
     untuk membuat `secrets.h` dari `.env` (Arduino tidak bisa membaca `.env` langsung).
     Ulangi setiap kali isi `.env` diubah.
   - `.env` dan `secrets.h` sudah ada di `.gitignore` sehingga password Wi-Fi tidak ikut di-commit.
   - Upload `05-web-led-control.ino` atau `06-web-sensor-monitoring.ino`.
   - Lihat alamat IP ESP8266 di Serial Monitor (baud 115200).
   - Buka `http://<IP-ESP>` di browser. Untuk praktikum 05 bisa juga ganti `ESP_IP` di `web/script.js`
     lalu buka `web/index.html`.
   - Laptop dan ESP8266 harus terhubung ke Wi-Fi yang sama.
