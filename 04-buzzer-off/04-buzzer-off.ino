// Menentukan pin buzzer yang terhubung ke D5
#define BUZZER_PIN D5

void setup() {
  // Mengatur pin buzzer sebagai output
  pinMode(BUZZER_PIN, OUTPUT);

  // Menghentikan nada jika buzzer menggunakan fungsi tone()
  noTone(BUZZER_PIN);

  // Mengatur pin ke kondisi LOW untuk mematikan buzzer
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  // Memastikan nada buzzer dihentikan secara berulang
  noTone(BUZZER_PIN);

  // Memastikan pin buzzer tetap dalam kondisi LOW
  digitalWrite(BUZZER_PIN, LOW);
}