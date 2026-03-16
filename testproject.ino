#include "wifiConfig.h"
#define BLYNK_TEMPLATE_ID "TMPL6dzICp2yC"
#define BLYNK_TEMPLATE_NAME "SmartHome"
#define BLYNK_AUTH_TOKEN "SoQcwvvvE09Ln8-JyPePQ2gQyJ3bydna"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27,16,2);
Servo doorServo;

// ===== MODE =====
bool autoMode = 0;

// ===== LED =====
const int led1 = 26;
const int led2 = 27;

// ===== BUTTON =====
const int button1 = 14;
const int button2 = 12;

// ===== ULTRASONIC =====
const int trigPin = 33;
const int echoPin = 32;

// ===== LM35 =====
const int LM35_PIN = 34;

// ===== SERVO =====
const int servoPin = 15;

// ===== STATE =====
bool ledState = 0;
bool servoState = 0;

unsigned long lastTempRead = 0;
unsigned long lastDistanceRead = 0;

unsigned long lastPress1 = 0;
unsigned long lastPress2 = 0;

const int debounceTime = 200;

int detectDistance = 80;

// ================= BLYNK =================

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V1,V2,V10);
}

BLYNK_WRITE(V1)
{
  ledState = param.asInt();
  digitalWrite(led1, ledState);
}

BLYNK_WRITE(V2)
{
  servoState = param.asInt();

  if(servoState)
    doorServo.write(90);
  else
    doorServo.write(0);
}

BLYNK_WRITE(V10)
{
  autoMode = param.asInt();
}

// ================= ULTRASONIC =================

float getDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH,30000);

  if(duration==0) return -1;

  float distance = duration * 0.0343 / 2;

  return distance;
}

// ================= TEMPERATURE =================

float readTemperature()
{
  int adc = analogRead(LM35_PIN);

  float voltage = adc * 3.3 / 4095.0;

  float temp = voltage * 100;

  return temp;
}

void sendTemperature()
{
  if(millis() - lastTempRead < 2000) return;

  lastTempRead = millis();

  float temp = readTemperature();

  Blynk.virtualWrite(V6,temp);

  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.print(temp);
  lcd.print("C   ");
}

// ================= AUTO MODE =================

void checkUltrasonic()
{
  if(millis() - lastDistanceRead < 300) return;

  lastDistanceRead = millis();

  float d = getDistance();

  if(d > 0 && d < detectDistance)
  {
    lcd.setCursor(0,1);
    lcd.print("Someone at door");

    digitalWrite(led2,HIGH);
    delay(200);
    digitalWrite(led2,LOW);
  }
  else
  {
    lcd.setCursor(0,1);
    lcd.print("Door clear      ");
  }
}

// ================= SETUP =================

void setup()
{
  Serial.begin(9600);

  wifiConfig.begin();

  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);

  pinMode(button1,INPUT_PULLUP);
  pinMode(button2,INPUT_PULLUP);

  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  // LCD I2C chân mới
  Wire.begin(2,4);

  lcd.init();
  lcd.backlight();

  doorServo.attach(servoPin);

  Blynk.config(BLYNK_AUTH_TOKEN,"blynk.cloud",80);

  lcd.setCursor(0,0);
  lcd.print("Smart Home");
}

// ================= LOOP =================

void loop()
{
  wifiConfig.run();

  if(WiFi.status()==WL_CONNECTED)
  {
    Blynk.run();
  }

  // ===== BUTTON LED =====
  if(digitalRead(button1)==LOW && millis()-lastPress1>debounceTime)
  {
    ledState = !ledState;

    digitalWrite(led1,ledState);

    Blynk.virtualWrite(V1,ledState);

    lastPress1 = millis();
  }

  // ===== BUTTON SERVO =====
  if(digitalRead(button2)==LOW && millis()-lastPress2>debounceTime)
  {
    servoState = !servoState;

    if(servoState)
      doorServo.write(90);
    else
      doorServo.write(0);

    Blynk.virtualWrite(V2,servoState);

    lastPress2 = millis();
  }

  // ===== TEMPERATURE =====
  sendTemperature();

  // ===== AUTO MODE =====
  if(autoMode)
  {
    checkUltrasonic();
  }
}