// ==========================================
// LED SHIELD ESP8266
// GPIO 13 = LED1
// ==========================================

#define LED_PIN 13

void setup() {
  pinMode(LED_PIN, OUTPUT);

  // Pastikan LED mati saat awal
  digitalWrite(LED_PIN, LOW);
}

void loop() {

  // ========================================
  // POLA 1 - NYALA 1 DETIK
  // ========================================

  digitalWrite(LED_PIN, HIGH);
  delay(1000);

  digitalWrite(LED_PIN, LOW);
  delay(500);


  // ========================================
  // POLA 2 - KEDIP CEPAT 3X
  // ========================================

  for (int i = 0; i < 3; i++) {

    digitalWrite(LED_PIN, HIGH);
    delay(200);

    digitalWrite(LED_PIN, LOW);
    delay(200);
  }


  // ========================================
  // JEDA
  // ========================================

  delay(1000);


  // ========================================
  // POLA 3 - KEDIP LAMBAT 2X
  // ========================================

  for (int i = 0; i < 2; i++) {

    digitalWrite(LED_PIN, HIGH);
    delay(700);

    digitalWrite(LED_PIN, LOW);
    delay(700);
  }


  // ========================================
  // JEDA SEBELUM MENGULANG
  // ========================================

  delay(1500);
}