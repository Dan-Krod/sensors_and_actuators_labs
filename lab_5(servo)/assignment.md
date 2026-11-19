# Non-Linear Potentiometer-to-Servo Control System (ADC, EMA Filter & PWM)

## Overview
This project implements a closed-loop-ready actuator control node on the Wemos D1 mini (ESP8266) platform, mapping analog input from a B10K potentiometer to the angular position of an MG996R servo motor via PWM. Designed for Variant 3 ($r = 0.5R$), the system models a non-linear voltage divider transfer characteristic and visualizes real-time control metrics ($\alpha_i$, $K_i$, angle, and progress bar) on an SSD1306 OLED display.

## Engineering Highlights
* **Multi-Stage Signal Conditioning:** Combines ADC range calibration (`POT_MIN`/`POT_MAX`), a 30-count ADC deadband filter, and a first-order Exponential Moving Average (EMA, $\alpha = 0.05$) low-pass filter to eliminate analog noise using $O(1)$ memory.
* **Actuator Protection (Servo Deadband):** Enforces a minimum $2^\circ$ angular threshold before issuing new PWM updates to prevent mechanical jitter, gear wear, and excessive current draw (MG996R stall current up to 2.5A).
* **Non-Linear Transfer Function:** Implements the analytical transfer curve for a loaded voltage divider ($R/r = 2$):
  $$K_i = \frac{\alpha_i}{1 + 2(\alpha_i - \alpha_i^2)}$$
* **Non-Blocking Scheduling:** Separates the 100 Hz control loop (`10 ms`), 6.6 Hz OLED/UART telemetry (`150 ms`), and 50 ms button debouncing using `millis()`, eliminating blocking delays and redundant I2C transfers in Standby mode.

## Hardware Pin Mapping
| Component | Wemos D1 mini Pin | GPIO | Note |
| :--- | :--- | :--- | :--- |
| **Potentiometer (B10K + 5.1kΩ)** | A0 | ADC0 | 10-bit Analog Input (0–1023)[cite: 5] |
| **MG996R Servo Motor** | D4 | 2 | PWM Output (500–2500 µs, 50 Hz) |
| **System Toggle Button** | D6 | 12 | `INPUT_PULLUP` |
| **Green LED (Active)** | D5 | 14 | Output |
| **Blue LED (Standby)** | D0 | 16 | Output |
| **OLED SDA / SCL** | D2 / D1 | 4 / 5 | I2C Bus |

## Experimental Findings Summary
Measured across 11 setpoints ($\alpha_i \in [0.0, 1.0]$ with $0.1$ step) against the theoretical curve $K_i$:
* **Non-Linearity Verification:** The physical actuator closely tracked the parabolic transfer curve induced by the parallel $0.5R$ resistor[cite: 5].
* **Error Analysis:** Maximum positive lag ($\Delta_i = +0.093$) occurred at $\alpha_i = 0.7$, while a slight overshoot ($\Delta_i = -0.027$) was observed at $\alpha_i = 0.9$, attributed to resistive track non-linearity, gearbox backlash, and 10-bit ADC quantization.