# Landslide Monitoring and Early Warning System

An autonomous environmental sensing station and civil defense prototype based on the **ESP32**. Operating as an Edge Computing unit, the system detects critical geological risk factors in real time—soil/air moisture saturation and physical ground displacement—issuing local audiovisual alerts and hosting an onboard web dashboard over its own wireless Access Point.

---

## Technical Features

* **Non-Blocking Architecture:** Cooperative periodic sampling governed by `millis()`, avoiding blocking routines (`delay()`) inside the execution cycle.
* **Embedded Web Server:** The ESP32 broadcasts a local Wi-Fi Access Point (`Proj_terra`) and hosts an HTTP server on port 80.
* **Real-Time Asynchronous Dashboard:** Web UI refreshed via AJAX polling (`/dados` JSON endpoint every 2 seconds) without requiring full page reloads.
* **Risk State Machine:**
  * `State 0 - SAFE (Green LED):` Relative humidity $< 70\%$ and displacement $\le 5.0\text{ cm}$.
  * `State 1 - WARNING (Yellow LED):` Relative humidity $\ge 70\%$ (indicating potential soil saturation).
  * `State 2 - IMMINENT DANGER (Red LED + Siren):` Displacement $> 5.0\text{ cm}$ relative to baseline calibration.

---

## Bill of Materials (BOM)

| Item | Component | Qty | Notes |
|---|---|:---:|---|
| 1 | ESP32 NodeMCU / DevKit V1 | 1 | Main MCU with integrated 2.4 GHz Wi-Fi |
| 2 | DHT11 Temperature & Humidity Sensor | 1 | Monitors air and surface humidity |
| 3 | HC-SR04 Ultrasonic Sensor | 1 | Detects physical ground displacement |
| 4 | Red LED (5mm) | 1 | Imminent danger indicator |
| 5 | Yellow LED (5mm) | 1 | Warning state indicator |
| 6 | Green LED (5mm) | 1 | Safe state indicator |
| 7 | $220\,\Omega$ or $330\,\Omega$ Resistors | 3 | Current limiters for LEDs |
| 8 | 5V Active Buzzers | 2 | Dual-tone oscillating acoustic siren |
| 9 | Solderless Breadboard (830 tie-points) | 1 | Prototyping base |
| 10 | Jumper Wires (M/M, M/F) | Set | Circuit routing |

---

## Pinout and Hardware Connections

| Component | Component Pin | ESP32 GPIO | Direction | Operational Role |
|---|---|---|:---:|---|
| **DHT11** | DATA / OUT | **GPIO 4** | Input | Digital humidity and temperature read |
| **HC-SR04** | TRIG | **GPIO 5** | Output | Ultrasonic trigger pulse (10 $\mu\text{s}$) |
| **HC-SR04** | ECHO | **GPIO 18** | Input | Time-of-flight echo duration |
| **Green LED** | Anode (+) via Resistor | **GPIO 19** | Output | State 0 (Safe) |
| **Yellow LED** | Anode (+) via Resistor | **GPIO 21** | Output | State 1 (Warning) |
| **Red LED** | Anode (+) via Resistor | **GPIO 2** | Output | State 2 (Imminent Danger) |
| **Buzzer 1** | Positive (+) | **GPIO 23** | Output | Non-blocking siren toggle (500 ms) |
| **Buzzer 2** | Positive (+) | **GPIO 15** | Output | Secondary siren channel |
| **Power Supply** | VCC / VIN / GND | **VIN (5V) / 3V3 / GND** | Power | Common ground bus |

> Note: All ground lines must share a common reference (GND). If the HC-SR04 is powered by 5V, ensure the ECHO signal respects the ESP32 3.3V logic level tolerance (via a voltage divider) or use a 3.3V-compatible ultrasonic module.

---

## Displacement Calculation Model

Distance is calculated using the acoustic Time-of-Flight (ToF) and the nominal speed of sound in atmospheric air ($v \approx 343\text{ m/s}$):

$$\text{Distance (cm)} = \frac{\Delta t \times 0.0343}{2}$$

During boot calibration, the firmware takes 10 consecutive measurements to determine the stable baseline distance. Any structural deviation exceeding $5.0\text{ cm}$ immediately activates the evacuation alarm.

---

## Firmware Deployment

1. Install the **DHT sensor library by Adafruit** in the Arduino IDE.
2. Select **DOIT ESP32 DEVKIT V1** as the target board.
3. Compile and upload `landslide-alert-system.ino`.
4. Connect your smartphone or computer to the Wi-Fi network:
   * **SSID:** `Proj_terra`
   * **Password:** *(Open network by default)*
5. Open any web browser and navigate to:
   ```text
   http://192.168.4.1
   ```
