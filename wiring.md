# 🔌 Wiring Guide
## Adaptive Environmental Control System (Arduino Uno R3)

---

## ⚡ Power Distribution

- Arduino **5V → Breadboard + rail**
- Arduino **GND → Breadboard – rail**

All components draw power from these rails unless specified otherwise.

---

## 📟 LCD Display (I2C)

| LCD Pin | Arduino |
|--------|--------|
| GND | GND |
| VCC | 5V |
| SDA | A4 |
| SCL | A5 |

---

## 🎮 Joystick Module (Menu Navigation)

| Joystick Pin | Arduino |
|-------------|--------|
| GND | GND |
| VCC | 5V |
| VRy (Vertical) | A2 |
| VRx (Horizontal) | A3 (optional) |
| SW (Click) | Not used |

---

## 🔘 Buttons (User Input)

### SELECT Button
- One leg → **5V**
- Other leg → **Pin 6**
- 10kΩ resistor → from Pin 6 to GND

### BACK Button
- One leg → **5V**
- Other leg → **Pin 7**
- 10kΩ resistor → from Pin 7 to GND

---

## 🌞 Light Sensor (LDR)

Voltage Divider:

- LDR → 5V  
- LDR other leg → **A0**
- 10kΩ resistor → from A0 to GND

---

## 🌡️ Temperature Sensor

### Option A: LM35

| Pin | Arduino |
|-----|--------|
| VCC | 5V |
| GND | GND |
| OUT | A1 |

### Option B: DHT11

| Pin | Arduino |
|-----|--------|
| VCC | 5V |
| GND | GND |
| DATA | Pin 10 |

---

## 🚶 PIR Motion Sensor

| Pin | Arduino |
|-----|--------|
| VCC | 5V |
| GND | GND |
| OUT | Pin 13 |

---

## 💡 LEDs (Lighting System)

Each LED:
- Anode → Arduino pin  
- Cathode → 220Ω resistor → GND  

| Function | Pin |
|---------|----|
| Light 1 | 2 |
| Light 2 | 3 |
| Light 3 | 4 |
| Fan Indicator | 5 |

---

## 🔊 Buzzer

| Pin | Arduino |
|-----|--------|
| + | Pin 8 |
| – | GND |

---

## ⚡ Relay Module

| Pin | Arduino |
|-----|--------|
| VCC | 5V |
| GND | GND |
| IN | Pin 9 |

---

## ⚙️ Servo Motor

| Wire Color | Arduino |
|-----------|--------|
| Red | 5V |
| Brown/Black | GND |
| Yellow/Orange (Signal) | Pin 10 |

⚠️ Note: If servo behaves erratically, use external 5V supply.

---

## 🧠 Pin Summary

| Component | Pin |
|----------|----|
| LCD (I2C) | A4, A5 |
| Joystick Y | A2 |
| Joystick X (optional) | A3 |
| LDR | A0 |
| Temp Sensor | A1 |
| Select Button | 6 |
| Back Button | 7 |
| Buzzer | 8 |
| Relay | 9 |
| Servo / DHT11 | 10 |
| LEDs | 2–5 |
| PIR | 13 |

---

## ⚠️ Notes

- Do **not** power high-load devices directly from Arduino if unstable
- Ensure **common ground** across all components
- Avoid pin conflicts (especially Pin 10 if using both servo and DHT11)

---

## ✅ Build Order (Recommended)

1. LCD (I2C test)
2. Joystick input
3. Buttons
4. One LED output
5. Sensors (LDR, Temp, PIR)
6. Buzzer
7. Relay
8. Servo

---

## 🎯 System Overview

This wiring supports:
- Menu-driven UI (LCD + joystick + buttons)
- Sensor-based automation (light, temperature, motion)
- Actuation (LEDs, buzzer, relay, servo)
- Multiple system modes (Auto / Manual / Eco)

---
