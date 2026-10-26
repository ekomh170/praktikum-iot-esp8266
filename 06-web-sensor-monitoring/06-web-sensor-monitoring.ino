#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <math.h>

// Konfigurasi Wi-Fi 2,4 GHz diambil dari secrets.h (tidak ikut di-commit)
// secrets.h dibuat dari .env dengan menjalankan generate-secrets.ps1
#include "secrets.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

#define DHT_PIN 4       // GPIO4: pin D4 di WeMos D1 R1 (D2 di D1 mini/NodeMCU)
#define DHT_TYPE DHT11  // Jika memakai DHT22, ubah menjadi DHT22

const uint8_t LED1_PIN = LED_BUILTIN; // Active-low (LOW = ON)
const uint8_t LED2_PIN = D6;          // Active-high (HIGH = ON)
const unsigned long INTERVAL_SENSOR = 2000UL;

ESP8266WebServer server(80);
DHT dht(DHT_PIN, DHT_TYPE);

bool led1On = false;
bool led2On = false;
float temperatur = NAN;
float kelembapan = NAN;
int nilaiAnalog = -1; // -1 berarti tidak dikirim / tidak aktif
unsigned long waktuSensor = 0;

// Tampilan HTML & JavaScript yang disimpan dalam PROGMEM
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP8266 Web Server - Praktikum IoT</title>
  <style>
    :root {
      --bg: #f3f5f9; --card: #ffffff; --text: #1d2433; --muted: #6b7385; --line: #e4e8ef;
      --green: #16834a; --green-soft: #e3f4ea; --red: #c8392b; --red-soft: #fbe9e7;
      --amber: #f5b301; --blue: #2f6fe4;
    }
    @media (prefers-color-scheme: dark) {
      :root {
        --bg: #0f141c; --card: #171e29; --text: #e7ebf2; --muted: #8e98aa; --line: #263041;
        --green: #2fb36a; --green-soft: #12321f; --red: #e05545; --red-soft: #3a1916;
      }
    }
    * { box-sizing: border-box; }
    body { margin: 0; background: var(--bg); color: var(--text);
           font-family: -apple-system, "Segoe UI", Roboto, Arial, sans-serif; }
    main { max-width: 560px; margin: 0 auto; padding: 24px 16px 32px; }
    header { display: flex; justify-content: space-between; align-items: flex-start; gap: 12px; margin-bottom: 18px; }
    h1 { font-size: 22px; margin: 0 0 4px; }
    .subjudul { color: var(--muted); font-size: 13px; margin: 0; }
    .badge { flex-shrink: 0; display: inline-flex; align-items: center; gap: 6px; font-size: 12px; font-weight: 600;
             padding: 5px 10px; border-radius: 999px; background: var(--line); color: var(--muted); }
    .badge::before { content: ""; width: 8px; height: 8px; border-radius: 50%; background: currentColor; }
    .badge.ok { background: var(--green-soft); color: var(--green); }
    .badge.putus { background: var(--red-soft); color: var(--red); }
    .card { background: var(--card); border: 1px solid var(--line); border-radius: 14px; padding: 16px; margin-bottom: 14px; }
    h2 { font-size: 15px; margin: 0 0 12px; }
    .led-row { display: flex; align-items: center; gap: 12px; padding: 10px 0; flex-wrap: wrap; }
    .led-row + .led-row { border-top: 1px solid var(--line); }
    .led-info { display: flex; align-items: center; gap: 10px; flex: 1; min-width: 150px; }
    .lampu { width: 14px; height: 14px; border-radius: 50%; background: var(--line); transition: all .2s; }
    .lampu.nyala { background: var(--amber); box-shadow: 0 0 10px var(--amber); }
    .nama { font-weight: 700; }
    .ket { font-size: 12px; color: var(--muted); }
    .pill { font-size: 12px; font-weight: 600; padding: 3px 10px; border-radius: 999px; background: var(--line); color: var(--muted); }
    .pill.nyala { background: var(--green-soft); color: var(--green); }
    .tombol { display: flex; border: 1px solid var(--line); border-radius: 8px; overflow: hidden; }
    button { border: 0; background: transparent; padding: 8px 18px; font-weight: 700; cursor: pointer; transition: all .15s; }
    button.on { color: var(--green); }
    button.off { color: var(--red); border-left: 1px solid var(--line); }
    button.on.aktif { background: var(--green); color: #fff; }
    button.off.aktif { background: var(--red); color: #fff; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); gap: 10px; }
    .stat { background: var(--bg); border-radius: 10px; padding: 12px 14px; }
    .label { font-size: 12px; color: var(--muted); margin-bottom: 6px; }
    .nilai { font-size: 26px; font-weight: 700; }
    .nilai small { font-size: 14px; font-weight: 600; color: var(--muted); margin-left: 3px; }
    .nilai .mati { font-size: 15px; color: var(--muted); }
    .bar { height: 6px; border-radius: 3px; background: var(--line); margin-top: 8px; overflow: hidden; }
    .bar div { height: 100%; width: 0; background: var(--blue); transition: width .3s; }
    .catatan { font-size: 12px; color: var(--muted); margin: 12px 0 0; }
    footer { font-size: 12px; color: var(--muted); text-align: center; }
  </style>
</head>
<body>
  <main>
    <header>
      <div>
        <h1>ESP8266 Web Server</h1>
        <p class="subjudul">Monitoring Sensor dan Kontrol LED</p>
      </div>
      <span id="koneksi" class="badge">Menghubungkan...</span>
    </header>

    <section class="card">
      <h2>Kontrol LED</h2>
      <div class="led-row">
        <div class="led-info">
          <span class="lampu" id="lampu1"></span>
          <div>
            <div class="nama">LED 1</div>
            <div class="ket">LED bawaan board</div>
          </div>
        </div>
        <span class="pill" id="st1">Mati</span>
        <div class="tombol">
          <button class="on" id="on1" onclick="setLED(1, 'on')">ON</button>
          <button class="off" id="off1" onclick="setLED(1, 'off')">OFF</button>
        </div>
      </div>
      <div class="led-row">
        <div class="led-info">
          <span class="lampu" id="lampu2"></span>
          <div>
            <div class="nama">LED 2</div>
            <div class="ket">LED eksternal (D6)</div>
          </div>
        </div>
        <span class="pill" id="st2">Mati</span>
        <div class="tombol">
          <button class="on" id="on2" onclick="setLED(2, 'on')">ON</button>
          <button class="off" id="off2" onclick="setLED(2, 'off')">OFF</button>
        </div>
      </div>
    </section>

    <section class="card">
      <h2>Data Sensor</h2>
      <div class="grid">
        <div class="stat">
          <div class="label">Temperatur</div>
          <div class="nilai"><span id="temp">--</span><small>°C</small></div>
        </div>
        <div class="stat">
          <div class="label">Kelembapan</div>
          <div class="nilai"><span id="hum">--</span><small>%</small></div>
        </div>
        <div class="stat">
          <div class="label">Analog (A0)</div>
          <div class="nilai"><span id="analog" class="mati">Tidak aktif</span></div>
          <div class="bar"><div id="barAnalog"></div></div>
        </div>
      </div>
      <p class="catatan" id="noteAnalog">Analog tidak dikirim karena kedua LED belum sama-sama menyala.</p>
    </section>

    <footer>Pembaruan otomatis setiap 2 detik · <span id="waktu">menunggu data</span></footer>
  </main>

  <script>
    const $ = (id) => document.getElementById(id);

    function tampilLED(num, nyala) {
      $('st' + num).textContent = nyala ? "Nyala" : "Mati";
      $('st' + num).classList.toggle('nyala', nyala);
      $('lampu' + num).classList.toggle('nyala', nyala);
      $('on' + num).classList.toggle('aktif', nyala);
      $('off' + num).classList.toggle('aktif', !nyala);
    }

    function setKoneksi(terhubung) {
      $('koneksi').textContent = terhubung ? "Terhubung" : "Terputus";
      $('koneksi').className = 'badge ' + (terhubung ? 'ok' : 'putus');
    }

    function updateUI(data) {
      tampilLED(1, data.led1);
      tampilLED(2, data.led2);

      $('temp').textContent = data.temp === null ? "--" : data.temp.toFixed(1);
      $('hum').textContent = data.hum === null ? "--" : data.hum.toFixed(1);

      // Analog bernilai -1 / null jika kedua LED belum sama-sama menyala
      const aktif = data.analog !== null && data.analog >= 0;
      $('analog').textContent = aktif ? data.analog : "Tidak aktif";
      $('analog').classList.toggle('mati', !aktif);
      $('barAnalog').style.width = aktif ? (data.analog / 1023 * 100) + '%' : '0';
      $('noteAnalog').textContent = aktif
        ? "Analog aktif karena kedua LED menyala."
        : "Analog tidak dikirim karena kedua LED belum sama-sama menyala.";

      $('waktu').textContent = "terakhir " + new Date().toLocaleTimeString('id-ID');
      setKoneksi(true);
    }

    function setLED(num, state) {
      fetch(`/led?num=${num}&state=${state}`, { cache: 'no-store' })
        .then(res => res.json())
        .then(updateUI)
        .catch(() => setKoneksi(false));
    }

    function getData() {
      fetch('/data', { cache: 'no-store' })
        .then(res => res.json())
        .then(updateUI)
        .catch(() => setKoneksi(false));
    }

    setInterval(getData, 2000);
    getData();
  </script>
</body>
</html>
)rawliteral";

// Fungsi untuk membuat string JSON status data
String getJSONData() {
  String json = "{";
  json += "\"temp\":" + (isnan(temperatur) ? "null" : String(temperatur, 1)) + ",";
  json += "\"hum\":" + (isnan(kelembapan) ? "null" : String(kelembapan, 1)) + ",";
  json += "\"analog\":" + String(nilaiAnalog) + ",";
  json += "\"led1\":" + String(led1On ? "true" : "false") + ",";
  json += "\"led2\":" + String(led2On ? "true" : "false");
  json += "}";
  return json;
}

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleData() {
  server.send(200, "application/json", getJSONData());
}

void handleLEDControl() {
  if (server.hasArg("num") && server.hasArg("state")) {
    int num = server.arg("num").toInt();
    String state = server.arg("state");
    bool status = (state == "on");

    if (num == 1) {
      led1On = status;
      digitalWrite(LED1_PIN, led1On ? LOW : HIGH); // Active LOW
    } else if (num == 2) {
      led2On = status;
      digitalWrite(LED2_PIN, led2On ? HIGH : LOW); // Active HIGH
    }
  }
  server.send(200, "application/json", getJSONData());
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  
  // Inisialisasi status LED (Mati saat awal)
  digitalWrite(LED1_PIN, HIGH); // Active-low (HIGH = OFF)
  digitalWrite(LED2_PIN, LOW);  // Active-high (LOW = OFF)

  dht.begin();

  // Koneksi Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Terhubung!");
  Serial.print("IP Address ESP8266: ");
  Serial.println(WiFi.localIP());

  // Routing HTTP Server
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/led", handleLEDControl);

  server.begin();
  Serial.println("HTTP Web Server berjalan.");
}

void loop() {
  server.handleClient();

  // Pembacaan sensor periodik setiap 2 detik
  unsigned long sekarang = millis();
  if (sekarang - waktuSensor >= INTERVAL_SENSOR) {
    waktuSensor = sekarang;

    // Baca sensor DHT
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t)) temperatur = t;
    if (!isnan(h)) kelembapan = h;

    // Aturan Logika Analog A0:
    // Hanya dibaca/dikirim jika KEDUA LED sama-sama MENYALA
    if (led1On && led2On) {
      nilaiAnalog = analogRead(A0);
    } else {
      nilaiAnalog = -1; // Status tidak aktif
    }
  }
}