
#include <DHT.h>

#define DHTPIN D4
#define DHTTYPE DHT11

#define LED_MERAH D5
#define LED_BIRU D6

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LED_MERAH, OUTPUT);
  pinMode(LED_BIRU, OUTPUT);

  // Kondisi awal LED
  digitalWrite(LED_MERAH, LOW);
  digitalWrite(LED_BIRU, LOW);

  Serial.println("Sistem Monitoring Suhu dan Kelembapan");
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Periksa pembacaan sensor
  if (isnan(t) || isnan(h)) {
    Serial.println("Gagal membaca sensor DHT11!");

    digitalWrite(LED_MERAH, LOW);
    digitalWrite(LED_BIRU, LOW);

    delay(2000);
    return;
  }

  // Tampilkan data sensor
  Serial.print("Suhu: ");
  Serial.print(t);
  Serial.print(" C | Kelembapan: ");
  Serial.print(h);
  Serial.println(" %");

  // Prioritas 1: Kelembapan di atas 75%
  if (h > 75) {
    Serial.println("PERINGATAN: Kelembapan tinggi!");

    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(LED_BIRU, LOW);
  }

  // Prioritas 2: Suhu di atas 60 derajat
  else if (t > 60) {
    Serial.println("PERINGATAN: Suhu tinggi!");

    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(LED_BIRU, HIGH);
  }

  // Kondisi normal
  else {
    Serial.println("Status Aman");

    digitalWrite(LED_MERAH, LOW);
    digitalWrite(LED_BIRU, HIGH);
  }

  Serial.println("---------------------------");
  delay(2000);
}
