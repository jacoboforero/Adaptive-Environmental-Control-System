# 🔌 Wiring Guide (FINAL — Parallel LCD + DC Motor, NO RELAY)
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

⚠️ Choose ONE

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

⚠️ If using DHT11 → move Light 3 → A4

---

## 🌬️ DC Motor (Fan Control)

### Components:
- DC Motor
- NPN Transistor (PN2222 / 2N2222 / S8050)
- Diode (1N400x)
- 1kΩ resistor

---

### Wiring:

#### Transistor (flat side facing you)

| Pin | Connection |
|-----|-----------|
| Emitter | GND |
| Base | **A3** through 1kΩ resistor |
| Collector | Motor (–) |

---

### Motor:
- Motor (+) → 5V  
- Motor (–) → Collector  

---

### Diode (CRITICAL):
- Stripe → 5V  
- Other side → Motor (–)

---

## 🔊 Buzzer

| Pin | Arduino |
|-----|--------|
| + | A4 |
| – | GND |

⚠️ If A4 used for LED → move buzzer → A5

---

## ⚙️ Servo Motor

| Wire | Arduino |
|------|--------|
| Red | 5V |
| Brown/Black | GND |
| Signal | Pin 10* |

⚠️ Shared pin with DHT11 / LED3

---

## 🧠 FINAL PIN MAP

### Digital Pins
| Pin | Use |
|-----|----|
| 2–5 | LCD |
| 6 | Select |
| 7 | Back |
| 8 | LED 1 |
| 9 | LED 2 |
| 10 | Servo OR DHT11 OR LED |
| 11–12 | LCD |
| 13 | PIR |

---

### Analog Pins
| Pin | Use |
|-----|----|
| A0 | Joystick |
| A1 | LDR |
| A2 | Temp |
| A3 | Motor control |
| A4 | Buzzer |
| A5 | (Available / optional expansion) |

---

## ⚠️ Critical Rules

- NEVER connect motor directly to Arduino
- ALWAYS use transistor + diode for motor
- Ensure common ground across all components
- Avoid pin conflicts on Pin 10

---

## ✅ Build Order

1. LCD
2. Joystick
3. Buttons
4. LEDs
5. Sensors (LDR + Temp)
6. PIR
7. Buzzer
8. DC motor (last)

---

## 🎯 System Capability

- Menu-driven control system (LCD + joystick + buttons)
- Real-time environmental sensing (light, temperature, motion)
- Automated decision-making logic
- Physical actuation (LEDs, buzzer, DC motor fan, optional servo)

---
