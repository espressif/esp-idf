# SD Card example

This example demonstrates how to use an SD card with ESP32. Example does the following steps:

1. Use an "all-in-one" `esp_vfs_fat_sdmmc_mount` function to:
    - initialize SDMMC peripheral,
    - probe and initialize the card connected to SD/MMC slot 1 (HS2_CMD, HS2_CLK, HS2_D0, HS2_D1, HS2_D2, HS2_D3 lines),
    - mount FAT filesystem using FATFS library (and format card, if the filesystem can not be mounted),
    - register FAT filesystem in VFS, enabling C standard library and POSIX functions to be used.
2. Print information about the card, such as name, type, capacity, and maximum supported frequency.
3. Create a file using `fopen` and write to it using `fprintf`.
4. Rename the file. Before renaming, check if destination file already exists using `stat` function, and remove it using `unlink` function.
5. Open renamed file for reading, read back the line, and print it to the terminal.

*Note:* despite the name, `sdmmc` component doesn't support MMC/eMMC cards yet. It is also possible to extend `sdmmc` component to support SPI mode with SD cards via SPI peripheral.

## Hardware

To run this example, ESP32 development board needs to be connected to SD card as follows:

ESP32 pin     | SD card pin | SPI pin | Notes
--------------|-------------|---------|------------
GPIO14 (MTMS) | CLK         | SCK     | 10k pullup in SD mode
GPIO15 (MTDO) | CMD         | MOSI    | 10k pullup, both in SD and SPI modes
GPIO2         | D0          | MISO    | 10k pullup in SD mode, pull low to go into download mode (see note below!)
GPIO4         | D1          | N/C     | not used in 1-line SD mode; 10k pullup in 4-line SD mode
GPIO12 (MTDI) | D2          | N/C     | not used in 1-line SD mode; 10k pullup in 4-line SD mode (see note below!)
GPIO13 (MTCK) | D3          | CS      | not used in 1-line SD mode, but card's D3 pin must have a 10k pullup
N/C           | CD          |         | optional, not used in the example
N/C           | WP          |         | optional, not used in the example

This example doesn't utilize card detect (CD) and write protect (WP) signals from SD card slot.

With the given pinout for SPI mode, same connections between the SD card and ESP32 can be used to test both SD and SPI modes, provided that the appropriate pullups are in place. In SPI mode, pins can be customized. See the initialization of ``sdspi_slot_config_t`` structure in the example code.

### Note about GPIO2

GPIO2 pin is used as a bootstrapping pin, and should be low to enter UART download mode. One way to do this is to connect GPIO0 and GPIO2 using a jumper, and then the auto-reset circuit on most development boards will pull GPIO2 low along with GPIO0, when entering download mode.

- Some boards have pulldown and/or LED on GPIO2. LED is usually ok, but pulldown will interfere with D0 signals and must be removed. Check the schematic of your development board for anything connected to GPIO2. 

### Note about GPIO12

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

## 4-line and 1-line modes

By default, example code uses the following initializer for SDMMC host peripheral configuration:

```c++
sdmmc_host_t host = SDMMC_HOST_DEFAULT();
```

Among other things, this sets `host.flags` to `SDMMC_HOST_FLAG_4BIT`, which means that SD/MMC driver will switch to 4-line mode when initializing the card (initial communication always happens in 1-line mode). If some of the card's D1, D2, D3 pins are not connected to the ESP32, set `host.flags` to `SDMMC_HOST_FLAG_1BIT` — then the SD/MMC driver will not attempt to switch to 4-line mode.
Note that even if card's D3 line is not connected to the ESP32, it still has to be pulled up, otherwise the card will go into SPI protocol mode.

## SPI mode

By default, the example uses SDMMC Host peripheral to access SD card. To use SPI peripheral instead, uncomment ``#define USE_SPI_MODE`` in the example code.

## Example output

Here is an example console output. In this case a 128MB SDSC card was connected, and `format_if_mount_failed` parameter was set to `true` in the source code. Card was unformatted, so the initial mount has failed. Card was then partitioned, formatted, and mounted again.

```
I (1776) example: Initializing SD card
W (1856) vfs_fat_sdmmc: failed to mount card (13)
W (1856) vfs_fat_sdmmc: partitioning card
W (1856) vfs_fat_sdmmc: formatting card
W (2726) vfs_fat_sdmmc: mounting again
I (2736) example: Card info:
I (2736) example: Name: SU128
I (2736) example: Type: SDSC
I (2736) example: Capacity: 120 MB
I (2736) example: Max clock speed: 25 MHz
I (2736) example: Opening file
I (2756) example: File written
I (2756) example: Renaming file
I (2756) example: Reading file
I (2756) example: Read from file: 'Hello SU128!'
I (2756) example: Card unmounted
```

