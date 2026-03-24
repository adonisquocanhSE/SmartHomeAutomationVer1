# 🏠 Smart Home Automation System (ESP32 + Blynk)

> 🚀 A complete IoT-based Smart Home system using ESP32, integrating sensors, actuators, and cloud control via Blynk.

---

## 📖 Overview
The **Smart Home Automation System** is an IoT project that allows users to monitor and control home devices remotely.  
It integrates multiple hardware components such as temperature sensors, ultrasonic sensors, LEDs, servo motors, and an LCD display.

The system supports both:
- 🎮 Manual control (via buttons & Blynk app)
- 🤖 Automatic mode (sensor-based behavior)

---

## 🎯 Features
- 🌡️ Real-time temperature monitoring (LM35)
- 📡 Ultrasonic-based person detection
- 💡 LED control (manual & auto)
- 🚪 Smart door control using Servo
- 📱 Remote control via Blynk
- 📺 LCD display for system status
- ⚡ Non-blocking system using `millis()`

---

## 🧰 Hardware Components

| Component | Description |
|----------|------------|
| ESP32 | Main microcontroller |
| LM35 | Temperature sensor |
| Ultrasonic (HC-SR04/SRF05) | Distance detection |
| LED | Lighting & alert |
| Servo Motor | Door control |
| LCD I2C | Display information |
| Push Buttons | Manual control |

---

## 🔌 Pin Configuration

| Component | ESP32 Pin |
|----------|----------|
| LED1 | GPIO 26 |
| LED2 (Alert) | GPIO 27 |
| Button1 | GPIO 14 |
| Button2 | GPIO 12 |
| TRIG | GPIO 33 |
| ECHO | GPIO 32 |
| LM35 | GPIO 34 |
| Servo | GPIO 25 |
| LCD (SDA, SCL) | GPIO 2, 4 |

---

## 🧠 System Architecture


User (Blynk App)
↓
WiFi
↓
ESP32
┌────┼────┐
↓ ↓ ↓
Sensors Actuators Display
(LM35, (LED, (LCD)
Ultra) Servo)


---

## ⚙️ Core Functionalities

### 🌡️ Temperature Monitoring
- Reads analog data from LM35
- Converts voltage → temperature
- Displays on LCD & sends to Blynk

---

### 📡 Ultrasonic Detection
- Measures distance using sound waves
- Detects if someone is near the door
- Activates LED alert system

---

### 💡 LED Control
- Manual toggle via button
- Remote control via Blynk
- Alert blinking when detecting motion

---

### 🚪 Servo Door Control
- Opens/closes door (0° / 90°)
- Controlled via:
  - Button
  - Blynk app

---

### 📺 LCD Display
- Shows:
  - Temperature
  - Door status
  - Detection message

---

## 🧾 Code Highlights

### 📡 Ultrasonic Distance Formula
```cpp
distance = duration * 0.0343 / 2;
⚡ Non-blocking LED Blink
if(millis() - ledTimer >= 200)
{
  ledTimer = millis();
  ledBlinkState = !ledBlinkState;
  digitalWrite(led2, ledBlinkState);
}
🔥 Why millis() Instead of delay()?
delay() ❌	millis() ✅
Blocks system	Non-blocking
Causes lag	Smooth operation
Stops Blynk	Works with real-time tasks
📱 Blynk Integration
Virtual Pin	Function
V1	LED Control
V2	Servo Control
V6	Temperature
V10	Auto Mode
🔁 System Workflow
ESP32 connects to WiFi & Blynk
Reads sensor data (temperature + distance)
Updates LCD & sends data to Blynk
Receives commands from user
Controls LED & Servo
Auto mode triggers alerts
🚀 Demo Scenario
When temperature changes → display & update on app
When user approaches door → LED blinks + LCD alert
User can open/close door via Blynk
System runs smoothly without delay
🧠 Technical Highlights
IoT-based architecture
Real-time processing
Non-blocking programming
Multi-module integration
Sensor + Cloud + Actuator combination
💡 Future Improvements
🔊 Add buzzer alarm
📸 Integrate camera module
☁️ Store data to cloud database
📊 Add analytics dashboard
🤖 AI-based automation
👨‍💻 Author
Phạm Quốc Anh
FPT University – IoT Project
📌 Conclusion

This project demonstrates a complete Smart Home system combining hardware and software.
It provides a scalable and efficient solution for real-time home automation using ESP32 and IoT technologies.
