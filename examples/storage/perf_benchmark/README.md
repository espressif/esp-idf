| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Storage performance benchmark example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

**WARNING: This example will format both your SPI flash "storage" partition and SD card (if used)!**

This example demonstrates a benchmark of a storage media such as SPI flash memory and SD card.
Only ESP32 and ESP32-S3 targets can use SDMMC mode when connecting to a SD card.

This example provides comparison between different types of storage in terms of speed under various configurable (from menuconfig) conditions

Currently these mediums and file-systems are supported:
- SPI Flash
    - Raw access
    - FATFS
    - SPIFFS
    - LittleFS
- SDMMC/SDSPI card
    - Raw access
    - FATFS
    - LittleFS

### Comparison scheme

For each filesystem, four sets of benchmarks are run:
- Target size R/W
- More than target size R/W (about target_size x 1.2)
- Less than target size R/W (about target_size / 1.2)
- Tiny size                 (255 bytes)

Some filesystems are also tested in two modes:
- New file - new file gets created every time
- Normal - the old file gets overwritten each time (`O_TRUNC`)

Every benchmark (except SPIFFS ones) is run multiple times (can be configuredd) for better accuracy, as the results often vary between runs.
We recommend using at least `100` runs, but default is set to `10` for convenience.

### Disclaimer

While this benchmark tries to give objective results, they may vary by significant amount with different settings, especially for filesystem with large amount of compile time configuration options such as LittleFS.

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

### Note about SPIFFS

The test for SPIFFS is run only once, because SPIFFS has a problem with deleting files.

## How to use example

### Configure

1. Set the target chip: `idf.py set-target TARGET` (replace TARGET with ESP32 chip series name)

1. Configure the example (`idf.py menuconfig` -> "Performance Benchmark Example Configuration") whether you want to:

- Set size of data to write
- Test internal SPI flash
  - Raw access (through wear-leveling)
  - FATFS
    - Set cluster size
  - SPIFFS
  - LittleFS

- Test SD card
  - Select SD card interface
    - SDSPI
    - SDMMC
  - SD card test configuration
    - Raw access
    - FATFS
    - LittleFS
    - Set SD card frequency
    - Set GPIO pins

Some configuration options may be available for ESP32 and ESP32-S3 only.

#### Note about LittleFS

LittleFS offers many other compile time configuration options which need to be set in it's component config.
These setting will have effect on it's performance.

#### Note about FATFS

