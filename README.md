# Embedded Systems and IoT Project Collection

A unified repository containing academic prototypes and community extension projects developed with **Arduino Uno (ATmega328P)** and **ESP32**. Projects range from bare-metal register manipulation to edge computing and distributed systems connected over local wireless networks.

---

## Repository Structure

```text
arduino-projects/
├── landslide-alert-system/
│   ├── landslide-alert-system.ino
│   └── README.md
├── pedestrian-traffic-light/
│   ├── pedestrian-traffic-light.ino
│   └── README.md
├── 1d-boss-fight-arcade/
│   ├── 1d-boss-fight-arcade.ino
│   └── README.md
├── baremetal-parking-sensor/
│   ├── baremetal-parking-sensor.ino
│   └── README.md
└── README.md
```

---

## Project Overview

| #      | Project                                                | Platform    | Key Peripherals                                  | Architectural Highlights                                                    |
| ------ | ------------------------------------------------------ | ----------- | ------------------------------------------------ | --------------------------------------------------------------------------- |
| **01** | [Landslide Warning System](./01-alerta-deslizamento/)  | ESP32       | DHT11, HC-SR04, LEDs, Dual Buzzers, Wi-Fi SoftAP | Cooperative multitasking (`millis()`), asynchronous AJAX JSON dashboard     |
| **02** | [Pedestrian Traffic Light](./02-semaforo-pedestre/)    | Arduino Uno | Traffic light LEDs, Push button with cooldown    | Timed sequential control, debounce protection, pedestrian safety window     |
| **03** | [1D Boss Fight Arcade](./03-boss-fight-1d/)            | ESP32       | WS2812B Strip (100 LEDs), Buzzer, Push buttons   | Finite State Machine, PWM sound synthesis via `ledc`, color matching engine |
| **04** | [Bare-Metal Reverse Sensor](./04-sensor-re-baremetal/) | Arduino Uno | KY-032 IR Sensor, Buzzer, LED                    | Empty `loop()`, External Hardware Interrupt (INT0), 500 ms Timer1 Overflow  |

---

## Technologies and Tools

* **Languages:** C / C++ (Arduino Framework and AVR-GCC).
* **Target Platforms:**
  * Microchip ATmega328P 8-bit AVR RISC microcontroller (Arduino Uno).
  * Espressif ESP32 dual-core Xtensa LX6 microcontroller with integrated 802.11 b/g/n Wi-Fi.
* **Core Libraries:**
  * `DHT.h` (Adafruit DHT Sensor Library)
  * `Adafruit_NeoPixel.h`
  * `WiFi.h` & `WebServer.h`
  * `<avr/io.h>` & `<avr/interrupt.h>` (Direct hardware register manipulation)

---

## Getting Started

1. **Clone the repository:**
   
   ```bash
   git clone https://github.com/rbinfantini/arduino-embedded-projects.git
   cd arduino-embedded-projects
   ```

2. **Open in your development environment:**
   
   * **Arduino IDE 2.x:** Ensure board definitions for *Arduino AVR Boards* and *esp32 by Espressif Systems* are installed.
   * **VS Code + PlatformIO:** Target the respective board environment (`uno` or `esp32dev`) for each project folder.

3. **Inspect individual project directories** for wiring schematics, pinout assignments, and Bill of Materials (BOM).

---
