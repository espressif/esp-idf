| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# SPI NAND Flash LittleFS Example (Block Device Layer)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example mounts LittleFS on external SPI NAND Flash using the wear-leveling block device (`esp_blockdev_t`) from the [`espressif/spi_nand_flash`](https://components.espressif.com/components/espressif/spi_nand_flash) component and **`joltwallet/littlefs` blockdev APIs** (`esp_vfs_littlefs_register` with `.blockdev`).

## Requirements

- ESP-IDF **6.0 or later** (CMake fails configure on older IDF)
- **`CONFIG_NAND_FLASH_ENABLE_BDL=y`** (enabled by default via `sdkconfig.defaults`)
- **`joltwallet/littlefs` >= 1.21.0** (blockdev support; pulled via Component Manager)
- **`CONFIG_LITTLEFS_CACHE_SIZE` >= NAND page size** (example defaults to **4096**)
- External SPI NAND Flash chip

## Configuration

- `CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED` — format LittleFS if the first mount fails
- Keep **`CONFIG_NAND_FLASH_ENABLE_BDL` enabled** (Component config → SPI NAND Flash)
- Set LittleFS cache size in menuconfig (Component config → LittleFS → Cache Size), or via `sdkconfig.defaults`

## Hardware Required

* Any ESP board from the supported targets list above
* An external SPI NAND Flash chip wired to the IOMUX pins below

The example uses SPI3 on ESP32 and SPI2 on every other target. It runs in single-IO mode and drives only MOSI, MISO, CLK, and CS. Tie the NAND WP# and HOLD# pins high (or follow the chip datasheet if internal pull-ups are enough). `PIN_WP` and `PIN_HD` in the example source are not connected to the SPI bus.

These are the default pins in `spi_nand_flash_littlefs_example_main.c`. To use other GPIOs, change the `PIN_*` macros; the driver then routes the signals through the GPIO matrix.

| Target | Host | MOSI | MISO | CLK | CS |
| --- | --- | ---: | ---: | ---: | ---: |
| ESP32 | SPI3 | 23 | 19 | 18 | 5 |
| ESP32-C2 | SPI2 | 7 | 2 | 6 | 10 |
| ESP32-C3 | SPI2 | 7 | 2 | 6 | 10 |
| ESP32-C5 | SPI2 | 7 | 2 | 6 | 10 |
| ESP32-C6 | SPI2 | 7 | 2 | 6 | 16 |
| ESP32-C61 | SPI2 | 7 | 2 | 6 | 8 |
| ESP32-H2 | SPI2 | 5 | 0 | 4 | 1 |
| ESP32-H21 | SPI2 | 3 | 4 | 2 | 12 |
| ESP32-H4 | SPI2 | 17 | 15 | 16 | 20 |
| ESP32-P4 | SPI2 | 8 | 10 | 9 | 7 |
| ESP32-S2 | SPI2 | 11 | 13 | 12 | 10 |
| ESP32-S3 | SPI2 | 11 | 13 | 12 | 10 |
| ESP32-S31 | SPI2 | 21 | 22 | 20 | 23 |

## How to Use Example

```bash
cd $IDF_PATH/examples/storage/spi_nand_flash/littlefs
idf.py set-target esp32
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

The example:

1. Initializes SPI and the NAND wear-leveling BDL via `spi_nand_flash_init_with_layers()`
2. Mounts LittleFS with `esp_vfs_littlefs_register()` (`.blockdev = wl_bdl`)
3. Writes and reads `hello.txt`
4. Unmounts with `esp_vfs_littlefs_unregister_blockdev()` (releases the BDL and NAND layers)

## Example Output

```
I (315) example: DMA CHANNEL: 3
W (xxx) esp_littlefs: mount failed, ... formatting...   # only if FORMAT enabled and media needs format
I (6655) example: LittleFS: 114240 kB total, 8 kB used
I (6655) example: Opening file
I (6685) example: File written
I (6685) example: Reading file
I (6685) example: Read from file: 'Written using ESP-IDF v6.0-...'
I (6695) example: Done
```

## Related Examples

- Internal-flash LittleFS: [`examples/storage/littlefs`](../../littlefs)
- FatFS on SPI NAND (registry component): [`espressif/spi_nand_flash_fatfs`](https://components.espressif.com/components/espressif/spi_nand_flash_fatfs)
