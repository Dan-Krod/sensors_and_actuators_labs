# Dual-Channel RPM Monitor (Hardware Interrupts & ISR Debouncing)

## Overview
This project implements a dual-channel tachometer system on the ESP8266 (Wemos D1 mini) platform to measure and compare the rotational speed of a DC motor using two distinct methods simultaneously:
1. **Optical Method:** Non-contact measurement using an FC-51 Infrared sensor.
2. **Electrical Method (Reference):** Direct signal reading from a 4-pin PC Fan's internal Hall-effect sensor (Tacho).

## Engineering Highlights
* **Hardware Interrupts (ISR):** Utilizes `attachInterrupt()` to capture high-frequency pulse trains directly via hardware, ensuring zero pulse loss compared to standard polling methods.
* **ISR Memory Optimization:** Interrupt Service Routines are forced into RAM using the `IRAM_ATTR` directive to prevent flash read conflicts and kernel panics.
* **Race Condition Prevention:** Implemented critical sections (`noInterrupts()` / `interrupts()`) in the main loop to safely transfer volatile counter data before performing floating-point RPM calculations.
* **Asymmetric Signal Processing:** 
  * The Tacho signal (digital, clean) is processed directly. 
  * The FC-51 optical signal (susceptible to environmental noise and reflection bouncing) is processed through a highly optimized 10ms hardware-timer-based debounce filter directly inside the ISR.

## Hardware Pin Mapping
| Component | Wemos D1 mini Pin | GPIO | Note |
| :--- | :--- | :--- | :--- |
| **System Toggle Button** | D6 | 12 | Configured as `INPUT_PULLUP` |
| **FC-51 IR Sensor (Optical)**| D7 | 13 | Trigger: `FALLING` edge |
| **Tacho Signal (Hall)** | D4 | 2 | Configured as `INPUT_PULLUP`, Trigger: `FALLING` |
| **Green LED (Active)** | D5 | 14 | Output |
| **Blue LED (Standby)** | D0 | 16 | Output |
| **Orange LED (Pulse Alert)** | D3 | 0 | Output |
| **OLED SDA** | D2 | 4 | I2C Data |
| **OLED SCL** | D1 | 5 | I2C Clock |

## Mathematical Model
The system continuously calculates Absolute and Relative errors to evaluate the accuracy of the optical sensor against the internal Hall-effect reference.
* `RPM = (Pulses * 60000) / (TimeDelta * PulsesPerRevolution)`
* FC-51 configuration: 1 pulse per revolution (`k=1`).
* Tacho configuration: 2 pulses per revolution (`k=2`).