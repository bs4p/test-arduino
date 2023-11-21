#define SENSOR_PIN 14  // Pin untuk sensor FC-04
#define RELAY_PIN 13   // Pin untuk relay

int sensorState = LOW;  // Menyimpan status sensor (HIGH atau LOW)
int lastSensorState = LOW;  // Menyimpan status sensor sebelumnya
unsigned long lastDebounceTime = 0;  // Waktu terakhir tepukan terdeteksi
unsigned long debounceDelay = 50;  // Waktu penundaan untuk debouncing

void setup() {
  pinMode(SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Inisialisasi relay mati
  Serial.begin(115200);
}

void loop() {
  int reading = digitalRead(SENSOR_PIN);
  Serial.println(reading);

  // Lakukan debouncing
  if (reading != lastSensorState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Update status sensor jika sudah melewati waktu debouncing
    if (reading != sensorState) {
      sensorState = reading;

      // Tepukan terdeteksi
      if (sensorState == HIGH) {
        handleClap();
      }
    }
  }

  lastSensorState = reading;
}

void handleClap() {
  static unsigned long lastClapTime = 0;
  static int clapCount = 0;

  // Jeda waktu antara tepukan
  if (millis() - lastClapTime > 500) {
    clapCount++;

    if (clapCount == 1) {
      // Tepuk satu kali, nyalakan lampu
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Lampu dinyalakan");
    } else if (clapCount == 2) {
      // Tepuk dua kali, matikan lampu
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Lampu dimatikan");
    }

    lastClapTime = millis();
  }

  // Reset hitungan tepukan setelah 2 detik tidak ada tepukan
  if (millis() - lastClapTime > 2000) {
    clapCount = 0;
  }
}