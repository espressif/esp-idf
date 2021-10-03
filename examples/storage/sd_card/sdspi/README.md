| Supported Targets | ESP32 | ESP32-S2 | ESP32-C3 |
| ----------------- | ----- | -------- | -------- |

# SD Card example (SDSPI)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use an SD card with an ESP device. Example does the following steps:

1. Use an "all-in-one" `esp_vfs_fat_sdspi_mount` function to:
    - initialize SDSPI peripheral,
    - probe and initialize the card connected to SPI bus (DMA channel 1, MOSI, MISO and CLK lines, chip-specific SPI host id),
    - mount FAT filesystem using FATFS library (and format card, if the filesystem cannot be mounted),
    - register FAT filesystem in VFS, enabling C standard library and POSIX functions to be used.
2. Print information about the card, such as name, type, capacity, and maximum supported frequency.
3. Create a file using `fopen` and write to it using `fprintf`.
4. Rename the file. Before renaming, check if destination file already exists using `stat` function, and remove it using `unlink` function.
5. Open renamed file for reading, read back the line, and print it to the terminal.

This example support SD (SDSC, SDHC, SDXC) cards.

## Hardware

Pins can be customized. See the initialization of ``spi_bus_config_t`` and ``sdspi_slot_config_t`` structures in the example code.

It is recommended to get familiar with [the document about pullup requirements](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/sd_pullup_requirements.html) to understand Pullup/down resistor support and compatibility of various ESP modules and development boards.

This example doesn't utilize card detect (CD) and write protect (WP) signals from SD card slot.

### Connections for ESP32

This example runs on ESP-WROVER-KIT boards without any extra modifications required, only the SD card needs to be inserted into the slot.

Other ESP32 development boards need to be connected to SD card as follows:

ESP32 pin     | SD card pin | SPI pin | Notes
--------------|-------------|---------|------------
GPIO2         | D0          | MISO    |
GPIO13 (MTCK) | D3          | CS      |
GPIO14 (MTMS) | CLK         | SCK     |
GPIO15 (MTDO) | CMD         | MOSI    | 10k pullup

Some boards require specific manipulation to enable UART Download mode (GPIO2 low) - eg ESP32-Azure IoT Kit needs KEY_IO0 pressed down for the time of firmware flashing operation (sets IO0 and IO2 low). See troubleshooting section for more details

### Connections for ESP32S2

ESP32S2 pin   | SD card pin | SPI pin | Notes
--------------|-------------|---------|------------
GPIO37        | D0          | MISO    |
GPIO34        | D3          | CS      |
GPIO36        | CLK         | SCK     |
GPIO35        | CMD         | MOSI    | 10k pullup

### Connections for ESP32-C3

ESP32-C3 pin  | SD card pin | SPI pin | Notes
--------------|-------------|---------|------------
GPIO5         | CLK         | SCK     |
GPIO4         | CMD         | MOSI    | 10k pullup
GPIO6        | D0          | MISO    |
GPIO1        | D3          | CS      |

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with serial port name.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


## Example output

Here is an example console output. In this case a 128MB SDSC card was connected, and `EXAMPLE_FORMAT_IF_MOUNT_FAILED` menuconfig option enabled. Card was unformatted, so the initial mount has failed. Card was then partitioned, formatted, and mounted again.

```
I (336) example: Initializing SD card
I (336) example: Using SPI peripheral
I (336) gpio: GPIO[13]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
W (596) vfs_fat_sdmmc: failed to mount card (13)
W (596) vfs_fat_sdmmc: partitioning card
W (596) vfs_fat_sdmmc: formatting card, allocation unit size=16384
W (7386) vfs_fat_sdmmc: mounting again
Name: XA0E5
Type: SDHC/SDXC
Speed: 20 MHz
Size: 61068MB
I (7386) example: Opening file /sdcard/hello.txt
I (7396) example: File written
I (7396) example: Renaming file /sdcard/hello.txt to /sdcard/foo.txt
I (7396) example: Reading file /sdcard/foo.txt
I (7396) example: Read from file: 'Hello XA0E5!'
I (7396) example: Card unmounted
```

## Troubleshooting

### Failure to mount filesystem

```
example: Failed to mount filesystem. If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.
```
The example will be able to mount only cards formatted using FAT32 filesystem. If the card is formatted as exFAT or some other filesystem, you have an option to format it in the example code. Enable the `EXAMPLE_FORMAT_IF_MOUNT_FAILED` menuconfig option, then build and flash the example.


### Unable to download the example BIN (or serial port not available)
```
After the first successful flashing of the example firmware, it is not possible to flash again. 
(Download mode not activated when running idf.py (==esptool.py) or the board's serial port disappears completely)
```

Some boards require specific handling to activate the Download mode after a system reset, due to GPIO2 pin now being used as both SDSPI (MISO) and a bootstrapping signal for enabling UART0 Boot (low).
(For instance, the ESP32-Azure IoT Kit requires KEY_IO0 button remain pressed during whole firmware flashing operation, as it sets both GPIO0 and GPIO2 signals low).
Check you board documentation/schematics for appropriate procedure.

An attempt to download a new firmware under this conditions may also result in the board's serial port disappearing from your PC device list - rebooting your computer should fix the issue. After your device is back, use

`esptool --port PORT --before no_reset --baud 115200 --chip esp32 erase_flash`

to clean your board's flash, then download your firmware properly.

