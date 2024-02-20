| Supported Targets | ESP32 | ESP32-P4 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- |

# SD Card example (SDMMC)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

__WARNING:__ This example can potentially delete all data from your SD card (when formatting is enabled). Back up your data first before proceeding.

This example demonstrates how to use an SD card with an ESP device. Example does the following steps:

1. Use an "all-in-one" `esp_vfs_fat_sdmmc_mount` function to:
    - initialize SDMMC peripheral,
    - probe and initialize an SD card,
    - mount FAT filesystem using FATFS library (and format card, if the filesystem cannot be mounted),
    - register FAT filesystem in VFS, enabling C standard library and POSIX functions to be used.
1. Print information about the card, such as name, type, capacity, and maximum supported frequency.
1. Create a file using `fopen` and write to it using `fprintf`.
1. Rename the file. Before renaming, check if destination file already exists using `stat` function, and remove it using `unlink` function.
1. Open renamed file for reading, read back the line, and print it to the terminal.
1. __OPTIONAL:__ Format the SD card, check if the file doesn't exist anymore.

This example supports SD (SDSC, SDHC, SDXC) cards and eMMC chips.

## Hardware

This example requires an ESP32 or ESP32-S3 development board with an SD card slot and an SD card.

Although it is possible to connect an SD card breakout adapter, keep in mind that connections using breakout cables are often unreliable and have poor signal integrity. You may need to use lower clock frequency when working with SD card breakout adapters.

This example doesn't utilize card detect (CD) and write protect (WP) signals from SD card slot.

### Pin assignments for ESP32

On ESP32, SDMMC peripheral is connected to specific GPIO pins using the IO MUX. GPIO pins cannot be customized. Please see the table below for the pin connections.

When using an ESP-WROVER-KIT board, this example runs without any extra modifications required. Only an SD card needs to be inserted into the slot.

ESP32 pin     | SD card pin | Notes
--------------|-------------|------------
GPIO14 (MTMS) | CLK         | 10k pullup in SD mode
GPIO15 (MTDO) | CMD         | 10k pullup in SD mode
GPIO2         | D0          | 10k pullup in SD mode, pull low to go into download mode (see Note about GPIO2 below!)
GPIO4         | D1          | not used in 1-line SD mode; 10k pullup in 4-line SD mode
GPIO12 (MTDI) | D2          | not used in 1-line SD mode; 10k pullup in 4-line SD mode (see Note about GPIO12 below!)
GPIO13 (MTCK) | D3          | not used in 1-line SD mode, but card's D3 pin must have a 10k pullup


### Pin assignments for ESP32-S3

On ESP32-S3, SDMMC peripheral is connected to GPIO pins using GPIO matrix. This allows arbitrary GPIOs to be used to connect an SD card. In this example, GPIOs can be configured in two ways:

1. Using menuconfig: Run `idf.py menuconfig` in the project directory and open "SD/MMC Example Configuration" menu.
2. In the source code: See the initialization of `sdmmc_slot_config_t slot_config` structure in the example code.

The table below lists the default pin assignments.

When using an ESP32-S3-USB-OTG board, this example runs without any extra modifications required. Only an SD card needs to be inserted into the slot.

ESP32-S3 pin  | SD card pin | Notes
--------------|-------------|------------
GPIO36        | CLK         | 10k pullup
GPIO35        | CMD         | 10k pullup
GPIO37        | D0          | 10k pullup
GPIO38        | D1          | not used in 1-line SD mode; 10k pullup in 4-line mode
GPIO33        | D2          | not used in 1-line SD mode; 10k pullup in 4-line mode
GPIO34        | D3          | not used in 1-line SD mode, but card's D3 pin must have a 10k pullup

### Pin assignments for ESP32-P4

On ESP32-P4, Slot 1 of the SDMMC peripheral is connected to GPIO pins using GPIO matrix. This allows arbitrary GPIOs to be used to connect an SD card. In this example, GPIOs can be configured in two ways:

1. Using menuconfig: Run `idf.py menuconfig` in the project directory and open "SD/MMC Example Configuration" menu.
2. In the source code: See the initialization of `sdmmc_slot_config_t slot_config` structure in the example code.

The table below lists the default pin assignments.

