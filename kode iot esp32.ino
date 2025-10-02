#include <WiFi.h>
#include <ThingSpeak.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== DETAIL KONEKSI ANDA =====
const char* ssid = "my-ITS-WIFI";       // <--- GANTI dengan SSID WiFi Anda
const char* password = "itssurabaya"; // <--- GANTI dengan Password WiFi Anda

unsigned long myChannelNumber = 3097544;         // <--- GANTI dengan Channel ID ThingSpeak Anda
const char* myWriteAPIKey = "7FY8A5VK7934GOXS"; // Write API Key

// ===== PENGATURAN PIN =====
Servo myServo;
int servoPin = 23;      // Servo ke IO23
int buzzPin = 25;        // Buzzer ke IO5
int ledPin  = 26;       // LED ke IO26

// Pin untuk Ultrasonik
const int trigPin = 18; // Trig ke IO18
const int echoPin = 33; // Echo ke IO19

// ===== PENGATURAN THINGSPEAK TIMER =====
WiFiClient client;
unsigned long lastThingSpeakUpdate = 0;
const unsigned long thingspeakInterval = 20000; // Kirim setiap 20 detik (min. 15 detik)

void setup() {
  Serial.begin(115200); // Gunakan baud rate tinggi

  // Servo
  myServo.attach(servoPin);
  myServo.write(90); // posisi awal

  // Ultrasonik
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // LED & Buzzer
  pinMode(ledPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);

  // KONEKSI WIFI
  if (WiFi.status() != WL_CONNECTED) {
  Serial.println("WiFi terputus, mencoba reconnecting....");
  WiFi.reconnect();
}

  Serial.println("\nWiFi Terkoneksi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // INISIALISASI THINGSPEAK
  ThingSpeak.begin(client);
}

void loop() {
  unsigned long now = millis();
  long duration;
  long distance;

  // Trigger pulse untuk Ultrasonik
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Baca echo dengan timeout 30ms
  duration = pulseIn(echoPin, HIGH, 30000);
  distance = duration * 0.034 / 2;

  // Kondisi LED & buzzer
  if (distance <= 10 && distance > 0) { // Pastikan bukan nilai error (0)
    Serial.print("❌ Jarak: ");
    Serial.print(distance);
    Serial.println(" cm. PERINGATAN!");

    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzPin, LOW);  // Anggap LOW = ON
  } else {
    Serial.print("✅ Jarak: ");
    Serial.print(distance);
    Serial.println(" cm");

    digitalWrite(ledPin, LOW);
    digitalWrite(buzzPin, HIGH); // Anggap HIGH = OFF
  }

  // ===== KIRIM DATA KE THINGSPEAK =====
  if (now - lastThingSpeakUpdate >= thingspeakInterval) {
    Serial.println("Mengirim data ke ThingSpeak...");

    // Kirim Jarak ke Field 1
    int statusCode = ThingSpeak.writeField(myChannelNumber, 1, distance, myWriteAPIKey);

    if (statusCode == 200) {
      Serial.println("Data Jarak terkirim ke ThingSpeak (Field 1)");
    } else {
      Serial.print("Gagal kirim ke ThingSpeak. Kode: ");
      Serial.println(statusCode);
    }

    lastThingSpeakUpdate = now;
  }

  delay(500); // Jeda pembacaan utama
}  