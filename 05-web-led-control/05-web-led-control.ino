// ==========================================
// PROGRAM KONTROL LED ESP8266 MELALUI WEB
// ==========================================

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Konfigurasi Wi-Fi diambil dari secrets.h (tidak ikut di-commit)
// secrets.h dibuat dari .env dengan menjalankan generate-secrets.ps1
#include "secrets.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Membuat web server pada port 80
ESP8266WebServer server(80);

// Menentukan pin LED
const int LED_BUILTIN_PIN = LED_BUILTIN;

// Menyimpan status LED (true = nyala)
bool ledState = false;

// ==========================================
// HALAMAN UTAMA
// ==========================================

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html lang="id">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Kontrol LED IoT</title>

    <style>
      body {
        font-family: Arial, sans-serif;
        text-align: center;
        background-color: #f2f2f2;
        margin-top: 80px;
      }

      h2 {
        color: #222;
      }

      p {
        color: #555;
      }

      button {
        padding: 12px 25px;
        margin: 10px;
        border: none;
        border-radius: 8px;
        font-size: 16px;
        color: white;
        cursor: pointer;
      }

      .on {
        background-color: green;
      }

      .off {
        background-color: red;
      }
    </style>
  </head>

  <body>
    <h2>Control LED ESP8266</h2>
    <p>Praktikum Pemrograman IoT</p>

    <button class="on" onclick="kirim('/on')">LED ON</button>
    <button class="off" onclick="kirim('/off')">LED OFF</button>

    <p id="status">Status LED: -</p>

    <script>
      function kirim(path) {
        fetch(path)
          .then(function (res) { return res.json(); })
          .then(function (data) {
            document.getElementById("status").textContent =
              "Status LED: " + (data.led === "on" ? "NYALA" : "MATI");
          })
          .catch(function () {
            document.getElementById("status").textContent = "Gagal terhubung ke ESP8266";
          });
      }

      kirim("/status");
    </script>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

// ==========================================
// HEADER CORS
// Agar index.html di laptop boleh memanggil
// ESP8266 lewat JavaScript (fetch)
// ==========================================

void sendCORS() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.sendHeader("Access-Control-Allow-Private-Network", "true");
}

// ==========================================
// MENGIRIM STATUS LED DALAM FORMAT JSON
// Contoh balasan: {"led":"on"}
// ==========================================

void sendStatus() {
  sendCORS();

  String json = "{\"led\":\"";
  json += ledState ? "on" : "off";
  json += "\"}";

  server.send(200, "application/json", json);
}

// ==========================================
// MENGATUR LED
// LED bawaan ESP8266 aktif LOW:
// LOW = nyala, HIGH = mati
// ==========================================

void setLED(bool on) {
  ledState = on;
  digitalWrite(LED_BUILTIN_PIN, on ? LOW : HIGH);

  Serial.println(on ? "LED NYALA" : "LED MATI");
}

// ==========================================
// FUNGSI MENYALAKAN LED
// ==========================================

void handleLEDOn() {
  setLED(true);
  sendStatus();
}

// ==========================================
// FUNGSI MEMATIKAN LED
// ==========================================

void handleLEDOff() {
  setLED(false);
  sendStatus();
}

// ==========================================
// FUNGSI CEK STATUS LED
// ==========================================

void handleStatus() {
  sendStatus();
}

// ==========================================
// ALAMAT TIDAK DIKENAL
// Juga menjawab permintaan OPTIONS (preflight CORS)
// ==========================================

void handleNotFound() {
  sendCORS();

  if (server.method() == HTTP_OPTIONS) {
    server.send(204);
    return;
  }

  server.send(404, "text/plain", "Halaman tidak ditemukan");
}

// ==========================================
// SETUP
// ==========================================

void setup() {
  // Memulai komunikasi serial
  Serial.begin(115200);

  // Mengatur pin LED sebagai output
  pinMode(LED_BUILTIN_PIN, OUTPUT);

  // Mematikan LED saat awal
  setLED(false);

  // Menghubungkan ke Wi-Fi
  WiFi.mode(WIFI_STA);

  // Matikan mode hemat daya Wi-Fi agar ESP tidak "hilang" dari jaringan
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi berhasil terhubung!");

  // Menampilkan alamat IP ESP8266
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());

  // Mengatur alamat halaman web
  server.on("/", handleRoot);
  server.on("/on", handleLEDOn);
  server.on("/off", handleLEDOff);
  server.on("/status", handleStatus);
  server.onNotFound(handleNotFound);

  // Memulai web server
  server.begin();

  Serial.println("Web server berjalan!");
}

// ==========================================
// LOOP
// ==========================================

void loop() {
  // Menangani permintaan dari browser
  server.handleClient();
}