ESP32-P4 pin  | SD card pin | Notes
--------------|-------------|------------
GPIO43        | CLK         | 10k pullup
GPIO44        | CMD         | 10k pullup
GPIO39        | D0          | 10k pullup
GPIO40        | D1          | not used in 1-line SD mode; 10k pullup in 4-line mode
GPIO41        | D2          | not used in 1-line SD mode; 10k pullup in 4-line mode
GPIO42        | D3          | not used in 1-line SD mode, but card's D3 pin must have a 10k pullup

### 4-line and 1-line SD modes

By default, this example uses 4 line SD mode, utilizing 6 pins: CLK, CMD, D0 - D3. It is possible to use 1-line mode (CLK, CMD, D0) by changing "SD/MMC bus width" in the example configuration menu (see `CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_1`).

Note that even if card's D3 line is not connected to the ESP chip, it still has to be pulled up, otherwise the card will go into SPI protocol mode.

### Note about GPIO2 (ESP32 only)

GPIO2 pin is used as a bootstrapping pin, and should be low to enter UART download mode. One way to do this is to connect GPIO0 and GPIO2 using a jumper, and then the auto-reset circuit on most development boards will pull GPIO2 low along with GPIO0, when entering download mode.

- Some boards have pulldown and/or LED on GPIO2. LED is usually ok, but pulldown will interfere with D0 signals and must be removed. Check the schematic of your development board for anything connected to GPIO2.

### Note about GPIO12 (ESP32 only)

GPIO12 is used as a bootstrapping pin to select output voltage of an internal regulator which powers the flash chip (VDD_SDIO). This pin has an internal pulldown so if left unconnected it will read low at reset (selecting default 3.3V operation). When adding a pullup to this pin for SD card operation, consider the following:

- For boards which don't use the internal regulator (VDD_SDIO) to power the flash, GPIO12 can be pulled high.
- For boards which use 1.8V flash chip, GPIO12 needs to be pulled high at reset. This is fully compatible with SD card operation.
- On boards which use the internal regulator and a 3.3V flash chip, GPIO12 must be low at reset. This is incompatible with SD card operation.
    * In most cases, external pullup can be omitted and an internal pullup can be enabled using a `gpio_pullup_en(GPIO_NUM_12);` call. Most SD cards work fine when an internal pullup on GPIO12 line is enabled. Note that if ESP32 experiences a power-on reset while the SD card is sending data, high level on GPIO12 can be latched into the bootstrapping register, and ESP32 will enter a boot loop until external reset with correct GPIO12 level is applied.
    * Another option is to burn the flash voltage selection efuses. This will permanently select 3.3V output voltage for the internal regulator, and GPIO12 will not be used as a bootstrapping pin. Then it is safe to connect a pullup resistor to GPIO12. This option is suggested for production use.

The following command can be used to program flash voltage selection efuses **to 3.3V**:

```sh
    components/esptool_py/esptool/espefuse.py set_flash_voltage 3.3V
```

This command will burn the `XPD_SDIO_TIEH`, `XPD_SDIO_FORCE`, and `XPD_SDIO_REG` efuses. With all three burned to value 1, the internal VDD_SDIO flash voltage regulator is permanently enabled at 3.3V. See the technical reference manual for more details.

`espefuse.py` has a `--do-not-confirm` option if running from an automated flashing script.

See [the document about pullup requirements](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/sd_pullup_requirements.html) for more details about pullup support and compatibility of modules and development boards.

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
I (336) example: Initializing SD card
I (336) example: Using SDMMC peripheral
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

### Failure to download the example

```
Connecting........_____....._____....._____....._____....._____....._____....._____

A fatal error occurred: Failed to connect to Espressif device: Invalid head of packet (0x34)
```

Disconnect the SD card D0/MISO line from GPIO2 and try uploading again. Read "Note about GPIO2" above.

### Card fails to initialize with `sdmmc_init_sd_scr: send_scr (1) returned 0x107` error

Check connections between the card and the ESP32. For example, if you have disconnected GPIO2 to work around the flashing issue, connect it back and reset the ESP32 (using a button on the development board, or by pressing Ctrl-T Ctrl-R in IDF Monitor).

### Card fails to initialize with `sdmmc_check_scr: send_scr returned 0xffffffff` error

Connections between the card and the ESP32 are too long for the frequency used. Try using shorter connections, or try reducing the clock speed of SD interface.

