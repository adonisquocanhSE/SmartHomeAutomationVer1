#include "wifiConfig.h"

#define BLYNK_TEMPLATE_ID "TMPL6XHJFf2l7"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#define BLYNK_AUTH_TOKEN "3P2IHsisqvrvYuxn-QBX4lcH7w6sYzYG"

#include <BlynkSimpleEsp32.h>

bool blynkConnect = 0;

// ====== RELAY ======
const int relay1 = 26;
const int relay2 = 27;

// ====== BUTTON ======
const int button1 = 14;
const int button2 = 12;

// ====== STATE ======
bool state1 = 0;
bool state2 = 0;

// ====== DEBOUNCE ======
unsigned long lastPress1 = 0;
unsigned long lastPress2 = 0;
const int debounceTime = 200;

// ================= BLYNK =================
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V1, V2);
}

BLYNK_WRITE(V1) {
  state1 = param.asInt();
  digitalWrite(relay1, state1);
}

BLYNK_WRITE(V2) {
  state2 = param.asInt();
  digitalWrite(relay2, state2);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  wifiConfig.begin();

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);

  digitalWrite(relay1, state1);
  digitalWrite(relay2, state2);

  Blynk.config(BLYNK_AUTH_TOKEN, "blynk.cloud", 80);
}

// ================= LOOP =================
void loop() {

  wifiConfig.run();

  // ===== WIFI OK =====
  if (WiFi.status() == WL_CONNECTED) {

    // ===== KẾT NỐI BLYNK =====
    if (blynkConnect == 0) {
      Serial.println("Connecting to Blynk...");
      if (Blynk.connect(3000)) {
        Serial.println("Blynk Connected!");
        blynkConnect = 1;
      } else {
        Serial.println("Blynk Failed!");
      }
    }

    if (!Blynk.connected()) blynkConnect = 0;

    Blynk.run();
  }

  // ===== BUTTON 1 =====
  if (digitalRead(button1) == LOW) {
    if (millis() - lastPress1 > debounceTime) {
      state1 = !state1;
      digitalWrite(relay1, state1);
      Blynk.virtualWrite(V1, state1);
      lastPress1 = millis();
    }
  }

  // ===== BUTTON 2 =====
  if (digitalRead(button2) == LOW) {
    if (millis() - lastPress2 > debounceTime) {
      state2 = !state2;
      digitalWrite(relay2, state2);
      Blynk.virtualWrite(V2, state2);
      lastPress2 = millis();
    }
  }
}