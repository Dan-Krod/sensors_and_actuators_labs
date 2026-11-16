# Non-Blocking IR Obstacle Monitor (FC-51) with OLED UI

## Overview
This project implements a hardware monitoring system using an FC-51 Infrared Obstacle Avoidance Sensor, an SSD1306 OLED display, and discrete LED indicators. 

## Engineering Highlights
* **Non-Blocking Architecture:** Replaced standard `delay()` functions with a `millis()`-based state machine, ensuring the microcontroller can process sensor data and UI updates asynchronously.
* **Software Debouncing:** Implemented a robust 50ms software debounce algorithm for the tactile button to filter out mechanical noise without halting the CPU.
* **I2C Zero-Redundancy Update:** The SSD1306 OLED display buffer is only pushed to the I2C bus when a state change is detected. This prevents unnecessary high-frequency I2C transmissions and significantly saves CPU cycles.

## Hardware Pin Mapping
| Component | Wemos D1 mini Pin | GPIO | Note |
| :--- | :--- | :--- | :--- |
| **System Toggle Button** | D6 | 12 | Configured as `INPUT_PULLUP` |
| **FC-51 IR Sensor** | D7 | 13 | Active `LOW` on obstacle detection |
| **Green LED (Clear)** | D5 | 14 | Output |
| **Blue LED (Standby/Alert)**| D0 | 16 | Output |
| **OLED SDA** | D2 | 4 | I2C Data |
| **OLED SCL** | D1 | 5 | I2C Clock |

## State Machine Logic
1. **Standby Mode (SYS: OFF):** The system is idle. The Blue LED is ON, indicating readiness. The sensor is ignored.
2. **Active Mode (SYS: ON - CLEAR):** The Green LED is ON. The system continuously polls the FC-51 sensor.
3. **Alert Mode (SYS: ON - OBSTACLE!):** Triggered when the FC-51 detects an object (`LOW`). The Green LED turns OFF, and the Blue LED turns ON as an alert indicator. The display updates immediately.