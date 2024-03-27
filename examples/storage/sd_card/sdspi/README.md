| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# SD Card example (SDSPI)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

__WARNING:__ This example can potentially delete all data from your SD card (when formatting is enabled). Back up your data first before proceeding.

This example demonstrates how to use an SD card with an ESP device over an SPI interface. Example does the following steps:

1. Use an "all-in-one" `esp_vfs_fat_sdspi_mount` function to:
    - initialize SDSPI peripheral,
    - probe and initialize the card connected to SPI bus (DMA channel 1, MOSI, MISO and CLK lines, chip-specific SPI host id),
    - mount FAT filesystem using FATFS library (and format card, if the filesystem cannot be mounted),
    - register FAT filesystem in VFS, enabling C standard library and POSIX functions to be used.
1. Print information about the card, such as name, type, capacity, and maximum supported frequency.
1. Create a file using `fopen` and write to it using `fprintf`.
1. Rename the file. Before renaming, check if destination file already exists using `stat` function, and remove it using `unlink` function.
1. Open renamed file for reading, read back the line, and print it to the terminal.
1. __OPTIONAL:__ Format the SD card, check if the file doesn't exist anymore.

This example support SD (SDSC, SDHC, SDXC) cards.

## Hardware

This example requires a development board with an SD card socket and and SD card.

Although it is possible to connect an SD card breakout adapter, keep in mind that connections using breakout cables are often unreliable and have poor signal integrity. You may need to use lower clock frequency when working with SD card breakout adapters.

It is recommended to get familiar with [the document about pullup requirements](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/sd_pullup_requirements.html) to understand Pullup/down resistor support and compatibility of various ESP modules and development boards.

### Pin assignments

The GPIO pin numbers used to connect an SD card can be customized. This can be done in two ways:

1. Using menuconfig: Run `idf.py menuconfig` in the project directory and open "SD SPI Example Configuration" menu.
2. In the source code: See the initialization of ``spi_bus_config_t`` and ``sdspi_device_config_t`` structures in the example code.

This example doesn't utilize card detect (CD) and write protect (WP) signals from SD card slot.

The table below shows the default pin assignments.

SD card pin | SPI pin | ESP32 pin     | ESP32-S2, ESP32-S3 | ESP32-H2 | ESP32-C3 and other chips  |  Notes
------------|---------|---------------|--------------------|----------|---------------------------|-------------
 D0         | MISO    | GPIO2         | GPIO37             | GPIO0    | GPIO6                     |
 D3         | CS      | GPIO13 (MTCK) | GPIO34             | GPIO1    | GPIO1                     |
 CLK        | SCK     | GPIO14 (MTMS) | GPIO36             | GPIO4    | GPIO5                     |
 CMD        | MOSI    | GPIO15 (MTDO) | GPIO35             | GPIO5    | GPIO4                     |  10k pullup


#### ESP32 related notes

With the default pin assignments, this example runs on ESP-WROVER-KIT boards without any extra modifications required. Only the SD card needs to be inserted into the slot.

For other development boards, adjust the pin assignments as explained above.

Some boards require specific manipulation to enable UART Download mode (GPIO2 low) - eg ESP32-Azure IoT Kit needs KEY_IO0 pressed down for the time of firmware flashing operation (sets IO0 and IO2 low). See troubleshooting section for more details

#### ESP32-S2 and ESP32-S3 related notes

With the default pin assignments, this example is compatible ESP32-S2-USB-OTG and ESP32-S3-USB-OTG development boards.

For other development boards, adjust the pin assignments as explained above.

#### Notes for ESP32-C3 and other chips

Espressif doesn't offer development boards with an SD card slot for these chips. Please check the pin assignments and adjust them for your board if necessary. The process to change pin assignments is described above.

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with serial port name.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


## Example output

Here is an example console output. In this case a 64GB SDHC card was connected, and `EXAMPLE_FORMAT_IF_MOUNT_FAILED` menuconfig option enabled. Card was unformatted, so the initial mount has failed. Card was then partitioned, formatted, and mounted again.

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

> The following error message is printed: `example: Failed to mount filesystem. If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.`

The example will be able to mount only cards formatted using FAT32 filesystem. If the card is formatted as exFAT or some other filesystem, you have an option to format it in the example code. Enable the `CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED` menuconfig option, then build and flash the example.

> Once you've enabled the `CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED` option, if you continue to encounter the following error:

```
E (600) sdmmc_cmd: sdmmc_read_sectors_dma: sdmmc_send_cmd returned 0x108
E (600) diskio_sdmmc: sdmmc_read_blocks failed (264)
W (610) vfs_fat_sdmmc: failed to mount card (1)
E (610) vfs_fat_sdmmc: mount_to_vfs failed (0xffffffff).
I (620) gpio: GPIO[13]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
E (630) example: Failed to mount filesystem. If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.
```

Please ensure that your SD card is operational and not experiencing any malfunctions.


