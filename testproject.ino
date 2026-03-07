#include "wifiConfig.h"
#define BLYNK_TEMPLATE_ID "TMPL6dzICp2yC"
#define BLYNK_TEMPLATE_NAME "SmartHome"
#define BLYNK_AUTH_TOKEN "SoQcwvvvE09Ln8-JyPePQ2gQyJ3bydna"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

bool blynkConnect = 0;
bool autoMode = 0;

// ===== RELAY =====
const int relay1 = 26;
const int relay2 = 27;

// ===== BUTTON =====
const int button1 = 14;
const int button2 = 12;

// ===== HY-SRF05 =====
const int trigPin = 33;
const int echoPin = 32;

// ===== STATE =====
bool state1 = 0;
bool state2 = 0;

float previousDistance = 200;

unsigned long lastPress1 = 0;
unsigned long lastPress2 = 0;
const int debounceTime = 200;

const int detectDistance = 80; // cm

// ================= BLYNK =================
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V1, V2, V10);
}

BLYNK_WRITE(V1) {
  state1 = param.asInt();
  digitalWrite(relay1, state1);
}

BLYNK_WRITE(V2) {
  state2 = param.asInt();
  digitalWrite(relay2, state2);
}

BLYNK_WRITE(V10) {
  autoMode = param.asInt();

  if(autoMode){
    Serial.println("AUTO MODE ON");
  }else{
    Serial.println("AUTO MODE OFF");
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  wifiConfig.begin();

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(relay1, state1);
  digitalWrite(relay2, state2);

  Blynk.config(BLYNK_AUTH_TOKEN, "blynk.cloud", 80);
}

// ====== ĐO KHOẢNG CÁCH ======
float getDistance() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) return -1;

  float distance = duration * 0.0343 / 2;

  return distance;
}

// ====== XỬ LÝ SIÊU ÂM (CHỈ BẬT, KHÔNG TẮT) ======
void checkUltrasonic() {

  static unsigned long lastRead = 0;

  if (millis() - lastRead < 200) return;
  lastRead = millis();

  float currentDistance = getDistance();

  if (currentDistance > 0) {

    // Chỉ bật khi đang tắt
    if (currentDistance < detectDistance &&
        previousDistance >= detectDistance &&
        state1 == 0) {

      state1 = 1;
      digitalWrite(relay1, state1);
      Blynk.virtualWrite(V1, state1);

      Serial.println("Motion detected -> Light ON");
    }

    previousDistance = currentDistance;
  }

  static unsigned long lastPrint = 0;

if (millis() - lastPrint > 500) {
  float d = getDistance();

  if (d > 0) {
    Serial.print("Distance: ");
    Serial.print(d);
    Serial.println(" cm");
  } else {
    Serial.println("Out of range");
  }

  lastPrint = millis();
}
}

// ================= LOOP =================
void loop() {

  wifiConfig.run();

  if (WiFi.status() == WL_CONNECTED) {

    if (blynkConnect == 0) {
      if (Blynk.connect(3000)) {
        blynkConnect = 1;
        Serial.println("Blynk Connected");
      }
    }

    if (!Blynk.connected()) blynkConnect = 0;

    Blynk.run();
  }

  // ===== BUTTON 1 =====
  if (digitalRead(button1) == LOW && millis() - lastPress1 > debounceTime) {
    state1 = !state1;
    digitalWrite(relay1, state1);
    Blynk.virtualWrite(V1, state1);
    lastPress1 = millis();
  }

  // ===== BUTTON 2 =====
  if (digitalRead(button2) == LOW && millis() - lastPress2 > debounceTime) {
    state2 = !state2;
    digitalWrite(relay2, state2);
    Blynk.virtualWrite(V2, state2);
    lastPress2 = millis();
  }

  // ===== ULTRASONIC =====
  if(autoMode){
    checkUltrasonic();
  }
}