## SPI master half duplex EEPROM example

 This code demonstrates how to use the SPI master half duplex mode to read/write a AT93C46D
 EEPROM (8-bit mode). There is also an Kconfig option `EXAMPLE_USE_SPI1_PINS` allowing use the
 SPI1 (bus with code Flash connected on official modules).

### Connections

For different chip and host used, the connections may be different.

|      | ESP32 | ESP32 | ESP32S2 |
| ---- | ----- | ----- | ------- |
| Host | SPI1  | HSPI  | FSPI    |
| VCC  | 3.3V  | 3.3V  | 3.3V    |
| GND  | GND   | GND   | GND     |
| DO   | 7     | 18    | 37      |
| DI   | 8     | 23    | 35      |
| SK   | 6     | 19    | 36      |
| CS   | 13    | 13    | 34      |
| ORG  | GND   | GND   | GND     |

### Notes

If you meet timeout issues, please check your connections.