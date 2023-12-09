| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

## SPI master half duplex EEPROM example

 This code demonstrates how to use the SPI master half duplex mode to read/write a AT93C46D
 EEPROM (8-bit mode). There is also an Kconfig option `EXAMPLE_USE_SPI1_PINS` allowing use the
 SPI1 (bus with code Flash connected on official modules).

### Connections

For different chip and host used, the connections may be different. Here show a example diagram of hardware connection, you can freely change the GPIO defined in start of `main/spi_eeprom_main.c` and change the hardware relatively.

|      | ESP32 |
| ---- | ----- |
| Host | SPI1  |
| VCC  | 3.3V  |
| GND  | GND   |
| DO   | 7     |
| DI   | 8     |
| SK   | 6     |
| CS   | 13    |
| ORG  | GND   |

### Notes

If you meet timeout issues, please check your connections.