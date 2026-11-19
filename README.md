# Sensors & Actuators Labs (ESP8266 / Wemos D1 mini)

A collection of embedded systems laboratory works focused on hardware interfacing, real-time signal processing, and actuator control on the **Wemos D1 mini (ESP8266)** platform. 

All firmware modules have been refactored from scratch to follow production-oriented embedded practices—eliminating blocking `delay()` calls, preventing race conditions in ISRs, and optimizing I2C bus utilization.

## Core Engineering Practices Applied
* **Non-Blocking State Machines:** Replaced blocking delays with `millis()`-based scheduling and software button debouncing (short-press and long-press detection).
* **Hardware Interrupts & Safety:** Used `IRAM_ATTR`, `volatile` qualifiers, and critical sections (`noInterrupts()` / `interrupts()`) for high-frequency pulse counting.
* **Digital Signal Processing (DSP):** Implemented Moving Average (with smart-fill startup), Exponential Moving Average (EMA, $O(1)$ memory), and dual-stage deadband filtering.
* **Bus & Telemetry Optimization:** Zero-redundancy SSD1306 OLED updates over I2C and structured CSV telemetry over UART (`115200 baud`) for external analysis.

## Repository Structure

Each lab directory contains the refactored source code (`main.ino`), detailed technical documentation (`assignment.md`), and the electrical schematic (`schematic.png`).

| Lab | Topic & Hardware | Key Embedded Concepts | Links |
| :--- | :--- | :--- | :--- |
| **Lab 1** | **IR Obstacle Monitor**<br>*(FC-51, OLED, LEDs)* | Non-blocking state machine, 50ms software debounce, I2C zero-redundancy updates | [Docs](<./lab_1(fc-51)/assignment.md>) · [Code](<./lab_1(fc-51)/main.ino>) · [Schematic](<./lab_1(fc-51)/schematic.png>) |
| **Lab 2** | **Dual-Channel RPM Monitor**<br>*(FC-51, 4-pin Fan Tacho)* | Hardware interrupts (`IRAM_ATTR`), critical sections, ISR debouncing, error analysis | [Docs](<./lab_2(dc-motor)/assignment.md>) · [Code](<./lab_2(dc-motor)/main.ino>) · [Schematic](<./lab_2(dc-motor)/schematic.png>) |
| **Lab 3** | **Ultrasonic Ranging**<br>*(HC-SR04, OLED)* | Speed-of-sound temperature compensation, smart-fill Moving Average, long-press UI | [Docs](<./lab_3(hc-sr04)/assignment.md>) · [Code](<./lab_3(hc-sr04)/main.ino>) · [Schematic](<./lab_3(hc-sr04)/schematic.png>) |
| **Lab 4** | **Climate Sensor Benchmark**<br>*(DHT11 vs DHT22)* | Single-wire protocol, fault-tolerant `NaN` handling, CSV UART telemetry, thermal inertia | [Docs](<./lab_4(dht)/assignment.md>) · [Code](<./lab_4(dht)/main.ino>) · [Schematic](<./lab_4(dht)/schematic.png>) |
| **Lab 5** | **Non-Linear Servo Control**<br>*(B10K Pot + 5.1kΩ, MG996R)* | 10-bit ADC calibration, EMA low-pass filter, servo deadband, 100Hz PWM control loop | [Docs](<./lab_5(servo)/assignment.md>) · [Code](<./lab_5(servo)/main.ino>) · [Schematic](<./lab_5(servo)/schematic.png>) |

## Hardware & Toolchain
* **MCU:** Wemos D1 mini (ESP8266)
* **Display:** 0.96" SSD1306 OLED (I2C, `0x3C`)
* **Framework / Language:** C++ (Arduino Core for ESP8266)
* **Libraries:** `Wire`, `Adafruit_GFX`, `Adafruit_SSD1306`, `DHT`, `Servo`
* **ECAD:** KiCad (schematics)

## How to Build
1. Open the target `main.ino` in Arduino IDE or VS Code (PlatformIO / Arduino Extension).
2. Select **LOLIN(WEMOS) D1 R2 & mini** as the target board.
3. Install the required Adafruit/DHT libraries via Library Manager.
4. Connect hardware according to the corresponding `schematic.png` and flash at `115200` baud.