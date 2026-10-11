# Pedestrian-Actuated Traffic Light System

An educational urban traffic control system implemented on the **Arduino Uno (ATmega328P)**. The system balances vehicle flow with pedestrian safety, incorporating confirmation windows, timed phase transitions, and re-trigger protection.

---

## Operating Logic and Phase Transitions

Under default conditions, the vehicular green signal remains energized. When a pedestrian presses the crosswalk push button, the controller executes the following state sequence:

1. **Default State:** Vehicle Green LED is active.
2. **Confirmation Delay:** When a low signal (`LOW`) is read on the push button, the MCU waits $2\text{ s}$ to confirm intent and reject transient noise.
3. **Transition Phase:** The Green LED turns off, and the Yellow LED lights up for $4\text{ s}$ to alert approaching drivers.
4. **Pedestrian Crossing Phase:** The Yellow LED turns off, and the Red LED activates for $8\text{ s}$, safely stopping vehicular traffic.
5. **Cooldown Phase:** The Red LED turns off, the Green LED is re-energized, and a mandatory $5\text{ s}$ cooldown timer begins before another crossing request can be registered.

---

## Bill of Materials (BOM)

| Item | Component | Qty | Function |
|---|---|:---:|---|
| 1 | Arduino Uno R3 (ATmega328P) | 1 | Central controller |
| 2 | Diffused Green LED (5mm) | 1 | Traffic go signal |
| 3 | Diffused Yellow LED (5mm) | 1 | Traffic caution signal |
| 4 | Diffused Red LED (5mm) | 1 | Traffic stop signal |
| 5 | Tactile Push Button (4-pin) | 1 | Pedestrian crossing input |
| 6 | $220\,\Omega$ or $330\,\Omega$ Resistors | 3 | Current limiting for LEDs |
| 7 | $10\,\text{k}\Omega$ Resistor | 1 | External pull-up / pull-down resistor |
| 8 | Mini Breadboard (400 tie-points) | 1 | Circuit assembly |
| 9 | Male-to-Male Jumper Wires | Set | Breadboard interconnects |

---

## Pinout and Connections

| Component | Physical Connection | Arduino Uno Pin | Notes |
|---|---|:---:|---|
| **Green LED** | Anode (+) via $220\,\Omega$ resistor | **Pin 8** | Digital output |
| **Yellow LED** | Anode (+) via $220\,\Omega$ resistor | **Pin 9** | Digital output |
| **Red LED** | Anode (+) via $220\,\Omega$ resistor | **Pin 11** | Digital output |
| **Pedestrian Button** | Signal leg | **Pin 10** | Digital input (Active `LOW`) |
| **LED Cathodes** | Negative terminals (-) | **GND** | Shared common ground |

---

## How to Upload

1. Connect the Arduino Uno to your workstation using a USB type-A/B cable.
2. Open `stoplight-code.ino` in the Arduino IDE.
3. Under `Tools > Board`, select **Arduino Uno**.
4. Select the corresponding serial port under `Tools > Port`.
5. Click **Upload** and verify the timing sequence on the breadboard.