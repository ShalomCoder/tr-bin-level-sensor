# Bin Level Sensor

An Arduino Uno R3 bin level indicator using an HC-SR04 ultrasonic sensor and an optional browser dashboard. The dashboard displays live distance, fill percentage, and status over the Arduino USB serial connection.

## What you need

### Required

- Arduino Uno R3
- HC-SR04 ultrasonic distance sensor
- USB A-to-B cable
- Jumper wires
- A bin with a clear view from the sensor to the contents

### Optional

- Green LED and 220 ohm resistor
- Yellow LED and 220 ohm resistor
- Red LED and 220 ohm resistor
- 5 V active buzzer

The sensor works without the optional LEDs or buzzer. Leave those pins unconnected if you do not install them.

## Wiring

| Component | Pin | Arduino Uno R3 |
|---|---:|---:|
| HC-SR04 | VCC | 5V |
| HC-SR04 | GND | GND |
| HC-SR04 | TRIG | D9 |
| HC-SR04 | ECHO | D10 |
| Optional green LED anode (+) | - | D4 through 220 ohm resistor |
| Optional yellow LED anode (+) | - | D5 through 220 ohm resistor |
| Optional red LED anode (+) | - | D6 through 220 ohm resistor |
| Optional LED cathodes (-) | - | GND |
| Optional active buzzer (+) | - | D7 |
| Optional active buzzer (-) | - | GND |

Place the ultrasonic sensor at the top of the bin, pointing straight down. Keep the sensor away from the side walls and make sure its beam is not blocked by the lid.

## Upload the Arduino code

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Open `arduino/bin_level_sensor/bin_level_sensor.ino`.
3. Select **Tools > Board > Arduino Uno**.
4. Select the correct **Tools > Port**.
5. Click **Upload**.
6. Open **Tools > Serial Monitor** at **9600 baud** to verify readings.

The sketch sends lines in this format:

```text
BIN,18.42,46,OK
```

The fields are `BIN`, distance in centimeters, fill percentage, and status. Edit `EMPTY_DISTANCE_CM` and `FULL_DISTANCE_CM` near the top of the sketch to match the physical bin:

- `EMPTY_DISTANCE_CM`: sensor-to-contents distance when the bin is empty
- `FULL_DISTANCE_CM`: safe minimum distance when the bin should be treated as full

## Open the dashboard

The dashboard is a static page in `dashboard/index.html`; it has no npm packages or build step.

1. Serve the repository directory from a local web server. For example, with Python:

   ```bash
   python -m http.server 8000
   ```

2. Open <http://localhost:8000/dashboard/> in Chrome or Edge.
3. Click **Connect Arduino**, select the Arduino serial port, and allow access.
4. Leave the baud rate at **9600**.

Web Serial requires a secure context, so use `localhost` rather than opening the HTML file directly. If no Arduino is connected, click **Demo mode** to preview the interface.

## Status thresholds

| Fill percentage | Status | Optional indicators |
|---:|---|---|
| 0-25% | EMPTY | Green LED |
| 26-75% | OK | Yellow LED |
| 76-100% | FULL | Red LED and buzzer |

The buzzer sounds briefly once when the bin changes into the `FULL` state. It does not continuously beep.

## Project layout

```text
arduino/bin_level_sensor/bin_level_sensor.ino  Arduino Uno R3 firmware
dashboard/index.html                            Browser dashboard
```

## Safety and calibration

Use only the Arduino's 5 V logic and power the HC-SR04 from the Uno 5V pin. Do not connect a bare high-power load directly to an Arduino pin. For motors, lamps, or other loads, use an appropriate transistor or relay driver with a flyback diode.

Ultrasonic readings can be affected by soft, angled, or irregular contents. Adjust the two distance constants and test the sensor with the actual bin before relying on the percentage as an exact measurement.
