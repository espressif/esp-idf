| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

# eMMC chip example (with SDMMC Host)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use an eMMC chip with an ESP device. Example does the following steps:

1. Use an "all-in-one" `esp_vfs_fat_sdmmc_mount` function to:
    - initialize SDMMC peripheral,
    - probe and initialize an eMMC chip,
    - mount FAT filesystem using FATFS library (and format card, if the filesystem cannot be mounted),
    - register FAT filesystem in VFS, enabling C standard library and POSIX functions to be used.
2. Print information about the eMMC chip, such as name, type, capacity, and maximum supported frequency.
3. Create a file using `fopen` and write to it using `fprintf`.
4. Rename the file. Before renaming, check if destination file already exists using `stat` function, and remove it using `unlink` function.
5. Open renamed file for reading, read back the line, and print it to the terminal.

This example supports eMMC chips.


## Hardware

This example requires an ESP32-S3 development board and an eMMC chip. Default connections can be found in next chapter. The standard ESP32S3 devkits do not have an eMMC integrated. Considering the signal integrity issues, we suggesting you make your custom PCB board with ESP32-S3 module and an eMMC chip. The signal integrity issues will be more important, when the bus is working under higher speeds. This includes but not limited to increasing the frequency, switching to DDR mode. We will talk a bit more about signal integrity issues in a following chapter.

It is possible to connect an eMMC breakout adapter, but note that cable connections will decrease the signal integrity, leading communication timing violations. When in this condition, you may need to use a lower clock frequency, or switch to SDR mode.

Pull-up resistors is needed. As the SD specification and the eMMC datasheet clarify, minimum 10k pull-up resistors are required for the bus IOs to protect the IOs against bus floating issue. Note these pull-up resistors are needed, even if the pin is not used (For example, you use 1-line-mode, the pull-up resistor is still required for the D1 pin).


### Pin assignments for ESP32-S3

On ESP32-S3, SDMMC peripheral is connected to GPIO pins using GPIO matrix. This allows arbitrary GPIOs to be used to connect an SD card. In this example, GPIOs can be configured in two ways:

1. Using menuconfig: Run `idf.py menuconfig` in the project directory and open "eMMC Example Configuration" menu.
2. In the source code: See the initialization of ``sdmmc_slot_config_t slot_config`` structure in the example code.

The table below lists the default pin assignments.

ESP32-S3 pin  |  eMMC pin   | Notes
--------------|-------------|------------
GPIO34        | CLK         | 10k pullup
GPIO33        | CMD         | 10k pullup
GPIO37        | D0          | 10k pullup
GPIO38        | D1          | not used in 1-line mode; but card's D1 pin must have a 10k pullup
GPIO39        | D2          | not used in 1-line mode; but card's D2 pin must have a 10k pullup
GPIO36        | D3          | not used in 1-line mode, but card's D3 pin must have a 10k pullup
GPIO35        | D4          | not used in 1/4-line mode, but card's D4 pin must have a 10k pullup
GPIO40        | D5          | not used in 1/4-line mode, but card's D5 pin must have a 10k pullup
GPIO42        | D6          | not used in 1/4-line mode, but card's D6 pin must have a 10k pullup
GPIO41        | D7          | not used in 1/4-line mode, but card's D7 pin must have a 10k pullup


### Line modes

By default, this example uses 4 line SD mode, utilizing 6 pins: CLK, CMD, D0 - D3. It is possible to use 1-line mode (CLK, CMD, D0) or 8-line mode (CLK, CMD, D0 - D7) by changing "eMMC bus width" in the example configuration menu (see `CONFIG_EXAMPLE_EMMC_BUS_WIDTH_1/8`).


### Signal integrity

The signal integrity talking about in this chapter is mainly about is in time domain.

#### The background of this issue

- Voltage level switching needs time, and this is influenced by the pin load, connected capacitors and resistors, and other noises.
- Clock signal rises and falls will be measured by the eMMC by its minimum input high voltage (VIH) and maximum input low voltage (VIL).
- Input CMD and Data signals' rises and falls will be measured by the eMMC by its minimum input high voltage (VIH) and maximum input low voltage (VIL).
- Output CMD and Data signals' rises and falls will be measured by the eMMC by its minimum output high voltage (VOH) and maximum output low voltage (VOL).

Therefore when using cables to connect your eMMC and the ESP chip, the time that a pin voltage level increases to the VIL / VOL, or the time that a pin voltage level decreases to the VIH / VOH will be longer.

#### Requirements

- Input setup time (tSU), the data signal change should happen earlier than a valid clock edge, and should be earlier at least tSU time.
- Input hold time (tH), after the data signal changes, its status should hold at least tH time.

Both the tSU and the tH are usually in the unit of nanosecond (ns). When in DDR mode, the timing requirements is even more strict, as both the clock posedge and the negedge are valid.

The eMMC chip datasheet should specify the requirements of the tSU and the tH.


## How to use example

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
I (308) example: Initializing eMMC
I (318) example: Using SDMMC peripheral
I (318) example: Mounting filesystem
I (328) gpio: GPIO[34]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (338) gpio: GPIO[33]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (348) gpio: GPIO[37]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (348) gpio: GPIO[38]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (358) gpio: GPIO[39]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (368) gpio: GPIO[36]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (428) gpio: GPIO[36]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (428) example: Filesystem mounted
Name: IS008GP
Type: MMC
Speed: 40.00 MHz (limit: 40.00 MHz), DDR
Size: 7456MB
CSD: ver=3, sector_size=512, capacity=15269888 read_bl_len=9
EXT CSD: bus_width=4
I (438) example: Opening file /eMMC/hello.txt
I (448) example: File written
I (468) example: Renaming file /eMMC/hello.txt to /eMMC/foo.txt
I (468) example: Reading file /eMMC/foo.txt
I (468) example: Read from file: 'Hello IS008GP!'
I (468) example: Card unmounted
```

## Troubleshooting

### Card fails to initialize with `sdmmc_check_scr: send_scr returned 0xffffffff` error

Connections between the eMMC chip and the ESP chip are too long for the frequency used. Try using shorter connections, or try reducing the clock speed of the bus.

### Failure to mount filesystem

```
example: Failed to mount filesystem. If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.
```
The example will be able to mount only cards formatted using FAT32 filesystem. If the card is formatted as exFAT or some other filesystem, you have an option to format it in the example code. Enable the `EXAMPLE_FORMAT_IF_MOUNT_FAILED` menuconfig option, then build and flash the example.