### Unable to flash the example, or serial port not available (ESP32 only)

> After the first successful flashing of the example firmware, it is not possible to flash again. Download mode not activated when running `idf.py flash` or the board's serial port disappears completely.

Some ESP32 boards require specific handling to activate the download mode after a system reset, due to GPIO2 pin now being used as both SDSPI (MISO) and an active-low bootstrapping signal for entering download mode. For instance, the ESP32-Azure IoT Kit requires KEY_IO0 button to remain pressed during whole firmware flashing operation, as it sets both GPIO0 and GPIO2 signals low.

Check you board documentation/schematics for appropriate procedure.

An attempt to download a new firmware under this conditions may also result in the board's serial port disappearing from your PC device list - rebooting your computer should fix the issue. After your device is back, use

`esptool --port PORT --before no_reset --baud 115200 --chip esp32 erase_flash`

to erase your board's flash, then flash the firmware again.

> If you insert an SD card into the slot and encounter issues when attempting to flash a supported target using the `idf.py flash` command, please consider removing the SD card and attempting to flash the target again. If the flashing process succeeds after removing the SD card, it suggests potential issues with power supply.

Ensure that the board and SD card adapter you are using are powered using the appropriate power source.


### Getting the following errors

> `vfs_fat_sdmmc: slot init failed (0x103)`

> `vfs_fat_sdmmc: sdmmc_card_init failed (0x102)`

> `sdmmc_init_ocr: send_op_cond (1) returned 0x107`

Attempt to reboot the board. This error may occur if you reset the ESP board or host controller without power-cycling it. In such cases, the card may remain in its previous state, causing it to potentially not respond to commands sent by the host controller.

Additionally, if the example works with certain SD cards but encounters issues with others, please confirm the read/write speed of the SD card. If the card is not compatible with the host frequency, consider lowering the host frequency and then attempting the operation again.

### Debug SD connections and pullup strength

> If the initialization of the SD card fails, initially follow the above options. If the issue persists, confirm the connection of pullups to the SD pins. To do this, enable the` Debug sd pin connections and pullup strength` option from menuconfig and rerun the code. This will provide the following result:

```
**** PIN recovery time ****

PIN 14 CLK   10049 cycles
PIN 15 MOSI  10034 cycles
PIN  2 MISO  10034 cycles
PIN 13 CS    10034 cycles

**** PIN recovery time with weak pullup ****

PIN 14 CLK   100 cycles
PIN 15 MOSI  100 cycles
PIN  2 MISO  100 cycles
PIN 13 CS    100 cycles

**** PIN voltage levels ****

PIN 14 CLK   0.6V
PIN 15 MOSI  0.4V
PIN  2 MISO  0.7V
PIN 13 CS    0.9V

**** PIN voltage levels with weak pullup ****

PIN 14 CLK   0.9V
PIN 15 MOSI  1.0V
PIN  2 MISO  1.0V
PIN 13 CS    1.2V

**** PIN cross-talk ****

             CLK    MOSI   MISO   CS
PIN 14 CLK    --    0.2V   0.2V   0.2V
PIN 15 MOSI  0.1V    --    0.1V   0.1V
PIN  2 MISO  0.1V   0.1V    --    0.1V
PIN 13 CS    0.1V   0.1V   0.1V    --

**** PIN cross-talk with weak pullup ****

             CLK    MOSI   MISO   CS
PIN 14 CLK    --    1.0V   1.1V   1.2V
PIN 15 MOSI  0.9V    --    1.0V   1.2V
PIN  2 MISO  0.9V   1.0V    --    1.2V
PIN 13 CS    0.9V   1.1V   1.0V    --

```
In the absence of connected pullups and having the weak pullups enabled, you can assess the pullup connections by comparing PIN recovery time measured in CPU cycles. To check pullup connections, configure the pin as open drain, set it to low state, and count the cpu cycles consumed before returning to high state. If a pullup is connected, the pin will get back to high state after reasonably small cycle count, typically around 50-300 cycles, depending on pullup strength. If no pullup is connected, the PIN stays low and the measurement times out after 10000 cycles.

It will also provide the voltage levels at the corresponding SD pins. By default, this information is provided for ESP32 chip only, and for other chipsets, verify the availability of ADC pins for the respective GPIO using [this](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html#gpio-summary) and configure ADC mapped pins using menuconfig. Then test the voltage levels accordingly.

You can monitor the voltage levels of individual pins using `PIN voltage levels` and `PIN voltage levels with weak pullup`. However, if one pin being pulled low and experiencing interference with another pin, you can detect it through `PIN cross-talk` and `PIN cross-talk with weak pullup`. In the absence of pullups, voltage levels at each pin should range from 0 to 0.3V. With 10k pullups connected, the voltage will be between 3.1V to 3.3V, contingent on the connection between ADC pins and SD pins, and with weak pullups connected, it can fluctuate between 0.8V to 1.2V, depending on pullup strength.