To achieve the best performance the cluster should match with the target size, and be as high as possible.
However the higher the cluster size the more space will be wasted by partially filled clusters.
Keep in mind that the cluster size needs to be a multiple of sector size for SPI-flash that's 4096 and for SD-Card it's 512.
Also, the size of the storage medium determines maximum cluster size, for that reason this example uses 2MB of flash just for the filesystem (4MB by default, for smaller cluster sizes (16k) around 1MB should be enough.

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
I (302) main_task: Calling app_main()
I (302) example: Starting benchmark test with target size 65536 bytes
I (302) example: Internal flash test
I (302) example: Mountig WL layer...
I (622) example: WL layer mounted
[SPI Flash raw write                                    ] (100x)   96.610 ms    64.00 kB      0.647 MB/s
[SPI Flash raw read                                     ] (100x)    8.076 ms    64.00 kB      7.739 MB/s
I (11242) example: WL layer unmounted
I (11242) example: Mounting FATFS partition, with cluster size 65536 bytes
W (11242) vfs_fat_spiflash: f_mount failed (13)
I (11252) vfs_fat_spiflash: Formatting FATFS partition, allocation unit size=65536
I (11672) vfs_fat_spiflash: Mounting again
I (11672) example: FATFS mounted to /spiflash
[SPI FATFS (new file): write target size                ] (100x) 1053.682 ms    64.00 kB      0.059 MB/s
[SPI FATFS (new file): read target size                 ] (100x)    8.124 ms    64.00 kB      7.694 MB/s
[SPI FATFS (new file): write more than target size      ] (100x) 1259.972 ms    76.80 kB      0.060 MB/s
[SPI FATFS (new file): read more than target size       ] (100x)   10.728 ms    76.80 kB      6.991 MB/s
[SPI FATFS (new file): write less than target size      ] (100x)  850.835 ms    53.33 kB      0.061 MB/s
[SPI FATFS (new file): read less than target size       ] (100x)    7.156 ms    53.33 kB      7.278 MB/s
[SPI FATFS (new file): write tiny size                  ] (100x)    1.180 ms     0.25 kB      0.206 MB/s
[SPI FATFS (new file): read tiny size                   ] (100x)    0.585 ms     0.25 kB      0.416 MB/s
I (510282) example: FATFS partition unmounted
I (510282) example: Mounting SPIFFS partition...
W (510282) SPIFFS: mount failed, -10025. formatting...
I (530862) example: SPIFFS mounted to /spiflash
[SPI SPIFFS (new file): write target size               ] (1x)  464.198 ms    64.00 kB      0.135 MB/s
[SPI SPIFFS (new file): read target size                ] (1x)  137.664 ms    64.00 kB      0.454 MB/s
[SPI SPIFFS (new file): write more than target size     ] (1x)  555.745 ms    76.80 kB      0.135 MB/s
[SPI SPIFFS (new file): read more than target size      ] (1x)  146.080 ms    76.80 kB      0.513 MB/s
[SPI SPIFFS (new file): write less than target size     ] (1x)  390.201 ms    53.33 kB      0.133 MB/s
[SPI SPIFFS (new file): read less than target size      ] (1x)  128.494 ms    53.33 kB      0.405 MB/s
[SPI SPIFFS (new file): write tiny size                 ] (1x)    0.050 ms     0.25 kB      4.864 MB/s
[SPI SPIFFS (new file): read tiny size                  ] (1x)    0.412 ms     0.25 kB      0.590 MB/s
I (534762) example: SPIFFS partition unmounted
I (534762) example: Mounting LittleFS partition...
E (534772) esp_littlefs: /IDF/examples/storage/perf_benchmark/managed_components/joltwallet__littlefs/src/littlefs/lfs.c:1366:error: Corrupted dir pair at {0x0, 0x1}

W (534782) esp_littlefs: mount failed,  (-84). formatting...
I (534862) example: LittleFS mounted to /spiflash
[SPI LittleFS (new file): write target size             ] (100x)  946.405 ms    64.00 kB      0.066 MB/s
[SPI LittleFS (new file): read target size              ] (100x)   28.988 ms    64.00 kB      2.156 MB/s
[SPI LittleFS (new file): write more than target size   ] (100x) 1176.518 ms    76.80 kB      0.064 MB/s
[SPI LittleFS (new file): read more than target size    ] (100x)   39.843 ms    76.80 kB      1.882 MB/s
[SPI LittleFS (new file): write less than target size   ] (100x)  821.555 ms    53.33 kB      0.063 MB/s
[SPI LittleFS (new file): read less than target size    ] (100x)   25.848 ms    53.33 kB      2.015 MB/s
[SPI LittleFS (new file): write tiny size               ] (100x)    0.043 ms     0.25 kB      5.635 MB/s
[SPI LittleFS (new file): read tiny size                ] (100x)    0.036 ms     0.25 kB      6.685 MB/s
I (849202) example: LittleFS partition unmounted
I (849202) example: SD card test
I (849202) example: Initializing SD card
I (849212) sd_utils: Using SDMMC peripheral
I (849212) example: Mounting SD card - raw access
I (849222) gpio: GPIO[13]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (849272) sd_utils: SD card mounted - raw access
Name: SD64G
Type: SDHC/SDXC
Speed: 40.00 MHz (limit: 40.00 MHz)
Size: 59640MB
CSD: ver=2, sector_size=512, capacity=122142720 read_bl_len=9
SSR: bus_width=4
[SD raw write                                           ] (100x)    6.757 ms    16.00 kB      2.312 MB/s
[SD raw read                                            ] (100x)    3.856 ms    16.00 kB      4.053 MB/s
[SD raw write                                           ] (100x)   13.380 ms    32.00 kB      2.336 MB/s
[SD raw read                                            ] (100x)    3.852 ms    32.00 kB      8.112 MB/s
[SD raw write                                           ] (100x)   13.145 ms    48.00 kB      3.566 MB/s
[SD raw read                                            ] (100x)    3.853 ms    48.00 kB     12.167 MB/s
[SD raw write                                           ] (100x)    4.924 ms    64.00 kB     12.692 MB/s
[SD raw read                                            ] (100x)    3.855 ms    64.00 kB     16.212 MB/s
I (854662) example: SD card unmounted - raw access
I (854662) example: Mounting SD card - FATFS, with cluster size 65536 bytes
I (854672) gpio: GPIO[13]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
W (854722) vfs_fat_sdmmc: failed to mount card (13)
W (854722) vfs_fat_sdmmc: partitioning card
W (854722) vfs_fat_sdmmc: formatting card, allocation unit size=65536
W (857742) vfs_fat_sdmmc: mounting again
Name: SD64G
Type: SDHC/SDXC
Speed: 40.00 MHz (limit: 40.00 MHz)
Size: 59640MB
CSD: ver=2, sector_size=512, capacity=122142720 read_bl_len=9
SSR: bus_width=4
I (857742) example: SD card mounted - FATFS
[SD FATFS (new file): write target size                 ] (100x)    6.236 ms    64.00 kB     10.022 MB/s
[SD FATFS (new file): read target size                  ] (100x)    3.923 ms    64.00 kB     15.932 MB/s
[SD FATFS (new file): write more than target size       ] (100x)    9.519 ms    76.80 kB      7.879 MB/s
[SD FATFS (new file): read more than target size        ] (100x)    5.690 ms    76.80 kB     13.181 MB/s
[SD FATFS (new file): write less than target size       ] (100x)    6.953 ms    53.33 kB      7.491 MB/s
[SD FATFS (new file): read less than target size        ] (100x)    3.624 ms    53.33 kB     14.372 MB/s
[SD FATFS (new file): write tiny size                   ] (100x)    0.423 ms     0.25 kB      0.575 MB/s
[SD FATFS (new file): read tiny size                    ] (100x)    0.345 ms     0.25 kB      0.704 MB/s
[SD FATFS: write target size                            ] (100x)    5.186 ms    64.00 kB     12.051 MB/s
[SD FATFS: read target size                             ] (100x)    3.907 ms    64.00 kB     15.995 MB/s
[SD FATFS: write more than target size                  ] (100x)  136.273 ms    76.80 kB      0.550 MB/s
[SD FATFS: read more than target size                   ] (100x)   46.052 ms    76.80 kB      1.629 MB/s
[SD FATFS: write less than target size                  ] (100x)  105.132 ms    53.33 kB      0.495 MB/s
[SD FATFS: read less than target size                   ] (100x)   31.151 ms    53.33 kB      1.672 MB/s
[SD FATFS: write tiny size                              ] (100x)    0.456 ms     0.25 kB      0.533 MB/s
[SD FATFS: read tiny size                               ] (100x)    0.200 ms     0.25 kB      1.217 MB/s
I (899172) example: SD card unmounted - FATFS
I (899172) example: Mounting SD card - LittleFS
I (899182) gpio: GPIO[13]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (899232) sd_utils: SD card mounted - raw access
Name: SD64G
Type: SDHC/SDXC
Speed: 40.00 MHz (limit: 40.00 MHz)
Size: 59640MB
CSD: ver=2, sector_size=512, capacity=122142720 read_bl_len=9
SSR: bus_width=4
I (899232) esp_littlefs: Using SD card handle 0x3ffd8880 for LittleFS mount
W (899242) esp_littlefs: SD card is too big (sector=512, count=122142720; total=62537072640 bytes), throttling to maximum possible 8388607 blocks
E (899262) esp_littlefs: /IDF/examples/storage/perf_benchmark/managed_components/joltwallet__littlefs/src/littlefs/lfs.c:1366:error: Corrupted dir pair at {0x0, 0x1}

W (899272) esp_littlefs: mount failed,  (-84). formatting...
I (901892) esp_littlefs: SD card formatted!
I (901932) example: LittleFS mounted to /sdcard
I (901932) example: SD card mounted - LittleFS
[SD LittleFS (new file): write target size              ] (100x) 2723.682 ms    64.00 kB      0.023 MB/s
[SD LittleFS (new file): read target size               ] (100x)  235.108 ms    64.00 kB      0.266 MB/s
[SD LittleFS (new file): write more than target size    ] (100x) 3312.231 ms    76.80 kB      0.023 MB/s
[SD LittleFS (new file): read more than target size     ] (100x)  402.211 ms    76.80 kB      0.186 MB/s
[SD LittleFS (new file): write less than target size    ] (100x) 2311.122 ms    53.33 kB      0.023 MB/s
[SD LittleFS (new file): read less than target size     ] (100x)  226.808 ms    53.33 kB      0.230 MB/s
[SD LittleFS (new file): write tiny size                ] (100x)   17.263 ms     0.25 kB      0.014 MB/s
[SD LittleFS (new file): read tiny size                 ] (100x)    0.348 ms     0.25 kB      0.699 MB/s
[SD LittleFS: write target size                         ] (100x) 2928.440 ms    64.00 kB      0.021 MB/s
[SD LittleFS: read target size                          ] (100x)  692.110 ms    64.00 kB      0.090 MB/s
[SD LittleFS: write more than target size               ] (100x) 3547.740 ms    76.80 kB      0.021 MB/s
[SD LittleFS: read more than target size                ] (100x)  952.181 ms    76.80 kB      0.079 MB/s
[SD LittleFS: write less than target size               ] (100x) 2418.240 ms    53.33 kB      0.022 MB/s
[SD LittleFS: read less than target size                ] (100x)  520.300 ms    53.33 kB      0.100 MB/s
[SD LittleFS: write tiny size                           ] (100x)   10.540 ms     0.25 kB      0.023 MB/s
[SD LittleFS: read tiny size                            ] (100x)    0.840 ms     0.25 kB      0.289 MB/s
I (2958872) example: SD card unmounted - LittleFS
I (2958872) main_task: Returned from app_main()
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
