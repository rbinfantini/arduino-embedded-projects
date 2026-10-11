# Interrupt-Driven Bare-Metal Reverse Parking Sensor

An academic automotive proximity alert prototype developed for the **Microchip ATmega328P (Arduino Uno)**. The firmware adheres to an event-driven bare-metal architecture, leaving the standard `loop()` function completely empty and delegating all tasks to hardware interrupts and internal timers.

---

## Software Architecture

Rather than relying on blocking delays (`delay()`) or CPU-intensive polling loops, this implementation executes entirely on hardware events:

```text
[KY-032 IR Sensor] ---> (State Change) ---> External Interrupt INT0 (Pin 2)
                                                         |
                                                         v
                                              Updates 'slot' (0 or 1)
                                                         |
[Timer1 Hardware (500 ms)] -> (Overflow) -> ISR(TIMER1_OVF_vect)
                                                         |
                                                         v
                                              Finite State Machine:
                                              - Slot 0: Clear (Silent / LED OFF)
                                              - Slot 1: Alert (Toggle LED & 1 kHz tone)
```

### 1. External Hardware Interrupt (Asynchronous Acquisition)

* The sensor digital output line is tied to **Digital Pin 2**, corresponding to external interrupt vector `INT0`.
* Configured using `attachInterrupt(..., CHANGE)` to trigger on every logic transition.
* The Interrupt Service Routine (ISR) `leSensor()` evaluates the pin level and updates the volatile variable `volatile int slot` (0 for clear path, 1 for detected obstacle).

### 2. Hardware Timer Interrupt (Timer1 500 ms Cadence)

* The 16-bit Timer1 is configured by direct register access (`TCCR1A`, `TCCR1B`, `TIMSK1`, `TCNT1`).
* **Timing Calculation:**
  * CPU Clock Frequency: $f_{\text{cpu}} = 16\text{ MHz}$.
  * Configured Prescaler: $256$.
  * Timer Increment Rate:
    $$f_{\text{timer}} = \frac{16 \times 10^6}{256} = 62{,}500\text{ Hz}$$
  * Target Period: $T = 0.5\text{ s}$ ($500\text{ ms}$).
  * Required Clock Pulses:
    $$\text{Pulses} = 62{,}500 \times 0.5 = 31{,}250\text{ cycles}$$
  * Timer1 overflows at $65{,}536$. Therefore, the reload offset is:
    $$\text{TCNT1} = 65{,}536 - 31{,}250 = 34{,}286$$

### 3. State Machine within Timer ISR

Inside `ISR(TIMER1_OVF_vect)`:
* **Slot 0 (Safe):** Deactivates buzzer (`noTone()`) and turns off the LED (`digitalWrite(LOW)`).
* **Slot 1 (Alert):** Inverts `pisca` each 500 ms cycle, producing a rhythmic 1 kHz acoustic pulse and synchronized blinking light.

---

## Bill of Materials (BOM)

| Item | Component | Qty | Purpose |
|---|---|:---:|---|
| 1 | Arduino Uno R3 (ATmega328P) | 1 | Microcontroller and hardware timer manager |
| 2 | KY-032 / IR-08H Obstacle Sensor | 1 | Optical infrared proximity detection |
| 3 | Passive Buzzer | 1 | 1 kHz acoustic frequency output |
| 4 | Diffused LED (5mm) | 1 | Visual warning indicator |
| 5 | $220\,\Omega$ Resistor | 1 | Current limiting resistor for LED |
| 6 | Breadboard & Jumper Wires | Set | Circuit connections |

---

## Pinout and Connections

| Component | Pin / Terminal | Arduino Uno Pin | Description |
|---|---|:---:|---|
| **KY-032 IR Sensor** | OUT / Signal | **Pin 2** | Connected to External Interrupt 0 (`INT0`) |
| **KY-032 IR Sensor** | VCC / GND | **5V / GND** | Range adjusted via onboard trimpots |
| **Passive Buzzer** | Positive (+) | **Pin 8** | Driven by `tone(..., 1000)` |
| **LED** | Anode (+) via $220\,\Omega$ | **Pin 9** | Pulsed synchronously with buzzer |
| **Common Ground** | Negative rails | **GND** | System ground reference |

---

## Bare-Metal Loop Execution

The main Arduino loop remains intentionally unpopulated:

```c
void loop() {
  // CPU remains idle.
  // All processing is dispatched via hardware interrupts and Timer1.
}
```

This design allows the microcontroller to enter AVR sleep modes (e.g., `set_sleep_mode(SLEEP_MODE_IDLE)`) without degrading input detection responsiveness.