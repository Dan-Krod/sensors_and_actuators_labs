# Dual-Sensor Climate Benchmark (DHT11 vs DHT22) with CSV Telemetry

## Overview
This project implements a comparative benchmarking node on the Wemos D1 mini (ESP8266) to simultaneously read, validate, and log temperature and relative humidity data from DHT11 and DHT22 digital sensors. It displays real-time metrics and temperature differentials on an SSD1306 OLED screen while streaming CSV-formatted telemetry over UART for thermal inertia analysis.

## Engineering Highlights
* **Fault-Tolerant Sensor Polling:** Encapsulated sensor data into a `SensorReading` struct with independent `NaN` validation. If one sensor fails or loses connection on its single-wire bus, the system continues logging and displaying valid data from the remaining sensor without halting.
* **Non-Blocking State Machine & I2C Optimization:** Replaced blocking button delays with a 50ms `millis()` debounce algorithm. Added a state-change guard (`standbyDrawn`) to prevent redundant I2C frame buffer transmissions while in Standby mode.
* **Structured CSV Telemetry:** Outputs timestamped sensor readings over Serial (`115200 baud`) in CSV format, ready for direct ingestion into Python/Matplotlib scripts to plot thermal cooling curves and calculate time constants.
* **Respecting Hardware Sampling Limits:** Enforces a strict 2000ms non-blocking polling interval to comply with the DHT22's 0.5 Hz maximum sampling rate.

## Hardware Pin Mapping
| Component | Wemos D1 mini Pin | GPIO | Note |
| :--- | :--- | :--- | :--- |
| **System Toggle Button** | D6 | 12 | `INPUT_PULLUP`|
| **DHT11 Sensor** | D7 | 13 | Single-Wire Data |
| **DHT22 Sensor** | D4 | 2 | Single-Wire Data |
| **Green LED (Active)** | D5 | 14 | Output |
| **Blue LED (Standby)** | D0 | 16 | Output |
| **OLED SDA / SCL** | D2 / D1 | 4 / 5 | I2C Bus |

## Experimental Findings Summary
Benchmarked against a reference thermo-hygrometer (Hoco HX) under ambient conditions:
* **Accuracy:** DHT22 demonstrated superior temperature accuracy ($\Delta T = -0.8^\circ\text{C}$) compared to DHT11 ($\Delta T = -3.8^\circ\text{C}$). Both capacitive humidity elements showed notable offsets (DHT11: $-15.0\%$, DHT22: $+7.1\%$), confirming the need for software calibration in precision applications.
* **Thermal Inertia:** Cooling curve analysis after thermal excitation revealed that DHT11 responds faster to sudden temperature drops due to its simpler, lower-mass housing, whereas DHT22 exhibits higher thermal inertia!