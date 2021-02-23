| Supported Targets | ESP32 |
| ----------------- | ----- |

# FAT FS on External Flash example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example is similar to the [wear levelling](../wear_levelling/README.md) example, except that it uses an external SPI Flash chip. This can be useful if you need to add more storage to a module with only 4 MB flash size.

The flow of the example is as follows:

1. Initialize the SPI bus and configure the pins. In this example, VSPI peripheral is used. The pins chosen in this example correspond to IOMUX pins for the VSPI peripheral. If the pin assignment is changed, SPI driver will instead connect the peripheral to the pins using the GPIO Matrix.

2. Initialize the SPI flash chip. This involves creating a run-time object which describes the flash chip (`esp_flash_t`), probing the flash chip, and configuring it for the selected read mode. By default this example uses DIO mode, which only requires 4 pins (MOSI, MISO, SCLK, CS) but we strongly recommand to connect (or pull-up) the WP and HD pins. For modes such as QIO and QOUT, additional pins (WP/DQ2, HD/DQ3) must be connected.

3. Register the entire area of the Flash chip as a *partition* (`esp_partition_t`). This allows other components (FATFS, SPIFFS, NVS, etc) to use the storage provided by the external flash chip.

4. Do some read and write operations using C standard library functions: create a file, write to it, open it for reading, print the contents to the console.

## How to use example

### Hardware required

This example needs an SPI NOR Flash chip connected to the ESP32. The SPI Flash chip must have 3.3V logic levels. The example has been tested with Winbond W25Q32 SPI Flash chip.

Use the following pin assignments:

ESP32 pin     | SPI bus signal | SPI Flash pin 
--------------|----------------|----------------
GPIO23        | MOSI           | DI
GPIO19        | MISO           | DO
GPIO18        | SCLK           | CLK
GPIO5         | CS             | CMD
GPIO22        | WP             | WP
GPIO21        | HD             | HOLD
GND           |                | GND
VCC           |                | VCC

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with serial port name.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

Here is a typical example console output. 

```
I (328) example: Initializing external SPI Flash
I (338) example: Pin assignments:
I (338) example: MOSI: 23   MISO: 19   SCLK: 18   CS:  5
I (348) spi_flash: detected chip: generic
I (348) spi_flash: flash io: dio
I (348) example: Initialized external Flash, size=4096 KB, ID=0xef4016
I (358) example: Adding external Flash as a partition, label="storage", size=4096 KB
I (368) example: Mounting FAT filesystem
I (378) example: FAT FS: 4024 kB total, 4020 kB free
I (378) example: Opening file
I (958) example: File written
I (958) example: Reading file
I (958) example: Read from file: 'Written using ESP-IDF v4.0-dev-1301-g0a1160468'
```
