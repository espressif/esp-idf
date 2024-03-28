| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Storage performance benchmark example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

**WARNING: This example will format both your SPI flash "storage" partition and SD card (if used)!**

This example demonstrates a benchmark of a storage media such as SPI flash memory and SD card.
Only ESP32 and ESP32-S3 targets can use SDMMC mode when connecting to a SD card.

### Pin assignments for SD card connection

The GPIO pin numbers used to connect an SD card can be customized using the following:

- Run `idf.py menuconfig` in the project directory and open "Performance Benchmark Example Configuration" menu.

However, pins cannot be customized when using ESP32 target in SDMMC mode (`SDMMC_HOST_SLOT_1`).

This example doesn't utilize card detect (CD) and write protect (WP) signals from SD card slot.

The table below shows the default pin assignments.

SD card pin | SPI pin | ESP32 pin     | ESP32-S2 | ESP32-S3 | ESP32-H2 | ESP32-C3 and other chips | Notes
------------|---------|---------------|----------|----------|----------|--------------------------|-------------
 D0         | MISO    | GPIO2         | GPIO37   | GPIO37   | GPIO0    | GPIO6                    | 10k pullup
 D1         | -       | GPIO4         | -        | GPIO38   | -        | -                        | not used in 1-line SD mode; 10k pullup in 4-line mode
 D2         | -       | GPIO12 (MTDI) | -        | GPIO33   | -        | -                        | not used in 1-line SD mode; 10k pullup in 4-line mode
 D3         | CS      | GPIO13 (MTCK) | GPIO34   | GPIO34   | GPIO1    | GPIO1                    | not used in 1-line SD mode, but card's D3 pin must have a 10k pullup
 CLK        | SCK     | GPIO14 (MTMS) | GPIO36   | GPIO36   | GPIO4    | GPIO5                    | 10k pullup
 CMD        | MOSI    | GPIO15 (MTDO) | GPIO35   | GPIO35   | GPIO5    | GPIO4                    | 10k pullup

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

### Configure

1. Set the target chip: `idf.py set-target TARGET` (replace TARGET with ESP32 chip series name)

1. Configure the example (`idf.py menuconfig` -> "Performance Benchmark Example Configuration") whether you want to:

- Test internal SPI flash
  - Raw access
  - FATFS
  - SPIFFS

- Test SD card
  - Select SD card interface
    - SDSPI
    - SDMMC
  - SD card test configuration
    - Raw access
    - FATFS
    - Set SD card frequency
    - Set GPIO pins

