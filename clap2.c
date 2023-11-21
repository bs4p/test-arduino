#define BLYNK_TEMPLATE_ID "TMPL6ujEANOxU"
#define BLYNK_TEMPLATE_NAME "kontrol lampu"
#define BLYNK_AUTH_TOKEN "ZK6truArNyM7uNgm8FkmZYeF1VhgncCV"

#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define BLYNK_PRINT Serial

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include "RTClib.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int sensorPin = 14;  // Pin sensor suara KY-037 terhubung ke pin analog A0
const int ledpin1 = 13;    // Pin LED terhubung ke pin digital 13
const int pinldr = 35;
const int ledpin2 = 12;
const int ledpin3 = 4;

int cahaya = 1200;

int clapCount = 0;  // Variabel untuk menghitung jumlah tepukan

int lastSensorState = LOW;

char auth[] = "ZK6truArNyM7uNgm8FkmZYeF1VhgncCV";
char ssid[] = "INKADE";
char pass[] = "Shakeyra2002";

RTC_DS3231 rtc;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
File file;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  pinMode(sensorPin, INPUT);
  pinMode(ledpin1, OUTPUT);
  pinMode(pinldr, INPUT);
  pinMode(ledpin2, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  // delay(jeda2);
  // display.clearDisplay();
  display.clearDisplay();
  Serial.println("RUMAH");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 14);
  display.print("SMART LAMP");
  // display.setTextSize(1);
  display.setCursor(5, 24);
  display.print("Hakim, Hagai, Evline");
  display.display();
  delay(2000);

  display.clearDisplay();

  if (!rtc.begin()) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Couldn't find RTC");
    display.display();
    return;
  }

  if (!SD.begin(5)) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Couldn't find SD");
    display.display();
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("Tidak Ada Kartu SD");
    return;
  }
  Serial.println("Menginisialisasi kartu SD...");

  file = SD.open("/lampu.txt", FILE_WRITE);
  rtc.adjust(DateTime(F(DATE), F(__TIME)));
}

void loop() {
  Blynk.run();
  display.clearDisplay();
  DateTime now = rtc.now();

  display.setCursor(0, 0);  //oled display
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print(now.day(), DEC);
  display.print('/');
  display.print(now.month(), DEC);
  display.print('/');
  display.print(now.year(), DEC);
  display.print(' ');
  display.print(now.hour(), DEC);
  display.print(':');
  display.print(now.minute(), DEC);
  display.print(':');
  display.print(now.second(), DEC);
  display.println();
  display.display();
  file.print(now.day(), DEC);
  file.print('/');
  file.print(now.month(), DEC);
  file.print('/');
  file.print(now.year(), DEC);
  file.print(' ');
  file.print(now.hour(), DEC);
  file.print(':');
  file.print(now.minute(), DEC);
  file.print(':');
  file.print(now.second(), DEC);
  file.println(' ');


  int reading = digitalRead(sensorPin);
  int ldrValue = analogRead(pinldr);
  Serial.println(ldrValue);
  Serial.println(reading);

  // jika detek SENSOR SUARA
  if (reading > 0) {
    clapCount++;

    if (clapCount == 1) {
      // Tepukkan satu kali untuk menyalakan lampu
      digitalWrite(ledpin1, LOW);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(4, 10);
      display.println("LAMPU 1 : HIDUP");
      display.display();
      file.println("lampu 1 : hidup");
      Serial.println("Lampu dinyalakan");
      lastSensorState = LOW // update lastSensorState
    } else {
      // Tepukkan dua kali untuk mematikan lampu
      digitalWrite(ledpin1, HIGH);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(4, 10);
      display.println("LAMPU 1 : MATI");
      display.display();
      file.println("lampu 1 : mati");
      Serial.println("Lampu dimatikan");
      clapCount = 0;  // Reset hitungan
      lastSensorState = HIGH // update lastSensorState
    }
    // delay(1000);  // Tunggu beberapa saat sebelum mendeteksi tepukan berikutnya
  }
  // delay(1000);  // Tunggu beberapa saat sebelum mendeteksi tepukan berikutnya
  else { // kalau takda detek 
    if(lastSensorState == LOW) {
      digitalWrite(ledpin1, LOW);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(4, 10);
      display.println("LAMPU 1 : HIDUP");
      display.display();
      file.println("lampu 1 : hidup");
    } else {
      digitalWrite(ledpin1, HIGH);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(4, 10);
      display.println("LAMPU 1 : MATI");
      display.display();
      file.println("lampu 1 : mati");
    }
  }


  //SENSOR LDR
  if (ldrValue > cahaya) {
    digitalWrite(ledpin2, HIGH);  // Aktifkan relay
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(4, 14);
    display.println("LAMPU 2 : MATI");
    display.display();
    Serial.println("terang");
  } else {
    digitalWrite(ledpin2, LOW);  // Matikan relay
    display.setTextColor(WHITE);
    display.setCursor(4, 14);
    display.println("LAMPU 2 : HIDUP");
    display.display();
    Serial.println("gelap");
  }

  delay(100);  // Delay untuk stabilitas pembacaan
  display.display();


  // if (reading > 0) {
  //   clapCount++;
  //   if (clapCount == 1) {
  //     file.println("LAMPU 1 : HIDUP");
  //   } else if (clapCount == 2) {
  //     file.println("LAMPU 1 : MATI");
  //     clapCount = 0;  // Reset hitungan
  //   }
  //   delay(1000);  // Tunggu beberapa saat sebelum mendeteksi tepukan berikutnya
  // }
  if (ldrValue > cahaya) {
    file.println("LAMPU 2 : MATI");
  } else {
    file.println("LAMPU 2 : HIDUP");
  }
  file.flush();
}