# Boss Fight 1D - One-Dimensional Interactive Arcade

A one-dimensional arcade game developed for the **ESP32** microcontroller, utilizing an addressable **WS2812B NeoPixel LED strip** as the primary visual display and a passive buzzer for real-time sound effects generated via hardware PWM timers (`ledc`).

---

## Gameplay Mechanics

* **1D LED Canvas:** A strip of 100 individually addressable RGB LEDs represents the linear battlefield.
* **Boss and Minion Waves:**
  * The **Boss** occupies the far end of the strip, rendered as purple LEDs whose width reflects current boss health points (HP).
  * Progressive waves of **minions** advance toward the player at position 0 in Green, Blue, and Yellow colors.
* **Player Actions:** The player presses physical buttons mapped to Green, Blue, or Yellow projectiles.
* **Rules of Engagement:**
  * **Color Match:** Striking an enemy of the same color eliminates the minion and plays an impact sound (`puf`).
  * **Color Mismatch / Miss:** Penalty triggered (`mehhh`): all remaining enemies instantly advance 2 positions forward.
  * **Boss Damage:** Clearing an entire wave inflicts 1 point of damage to the Boss, increments the wave level, and accelerates minion movement speed.

---

## Finite State Machine

The core loop is managed by a three-phase Finite State Machine:

1. **`hiatus`:** Idle attract mode displaying an animated rainbow wave (`rainbow()`). Pressing any button initializes the game session.
2. **`spawning`:** Staggered generation of minions and Boss entry music (`bossIntro()`).
3. **`playing`:** Active combat mode updating bullet trajectories, enemy shifts, hit detection, and win/loss verification.

---

## Bill of Materials (BOM)

| Item | Component | Qty | Role |
|---|---|:---:|---|
| 1 | ESP32 DevKit V1 | 1 | Game physics, state control, and rendering |
| 2 | WS2812B Addressable LED Strip | 100 LEDs | One-dimensional game display |
| 3 | Passive Buzzer | 1 | Tone and audio frequency generation |
| 4 | Tactile Push Buttons | 3 | Input triggers (Blue, Green, Yellow) |
| 5 | External 5V Power Supply ($\ge 2\text{A}$) | 1 | Dedicated power for WS2812B strip |
| 6 | Breadboard and Jumper Wires | Set | Component interconnection |

---

## Pinout and Hardware Configuration

| Component | Module Pin | ESP32 GPIO | Mode / Description |
|---|---|:---:|---|
| **WS2812B Strip** | DIN (Data In) | **GPIO 5** | High-speed single-wire NRZ output (800 kHz) |
| **Blue Button** | Signal Leg | **GPIO 14** | `INPUT_PULLUP` (Active `LOW`) |
| **Green Button** | Signal Leg | **GPIO 27** | `INPUT_PULLUP` (Active `LOW`) |
| **Yellow Button** | Signal Leg | **GPIO 26** | `INPUT_PULLUP` (Active `LOW`) |
| **Passive Buzzer** | Positive (+) | **GPIO 25** | Hardware PWM channel via `ledc` |
| **Power Ground** | GND | **GND** | Shared common ground between PSU and ESP32 |

> Power Advisory: Driving 100 LEDs at maximum white brightness can draw significant current. To prevent ESP32 brownout resets, use an independent 5V power supply for the LED strip and maintain a shared ground reference.

---

## Compilation and Dependencies

1. Install the **Adafruit NeoPixel** library from the Arduino Library Manager.
2. Select your ESP32 board target (e.g., **ESP32 Dev Module**).
3. Upload `1d-boss-fight-arcade.ino` to the device.
4. Press any button during the rainbow animation to begin playing.