Some configuration options may be available for ESP32 and ESP32-S3 only.

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(Replace PORT with serial port name.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

```
I (345) example: Start of the example
I (355) example: Internal flash test
I (355) example: Mountig WL layer...
I (415) example: WL layer mounted
I (505) example: Test with 128kiB file
Wrote 131072 bytes in 320.295ms (0.390MiB/s) to address 0
I (895) example: WL layer unmounted
I (895) example: Mounting FATFS partition...
W (895) vfs_fat_spiflash: f_mount failed (13)
I (895) vfs_fat_spiflash: Formatting FATFS partition, allocation unit size=16384
I (1105) vfs_fat_spiflash: Mounting again
I (1105) example: FATFS mounted to /spiflash
I (1205) example: Test with 128kiB file
Wrote 131072 bytes (buffer size 4096B) in 1295.988ms (0.096MiB/s)
Wrote 131072 bytes (buffer size 8192B) in 1277.569ms (0.098MiB/s)
Wrote 131072 bytes (buffer size 16384B) in 1439.405ms (0.087MiB/s)
Read 131072 bytes (buffer size 4096B) in 17.721ms (7.054MiB/s)
Read 131072 bytes (buffer size 8192B) in 16.945ms (7.377MiB/s)
Read 131072 bytes (buffer size 16384B) in 16.596ms (7.532MiB/s)
I (5705) example: Test with 256kiB file
Wrote 262144 bytes (buffer size 4096B) in 2529.380ms (0.099MiB/s)
Wrote 262144 bytes (buffer size 8192B) in 2867.352ms (0.087MiB/s)
Wrote 262144 bytes (buffer size 16384B) in 2879.113ms (0.087MiB/s)
Read 262144 bytes (buffer size 4096B) in 34.809ms (7.182MiB/s)
Read 262144 bytes (buffer size 8192B) in 33.315ms (7.504MiB/s)
Read 262144 bytes (buffer size 16384B) in 32.601ms (7.668MiB/s)
I (14535) example: Test with 512kiB file
Wrote 524288 bytes (buffer size 4096B) in 5119.009ms (0.098MiB/s)
Wrote 524288 bytes (buffer size 8192B) in 5897.010ms (0.085MiB/s)
Wrote 524288 bytes (buffer size 16384B) in 5888.743ms (0.085MiB/s)
Read 524288 bytes (buffer size 4096B) in 68.975ms (7.249MiB/s)
Read 524288 bytes (buffer size 8192B) in 66.063ms (7.569MiB/s)
Read 524288 bytes (buffer size 16384B) in 64.624ms (7.737MiB/s)
I (32295) example: FATFS partition unmounted
I (32295) example: Mounting SPIFFS partition...
W (32295) SPIFFS: mount failed, -10025. formatting...
I (38195) example: SPIFFS mounted to /spiflash
I (41555) example: Partition size: total: 896321, used: 0
I (41645) example: Test with 128kiB file
Wrote 131072 bytes (buffer size 4096B) in 819.950ms (0.152MiB/s)
Wrote 131072 bytes (buffer size 8192B) in 777.949ms (0.161MiB/s)
Wrote 131072 bytes (buffer size 16384B) in 751.682ms (0.166MiB/s)
Read 131072 bytes (buffer size 4096B) in 101.810ms (1.228MiB/s)
Read 131072 bytes (buffer size 8192B) in 99.518ms (1.256MiB/s)
Read 131072 bytes (buffer size 16384B) in 98.472ms (1.269MiB/s)
I (45335) example: Test with 256kiB file
Wrote 262144 bytes (buffer size 4096B) in 1650.525ms (0.151MiB/s)
Wrote 262144 bytes (buffer size 8192B) in 3220.004ms (0.078MiB/s)
Wrote 262144 bytes (buffer size 16384B) in 7139.803ms (0.035MiB/s)
Read 262144 bytes (buffer size 4096B) in 204.933ms (1.220MiB/s)
Read 262144 bytes (buffer size 8192B) in 200.330ms (1.248MiB/s)
Read 262144 bytes (buffer size 16384B) in 198.129ms (1.262MiB/s)
I (60365) example: Test with 512kiB file
Wrote 524288 bytes (buffer size 4096B) in 13917.135ms (0.036MiB/s)
Wrote 524288 bytes (buffer size 8192B) in 14302.046ms (0.035MiB/s)
Wrote 524288 bytes (buffer size 16384B) in 14411.794ms (0.035MiB/s)
Read 524288 bytes (buffer size 4096B) in 317.944ms (1.573MiB/s)
Read 524288 bytes (buffer size 8192B) in 308.865ms (1.619MiB/s)
Read 524288 bytes (buffer size 16384B) in 304.305ms (1.643MiB/s)
I (108835) example: SPIFFS partition unmounted
I (108835) example: SD card test
I (108845) example: Initializing SD card
I (108845) example: Using SDMMC peripheral
I (108855) example: Mounting SD card - raw access
I (108855) gpio: GPIO[36]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (108865) gpio: GPIO[35]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (108875) gpio: GPIO[37]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (108885) gpio: GPIO[38]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (108895) gpio: GPIO[33]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (108905) gpio: GPIO[34]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (108955) gpio: GPIO[34]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (108965) example: SD card mounted - raw access
Name: USD00
Type: SDHC/SDXC
Speed: 40.00 MHz (limit: 40.00 MHz)
Size: 15080MB
CSD: ver=2, sector_size=512, capacity=30883840 read_bl_len=9
SSR: bus_width=4
Wrote 65536 bytes in 25.321ms (2.468MiB/s) to sector 415075
Read 65536 bytes in 3.809ms (16.409MiB/s) from sector 415075
Wrote 131072 bytes in 7.415ms (16.858MiB/s) to sector 823598
Read 131072 bytes in 7.149ms (17.485MiB/s) from sector 823598
Wrote 196608 bytes in 10.829ms (17.315MiB/s) to sector 180761
Read 196608 bytes in 10.539ms (17.791MiB/s) from sector 180761
I (109045) example: SD card unmounted - raw access
I (109045) example: Mounting SD card - FATFS
I (109045) gpio: GPIO[36]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (109055) gpio: GPIO[35]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (109065) gpio: GPIO[37]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (109075) gpio: GPIO[38]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (109085) gpio: GPIO[33]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (109095) gpio: GPIO[34]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (109145) gpio: GPIO[34]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
W (109155) vfs_fat_sdmmc: failed to mount card (13)
W (109155) vfs_fat_sdmmc: partitioning card
W (109155) vfs_fat_sdmmc: formatting card, allocation unit size=16384
W (110525) vfs_fat_sdmmc: mounting again
Name: USD00
Type: SDHC/SDXC
Speed: 40.00 MHz (limit: 40.00 MHz)
Size: 15080MB
CSD: ver=2, sector_size=512, capacity=30883840 read_bl_len=9
SSR: bus_width=4
I (110535) example: SD card mounted - FATFS
I (110715) example: Test with 1MiB file
Wrote 1048576 bytes (buffer size 4096B) in 200.241ms (4.994MiB/s)
Wrote 1048576 bytes (buffer size 8192B) in 129.756ms (7.707MiB/s)
Wrote 1048576 bytes (buffer size 16384B) in 91.955ms (10.875MiB/s)
Wrote 1048576 bytes (buffer size 32768B) in 91.424ms (10.938MiB/s)
Read 1048576 bytes (buffer size 4096B) in 121.086ms (8.259MiB/s)
Read 1048576 bytes (buffer size 8192B) in 87.896ms (11.377MiB/s)
Read 1048576 bytes (buffer size 16384B) in 72.525ms (13.788MiB/s)
Read 1048576 bytes (buffer size 32768B) in 74.664ms (13.393MiB/s)
I (111655) example: Test with 4MiB file
Wrote 4194304 bytes (buffer size 4096B) in 797.167ms (5.018MiB/s)
Wrote 4194304 bytes (buffer size 8192B) in 533.538ms (7.497MiB/s)
Wrote 4194304 bytes (buffer size 16384B) in 530.830ms (7.535MiB/s)
Wrote 4194304 bytes (buffer size 32768B) in 497.953ms (8.033MiB/s)
Read 4194304 bytes (buffer size 4096B) in 473.495ms (8.448MiB/s)
Read 4194304 bytes (buffer size 8192B) in 351.240ms (11.388MiB/s)
Read 4194304 bytes (buffer size 16384B) in 289.767ms (13.804MiB/s)
Read 4194304 bytes (buffer size 32768B) in 287.260ms (13.925MiB/s)
I (115495) example: Test with 16MiB file
Wrote 16777216 bytes (buffer size 4096B) in 4754.141ms (3.365MiB/s)
Wrote 16777216 bytes (buffer size 8192B) in 2290.545ms (6.985MiB/s)
Wrote 16777216 bytes (buffer size 16384B) in 2088.811ms (7.660MiB/s)
Wrote 16777216 bytes (buffer size 32768B) in 2148.595ms (7.447MiB/s)
Read 16777216 bytes (buffer size 4096B) in 1899.620ms (8.423MiB/s)
Read 16777216 bytes (buffer size 8192B) in 1419.345ms (11.273MiB/s)
Read 16777216 bytes (buffer size 16384B) in 1158.909ms (13.806MiB/s)
Read 16777216 bytes (buffer size 32768B) in 1148.426ms (13.932MiB/s)
I (132515) example: SD card unmounted - FATFS
I (132515) example: End of the example
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
