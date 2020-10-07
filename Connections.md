Based on a similar device to the Featherboard with LoRA capabilities, the Metro Express M0 was used with the following connections:

BME280:
| Device Pinout | Board Pinout |
| ------------- | ------------ |
| VCC | 3.3 V |
| GND | Shared GND |
| SCL | SCL |
| SDA | SDA |
| SDO | 3.3 V |

SD Breakout Board:
| Device Pinout | Board Pinout |
| ------------- | ------------ |
| 5 V | 3.3 V (this is allowed due to level shifting) |
| 3 V | Connects to VIN of Ultimate GPS |
| GND | Shared GND |
| CLK | SCK |
| DO | MOSO |
| DI | MOSI |
| CS | D10 ( can be changed, but needs to be accounted for in code ) |
| CD | Not connected |

Ultimate GPS Breakout Board:
| Device Pinout | Board Pinout |
| ------------- | ------------ |
| PPS | Not connected |
| VIN | 3 V Output from SD Breakout |
| GND | Shared GND |
| RX | Pin 1 (TX) |
| TX | Pin 0 (RX) |
| FIX | Not connected |
| VBAT | Not connected |
| EN | Not connected |
| 3.3 V | Not connected |