### Failure to mount filesystem

```
example: Failed to mount filesystem. If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.
```
The example will be able to mount only cards formatted using FAT32 filesystem. If the card is formatted as exFAT or some other filesystem, you have an option to format it in the example code. Enable the `EXAMPLE_FORMAT_IF_MOUNT_FAILED` menuconfig option, then build and flash the example.

### Debug SD connections and pullup strength

> If the initialization of the SD card fails, initially follow the above options. If the issue persists, confirm the connection of pullups to the SD pins. To do this, enable the` Debug sd pin connections and pullup strength` option from menuconfig and rerun the code. This will provide the following result:

```
**** PIN recovery time ****

PIN 14 CLK  10044 cycles
PIN 15 CMD  10034 cycles
PIN  2  D0  10034 cycles
PIN  4  D1  10034 cycles
PIN 12  D2  10034 cycles
PIN 13  D3  10034 cycles

**** PIN recovery time with weak pullup ****

PIN 14 CLK  100 cycles
PIN 15 CMD  100 cycles
PIN  2  D0  100 cycles
PIN  4  D1  100 cycles
PIN 12  D2  100 cycles
PIN 13  D3  100 cycles

**** PIN voltage levels ****

PIN 14 CLK  0.6V
PIN 15 CMD  0.3V
PIN  2  D0  0.8V
PIN  4  D1  0.6V
PIN 12  D2  0.4V
PIN 13  D3  0.8V

**** PIN voltage levels with weak pullup ****

PIN 14 CLK  1.0V
PIN 15 CMD  1.1V
PIN  2  D0  1.0V
PIN  4  D1  1.0V
PIN 12  D2  1.0V
PIN 13  D3  1.2V

**** PIN cross-talk ****

              CLK   CMD    D0    D1    D2    D3
PIN 14 CLK     --   0.2V  0.1V  0.1V  0.1V  0.2V
PIN 15 CMD    0.1V   --   0.1V  0.1V  0.1V  0.1V
PIN  2  D0    0.1V  0.1V   --   0.2V  0.1V  0.1V
PIN  4  D1    0.1V  0.1V  0.3V   --   0.1V  0.1V
PIN 12  D2    0.1V  0.2V  0.2V  0.1V   --   0.1V
PIN 13  D3    0.1V  0.2V  0.1V  0.1V  0.1V   --

**** PIN cross-talk with weak pullup ****

              CLK   CMD    D0    D1    D2    D3
PIN 14 CLK     --   1.0V  1.0V  1.0V  1.0V  1.2V
PIN 15 CMD    0.9V   --   1.0V  1.0V  1.0V  1.2V
PIN  2  D0    0.9V  1.0V   --   1.0V  1.0V  1.2V
PIN  4  D1    0.9V  1.0V  1.2V   --   1.0V  1.2V
PIN 12  D2    0.9V  1.1V  1.2V  0.9V   --   1.2V
PIN 13  D3    0.9V  1.2V  1.1V  0.9V  0.9V   --
I (845) main_task: Returned from app_main()
```

In the absence of connected pullups and having the weak pullups enabled, you can assess the pullup connections by comparing PIN recovery time measured in CPU cycles. To check pullup connections, configure the pin as open drain, set it to low state, and count the cpu cycles consumed before returning to high state. If a pullup is connected, the pin will get back to high state after reasonably small cycle count, typically around 50-300 cycles, depending on pullup strength. If no pullup is connected, the PIN stays low and the measurement times out after 10000 cycles.

It will also provide the voltage levels at the corresponding SD pins. By default, this information is provided for ESP32 chip only, and for other chipsets, verify the availability of ADC pins for the respective GPIO using [this](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html#gpio-summary) and configure ADC mapped pins using menuconfig. Then test the voltage levels accordingly.

You can monitor the voltage levels of individual pins using `PIN voltage levels` and `PIN voltage levels with weak pullup`. However, if one pin being pulled low and experiencing interference with another pin, you can detect it through `PIN cross-talk` and `PIN cross-talk with weak pullup`. In the absence of pullups, voltage levels at each pin should range from 0 to 0.3V. With 10k pullups connected, the voltage will be between 3.1V to 3.3V, contingent on the connection between ADC pins and SD pins, and with weak pullups connected, it can fluctuate between 0.8V to 1.2V, depending on pullup strength.