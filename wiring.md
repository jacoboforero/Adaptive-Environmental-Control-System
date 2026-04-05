# 🔌 Wiring Guide (FINAL — Parallel LCD + DC Motor Version)
## Adaptive Environmental Control System (Arduino Uno R3)

---

## ⚡ Power Distribution

- Arduino **5V → Breadboard + rail**
- Arduino **GND → Breadboard – rail**

All components share common power and ground.

---

## 📟 LCD Display (16x2, 4-bit mode — KEEP EXISTING)

| LCD Pin | Arduino |
|--------|--------|
| VSS | GND |
| VDD | 5V |
| VO | Potentiometer middle pin |
| RS | Pin 12 |
| RW | GND ⚠️ REQUIRED |
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
| VRx | NOT USED |
| SW | NOT USED |

---

## 🔘 Buttons

### SELECT Button
- One leg → 5V  
- Other leg → **Pin 6**  
- 10kΩ resistor → Pin 6 → GND  

### BACK Button
- One leg → 5V  
- Other leg → **Pin 7**  
- 10kΩ resistor → Pin 7 → GND  

---

## 🌞 Light Sensor (LDR)

- LDR → 5V  
- Other leg → **A1**  
- 10kΩ resistor → A1 → GND  

---

## 🌡️ Temperature Sensor

### Option A: LM35
- VCC → 5V  
- GND → GND  
- OUT → **A2**

### Option B: DHT11
- VCC → 5V  
- GND → GND  
- DATA → **Pin 10**

⚠️ Choose ONE (affects Pin 10 usage)

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
- Cathode → 220Ω → GND  

| Function | Pin |
|---------|----|
| Light 1 | 8 |
| Light 2 | 9 |
| Light 3 | 10* |

⚠️ If using DHT11 (Pin 10), move Light 3 → A4

---

## 🌬️ DC Motor (Fan Control — REPLACES FAN LED)

### Required:
- DC Motor
- NPN Transistor (PN2222 / 2N2222 / S8050)
- Diode (1N400x)
- 1kΩ resistor

---

### Transistor Wiring (flat side facing you)

| Pin | Connection |
|-----|-----------|
| Emitter (left) | GND |
| Base (middle) | **A3** through 1kΩ resistor |
| Collector (right) | Motor (–) |

---

### Motor Wiring
- Motor (+) → 5V rail  
- Motor (–) → Transistor Collector  

---

### Diode (CRITICAL)
- Stripe side → 5V  
- Other side → Motor (–)  

---

## 🔊 Buzzer

| Pin | Arduino |
|-----|--------|
| + | A4 |
| – | GND |

⚠️ If A4 used for LED (DHT11 case), move buzzer → A5

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
- Pin 10 = Servo OR DHT11 OR LED3  
- Choose and reassign accordingly

---

## 🧠 FINAL PIN MAP

### Digital Pins
| Pin | Use |
|-----|----|
| 2–5 | LCD |
| 6 | Select button |
| 7 | Back button |
| 8 | LED 1 |
| 9 | LED 2 |
| 10 | Servo OR DHT11 OR LED 3 |
| 11–12 | LCD |
| 13 | PIR |

---

### Analog Pins
| Pin | Use |
|-----|----|
| A0 | Joystick Y |
| A1 | LDR |
| A2 | Temp (LM35) |
| A3 | Motor control (transistor) |
| A4 | Buzzer (or LED if reassigned) |
| A5 | Relay |

---

## ⚠️ Important Constraints

- Pin 10 is shared → must choose usage
- Do NOT connect motor directly to Arduino
- Always use transistor + diode for motor
- Ensure common ground across all components

---

## ✅ Recommended Build Order

1. LCD (confirm working)
2. Joystick
3. Buttons
4. LEDs
5. LDR + Temp sensor
6. PIR
7. Buzzer
8. Relay
9. DC Motor (last)

---

## 🎯 System Capability

- Menu navigation via joystick + buttons
- Real-time environmental sensing
- Automated control logic (light, temperature, motion)
- Physical actuation (LEDs, buzzer, relay, motor, servo)

---
