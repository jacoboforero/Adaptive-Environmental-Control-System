# 🔌 Wiring Guide (FINAL — Parallel LCD Version)
## Adaptive Environmental Control System (Arduino Uno R3)

---

## ⚡ Power Distribution

- Arduino **5V → Breadboard + rail**
- Arduino **GND → Breadboard – rail**

All components share common power and ground.

---

## 📟 LCD Display (16x2, 4-bit mode — EXISTING, KEEP THIS)

| LCD Pin | Arduino |
|--------|--------|
| VSS | GND |
| VDD | 5V |
| VO | Potentiometer middle pin |
| RS | Pin 12 |
| RW | GND ⚠️ IMPORTANT |
| E  | Pin 11 |
| D4 | Pin 5 |
| D5 | Pin 4 |
| D6 | Pin 3 |
| D7 | Pin 2 |
| A (LED+) | 5V (through 220Ω resistor) |
| K (LED–) | GND |

### Potentiometer (LCD contrast)
- Left → GND  
- Right → 5V  
- Middle → VO  

---

## 🎮 Joystick (Menu Navigation)

| Joystick Pin | Arduino |
|-------------|--------|
| GND | GND |
| VCC | 5V |
| VRy (Up/Down) | A0 |
| VRx (optional) | NOT USED |
| SW (click) | NOT USED |

---

## 🔘 Buttons

### SELECT Button
- One leg → 5V  
- Other leg → **Pin 6**  
- 10kΩ resistor → from Pin 6 to GND  

### BACK Button
- One leg → 5V  
- Other leg → **Pin 7**  
- 10kΩ resistor → from Pin 7 to GND  

---

## 🌞 Light Sensor (LDR)

Voltage divider:

- LDR → 5V  
- Other leg → **A1**  
- 10kΩ resistor → from A1 to GND  

---

## 🌡️ Temperature Sensor

### Use ONE option:

#### LM35
- VCC → 5V  
- GND → GND  
- OUT → **A2**

#### OR DHT11
- VCC → 5V  
- GND → GND  
- DATA → **Pin 10**

---

## 🚶 PIR Motion Sensor

| Pin | Arduino |
|-----|--------|
| VCC | 5V |
| GND | GND |
| OUT | Pin 13 |

---

## 💡 LEDs (Lighting + Fan)

Each LED:
- Anode → Arduino pin  
- Cathode → 220Ω → GND  

| Function | Pin |
|---------|----|
| Light 1 | 8 |
| Light 2 | 9 |
| Light 3 | 10* |
| Fan LED | A3 |

⚠️ If using DHT11 (Pin 10), move Light 3 → A4

---

## 🔊 Buzzer

| Pin | Arduino |
|-----|--------|
| + | A4 |
| – | GND |

⚠️ If A4 used above, move buzzer → A5

---

## ⚡ Relay Module

| Pin | Arduino |
|-----|--------|
| VCC | 5V |
| GND | GND |
| IN | A5 |

---

## ⚙️ Servo Motor

| Wire | Arduino |
|------|--------|
| Red | 5V |
| Brown/Black | GND |
| Signal | Pin 10* |

⚠️ Conflict rule:
- If using **DHT11**, move servo → Pin 9 (and shift LED accordingly)

---

## 🧠 FINAL PIN MAP (No Conflicts)

### Digital Pins
| Pin | Use |
|-----|----|
| 2–5 | LCD |
| 6 | Select button |
| 7 | Back button |
| 8 | LED 1 |
| 9 | LED 2 |
| 10 | Servo OR DHT11 |
| 11–12 | LCD |
| 13 | PIR |

### Analog Pins
| Pin | Use |
|-----|----|
| A0 | Joystick Y |
| A1 | LDR |
| A2 | Temp (LM35) |
| A3 | LED (fan) |
| A4 | Buzzer |
| A5 | Relay |

---

## ⚠️ Important Constraints

- You **cannot use everything at once on Pin 10**
  - Choose: Servo OR DHT11
- Analog pins are used as digital outputs where needed
- Total pin usage is at the safe limit of Arduino Uno

---

## ✅ Recommended Build Order

1. LCD (confirm working)
2. Joystick input
3. Buttons
4. One LED
5. LDR + temp sensor
6. PIR
7. Buzzer
8. Relay
9. Servo (last)

---

## 🎯 System Capability

- Menu navigation via joystick + buttons
- Real-time sensor monitoring
- Automated control logic (light, temp, motion)
- Output actuation (LEDs, buzzer, relay, servo)

---
