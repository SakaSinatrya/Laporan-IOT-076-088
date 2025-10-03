## Anggota Kelompok
| No | Nama                       | NRP         |
|----|----------------------------|-------------|
| 1  | Dimas Muhammad Putra       | 5027241076  |
| 2  | I Gede Bagus Saka Sinatrya |	5027241088  |

## Pendahuluan
Di sini kami membuat alat iot dengan board esp32. Pada alat ini kami menggunakan sensor ultrasonic, lampu led, dan buzzer. Sensor ultrasonic digunakan untuk mengukur jarak, jadi semakin dekat jarak yang di ukur maka lampu led dan buzzer akan menyala sedangkan jika jaraknya lebih dari 10 cm maka lampu led akan mati dan buzzer akan berhenti berbunyi.
## Dokumentasi Alat
![Image](https://github.com/user-attachments/assets/779f6e83-cbb5-4df8-9b38-f9b873a588ce)

## Penjelasan kode
### library yang digunakan
```
#include <WiFi.h>
#include <ThingSpeak.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
```
- WiFi.h → untuk koneksi ESP32 ke jaringan WiFi.
- ThingSpeak.h → library untuk mengirim data sensor ke server IoT ThingSpeak.
- ESP32Servo.h → mengontrol motor servo dengan sinyal PWM.
- Wire.h, Adafruit_GFX.h, Adafruit_SSD1306.h → library untuk menghubungkan dan mengontrol OLED display (walau di kode ini OLED belum dipakai).

### Detail Koneksi WiFi & ThingSpeak
```
const char* ssid = "my-ITS-WIFI";       
const char* password = "itssurabaya"; 

unsigned long myChannelNumber = 3097544;        
const char* myWriteAPIKey = "7FY8A5VK7934GOXS"; 
```
- ssid & password → nama WiFi dan password agar ESP32 bisa terhubung.
- myChannelNumber → ID channel dari ThingSpeak (unik untuk setiap user).
- myWriteAPIKey → API Key agar ESP32 bisa menulis data ke channel tersebut.

### Deklarasi Pin
```
Servo myServo;
int servoPin = 23;      
int buzzPin = 25;       
int ledPin  = 26;       

const int trigPin = 18; 
const int echoPin = 33; 
```
- servoPin (23) → pin kontrol untuk motor servo.
- buzzPin (25) → buzzer untuk alarm.
- ledPin (26) → LED indikator.
- trigPin (18) & echoPin (33) → pin untuk sensor ultrasonik HC-SR04.

### Timer ThingSpeak
```
WiFiClient client;
unsigned long lastThingSpeakUpdate = 0;
const unsigned long thingspeakInterval = 20000; // 20 detik
```
- client → objek WiFi untuk komunikasi dengan ThingSpeak.
- lastThingSpeakUpdate → mencatat waktu terakhir kirim data.
- thingspeakInterval → jeda antar pengiriman data (minimal 15 detik sesuai aturan ThingSpeak).

### Fungsi setup()
```
void setup() {
  Serial.begin(115200); 
  myServo.attach(servoPin);
  myServo.write(90); 

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(ledPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi terputus, mencoba reconnecting....");
    WiFi.reconnect();
  }

  Serial.println("\nWiFi Terkoneksi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);
}
```
- Serial.begin(115200) → untuk debugging di Serial Monitor.
- myServo.attach() → menghubungkan servo ke pin 23. Posisi awal di set ke 90°.
- pinMode() → konfigurasi pin input/output.
- WiFi.status() → cek koneksi WiFi, jika terputus → reconnect.
- ThingSpeak.begin(client) → inisialisasi komunikasi ke server ThingSpeak.

### Fungsi loop()
```
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

```
- millis() → menghitung waktu sejak ESP32 menyala (ms).
- trigPin HIGH 10 µs → memicu sensor ultrasonik.
- pulseIn() → membaca lama pantulan (echo).
- distance = duration * 0.034 / 2 → konversi waktu ke jarak (cm).

### Logika LED & Buzzer
```
  if (distance <= 10 && distance > 0) {
    Serial.print("❌ Jarak: ");
    Serial.print(distance);
    Serial.println(" cm. PERINGATAN!");

    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzPin, LOW);  
  } else {
    Serial.print("✅ Jarak: ");
    Serial.print(distance);
    Serial.println(" cm");

    digitalWrite(ledPin, LOW);
    digitalWrite(buzzPin, HIGH); 
  }

```
- Jika jarak ≤ 10 cm → LED nyala, buzzer aktif.
- Jika jarak > 10 cm → LED mati, buzzer nonaktif.
- LOW = ON dan HIGH = OFF karena buzzer dihubungkan dengan logika terbalik.

### Mengirim Data ke ThingSpeak
```
   if (now - lastThingSpeakUpdate >= thingspeakInterval) {
    Serial.println("Mengirim data ke ThingSpeak...");

    int statusCode = ThingSpeak.writeField(myChannelNumber, 1, distance, myWriteAPIKey);

    if (statusCode == 200) {
      Serial.println("Data Jarak terkirim ke ThingSpeak (Field 1)");
    } else {
      Serial.print("Gagal kirim ke ThingSpeak. Kode: ");
      Serial.println(statusCode);
    }

    lastThingSpeakUpdate = now;
  }

```
- Setiap 20 detik sekali, data jarak dikirim ke ThingSpeak Field 1.
- Jika berhasil → status 200 (OK).
- Jika gagal → tampilkan kode error di Serial Monitor.

### Delay loop
```
  delay(500); // Jeda pembacaan utama
}
```
- Memberi jeda 0.5 detik antar pembacaan sensor ultrasonik.

## Dokumentasi Video Hasil
https://github.com/user-attachments/assets/323fba15-b239-4ca5-8c62-ed2c41d4977f

## Hasli Pengukuran Thingspeak
![Image](https://github.com/user-attachments/assets/4f54426c-8f50-47d3-8766-09b2551aef20)


