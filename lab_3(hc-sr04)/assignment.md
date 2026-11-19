# Ultrasonic Distance Monitor with Temperature Compensation & Signal Filtering

## Overview
This project implements a high-precision, non-contact distance measurement system using the HC-SR04 ultrasonic sensor[cite: 3]. Built on the Wemos D1 mini (ESP8266) platform, the system addresses common ultrasonic limitations by integrating real-time environmental temperature compensation[cite: 3] and a moving average digital filter[cite: 3].

## Engineering Highlights
* **Temperature Compensation:** The speed of sound in air varies significantly with temperature, causing a ~1.8% error per 10°C change if uncompensated[cite: 3]. This system dynamically recalculates the speed of sound using the linear approximation: $V \approx 331.3 + (0.606 \cdot T)$[cite: 3].
* **Digital Signal Processing (Moving Average):** Ultrasonic sensors suffer from signal "bouncing" due to complex acoustic reflections[cite: 3]. A dynamic 10-sample moving average filter[cite: 3] is implemented to smooth the data. The filter features a smart-fill algorithm to prevent false drops (averaging with zeros) during the initial sampling phase.
* **Non-Blocking Architecture:** Uses `millis()`-based state machines for UI updates and sensor polling (60ms intervals), minimizing the impact of the inherently blocking `pulseIn()` function.
* **State Machine Button Control:** Implemented advanced button logic distinguishing between short clicks (system toggle) and long holds (>1000ms, temperature preset toggle)[cite: 3] without halting CPU execution.

## Hardware Pin Mapping
| Component | Wemos D1 mini Pin | GPIO | Note |
| :--- | :--- | :--- | :--- |
| **System/Mode Button** | D6 | 12 | `INPUT_PULLUP` |
| **HC-SR04 TRIG** | D7 | 13 | Output (10µs pulse)[cite: 3] |
| **HC-SR04 ECHO** | D4 | 2 | Input |
| **Green LED (Active)** | D5 | 14 | Output |
| **Blue LED (Standby)** | D0 | 16 | Output |
| **Orange LED (Target Lock)**| D3 | 0 | Output |
| **OLED SDA** | D2 | 4 | I2C Data |
| **OLED SCL** | D1 | 5 | I2C Clock |

## Performance Metrics
Based on experimental data[cite: 3], the uncompensated sensor shows standard absolute errors. The system limits readings outside the hardware specifications (<2cm and >400cm) to prevent buffer corruption[cite: 3]. Maximum relative error typically occurs at boundary near-field distances (5-10cm) due to internal emitter-receiver parallax[cite: 3].