# 🌐 Adaptive Environmental Control System
### Arduino Uno R3 Embedded Systems Project

---

## 📌 Overview

This project implements a **menu-driven embedded control system** using an Arduino Uno R3 that monitors environmental conditions and automatically controls outputs based on sensor data and user-defined modes.

The system integrates:
- Real-time sensor monitoring (light, temperature, motion)
- A structured user interface (LCD + joystick + buttons)
- Automated and manual control logic
- Physical actuation (LEDs, buzzer, DC motor fan, servo)

The goal is to simulate a **modular environmental control system** similar to those used in industrial, smart infrastructure, and energy optimization applications.

---

## 🎯 Key Features

- 📟 **Menu-driven UI**
  - 16x2 LCD display
  - Joystick navigation (up/down)
  - Select and back buttons

- 🌡️ **Environmental sensing**
  - Light (LDR)
  - Temperature & humidity (DHT11)
  - Motion detection (PIR)

- ⚙️ **Automation modes**
  - AUTO → sensor-driven behavior
  - MANUAL → user-controlled outputs
  - ECO → energy-optimized logic

- 🔌 **Actuation**
  - Multi-zone lighting (LEDs)
  - Audible alerts (buzzer)
  - DC motor fan (transistor-driven)
  - Servo motor (position control)

---

## 🧠 System Architecture

The system is designed as a **layered embedded architecture**:

### 1. Input Layer
Handles all user interaction and navigation:
- Joystick (analog input)
- Buttons (digital input)

### 2. Sensor Layer
Collects environmental data:
- LDR → ambient light level
- DHT11 → temperature & humidity
- PIR → motion detection

### 3. Control Layer
Core decision-making logic:
- Mode handling (AUTO / MANUAL / ECO)
- Threshold-based automation
- Event detection (e.g., motion alerts)

### 4. Output Layer
Executes system actions:
- LEDs (lighting control)
- DC motor (fan simulation)
- Servo motor (mechanical actuation)
- Buzzer (alerts)

### 5. UI Layer
Displays system state and allows configuration:
- LCD rendering
- Menu navigation
- Settings adjustment

---

## 🔄 Control Flow

The system follows a structured main loop:

```cpp
loop() {
  readInputs();
  readSensors();
  updateMenu();
  runControlLogic();
  updateOutputs();
  updateDisplay();
}
