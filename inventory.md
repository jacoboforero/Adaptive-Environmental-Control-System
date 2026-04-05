# 📦 Project Hardware Inventory
## Adaptive Environmental Control System (Arduino Uno R3)

---

## 🔌 Core Controller
- Arduino Uno R3
- USB cable (power + programming)

---

## 📟 User Interface (UI Components)
- 16x2 LCD Display (HD44780 compatible)
- I2C LCD Adapter (optional, if used)
- Potentiometer (10kΩ) — LCD contrast / input control
- Push Buttons (×4)
  - Up
  - Down
  - Select
  - Back
- Resistors (220Ω ×4–6) — button pull-down / LED current limiting

---

## 🌡️ Sensors (Environmental Inputs)

### Light Detection
- LDR (Photoresistor)
- Resistor (10kΩ) — voltage divider

### Temperature Monitoring
- LM35 Temperature Sensor  
  **or**
- DHT11 Temperature & Humidity Sensor

### Motion Detection
- PIR Motion Sensor (HC-SR501)

---

## 💡 Actuators (Outputs)

### Lighting System
- LEDs (×3–5) — simulate lighting zones
- Resistors (220Ω ×3–5)

### Alert System
- Active Buzzer

### Cooling System Simulation
- LED (×1) — fan indicator  
  **or**
- DC Motor (if included)

---

## ⚡ Control & Expansion Modules

### Power Control
- Relay Module (1-channel or multi-channel)

### Mechanical Actuation
- Servo Motor (e.g., SG90)

---

## 🔗 Prototyping Hardware
- Breadboard
- Jumper Wires (Male–Male, Male–Female if needed)

---

## 🧠 System Capability Mapping

| Subsystem        | Components Used                          |
|----------------|------------------------------------------|
| User Interface | LCD, Buttons, Potentiometer              |
| Lighting       | LEDs, PWM (Arduino pins)                 |
| Climate        | Temperature Sensor, LED/Motor            |
| Security       | PIR Sensor, Buzzer                       |
| Automation     | Arduino Uno (control logic)              |
| Power Control  | Relay Module                            |
| Actuation      | Servo Motor                             |

---

## 📌 Notes
- All components are sourced from a standard Arduino Uno R3 starter kit
- No external modules or third-party hardware required
- System designed for full local (offline) embedded control
