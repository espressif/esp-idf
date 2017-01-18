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

ESP32 pin     | SD card pin | Notes
--------------|-------------|------------
GPIO14 (MTMS) | CLK         | 10k pullup
GPIO15 (MTDO) | CMD         | 10k pullup
GPIO2         | D0          | 10k pullup, pull low to go into download mode
GPIO4         | D1          | 10k pullup; not used in 1-line mode
GPIO12 (MTDI) | D2          | otherwise 10k pullup (see note below!); not used in 1-line mode
GPIO13 (MTCK) | D3          | 10k pullup needed at card side, even in 1-line mode
N/C           | CD          |
N/C           | WP          |

This example doesn't utilize card detect (CD) and write protect (WP) signals from SD card slot.

### Note about GPIO2

GPIO2 pin is used as a bootstrapping pin, and should be low to enter UART download mode. One way to do this is to connect GPIO0 and GPIO2 using a jumper, and then the auto-reset circuit on most development boards will pull GPIO2 low along with GPIO0, when entering download mode.

### Note about GPIO12

GPIO12 is used as a bootstrapping pin to select output voltage of an internal regulator which powers the flash chip. 

- For boards which don't use the internal regulator, GPIO12 can be pulled high.
- On boards which use the internal regulator and a 3.3V flash chip, GPIO12 should be pulled up high, which is compatible with SD card operation.
- For boards which use 3.3V flash chip, GPIO12 needs to be low at reset.
    * In this case, internal pullup can be enabled using a `gpio_pullup_en(GPIO_NUM_12);` call. Most SD cards work fine when an internal pullup on GPIO12 line is enabled. Note that if ESP32 experiences a power-on reset while the SD card is sending data, high level on GPIO12 can be latched into the bootstrapping register, and ESP32 will enter a boot loop until external reset with correct GPIO12 level is applied.
    * Another option is to program flash voltage selection efuses: set `XPD_SDIO_TIEH=1`, `XPD_SDIO_FORCE=1`, and `XPD_SDIO_REG = 1`. This will permanently select 3.3V output voltage for the internal regulator, and GPIO12 will not be used as a bootstrapping pin anymore. Then it is safe to connect a pullup resistor to GPIO12. This option is suggested for production use.

The following commands can be used to program flash voltage selection efuses **to 3.3V**:

```sh
	# Override flash regulator configuration using efuses
	components/esptool_py/esptool/espefuse.py burn_efuse XPD_SDIO_FORCE
	# Select 3.3V output voltage
	components/esptool_py/esptool/espefuse.py burn_efuse XPD_SDIO_TIEH
	# Enable internal voltage regulator
	components/esptool_py/esptool/espefuse.py burn_efuse XPD_SDIO_REG
```

## 4-line and 1-line modes

By default, example code uses the following initializer for SDMMC host peripheral configuration:

```c++
sdmmc_host_t host = SDMMC_HOST_DEFAULT();
```

Among other things, this sets `host.flags` to `SDMMC_HOST_FLAG_4BIT`, which means that SD/MMC driver will switch to 4-line mode when initializing the card (initial communication always happens in 1-line mode). If some of D1, D2, D3 pins are not connected to the card, set `host.flags` to `SDMMC_HOST_FLAG_1BIT` — then the SD/MMC driver will not attempt to switch to 4-line mode.
Note that even if D3 line is not connected to the ESP32, it still has to be pulled up at card side, otherwise the card will go into SPI protocol mode.


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

