# Atmospheric Sensor for UAV Deployment

## Goal
Provide a compact, lightweight, “ready to use” device that can accurately transmit data such as the GPS, temperature, humidity and altitude readings of a device.  Under the guidance of Dr. Moshary, this device is able to perform these functions appropriately.

## Team Members/Mentor
- Steven Silverio - Project Leader
- Ji-hye Hah - Power Design
- Muhammad Ahsan - Telemetry
- Kendall Molas - Software Design / Programming
- Mentor: Dr. Moshary

## Components Used
- Adafruit Feather M0 with RFM95 LoRa Radio - 900MHz
- Adafruit 2652 I2C or SPI Temperature Humidity Pressure Sensor
- MicroSD card breakout board+
- FeatherWing Proto
- 915MHZ Whip Antenna

## Software Used
- Fusion 360
- Arduino IDE

## Testing
In order to test the functionality of this device, two Adafruit Feather M0 with RFM95 LoRA Radio are used. One acts as a local workstation, while the other is mounted on top of some UAV ( eg. a drone ). The board that acts as a local workstation will be referred to as LoRa A, and the other board will be referred to as LoRa B.

### Receiver
Upload RX_LORAv1.1 to LoRa A. There is no additional options that need to be changed.

### Transmitter
Upload TX_LoRav1.2 to LoRa B. There are several debugging options available. Change **DEBUG** from 0 to 1 to see the outputs on the Serial Monitor.
