# Storage Examples

Storage and management of user and system data in module’s flash and on external memory / devices.
This directory contains a range of examples ESP-IDF projects. These are intended to demonstrate the storage features, and to provide code that you can copy and adapt into your own projects.

# Example Layout

The examples are grouped into sub-directories by category. Each category directory contains one or more example projects:

* `fatfs/getting_started` — minimal FATFS on internal SPI flash (mount / write / read / unmount). Wear levelling is used via `esp_vfs_fat_spiflash_mount_rw_wl`.
* `fatfs/fs_operations` — broader FATFS file and directory operations.
* `fatfs/ext_flash` — FATFS on an external SPI flash chip.
* `fatfs/fatfsgen` — build-time FATFS image generation.
* `fatfs/bdl_wl` — FATFS with wear-levelling block device layer (BDL).
* `wear_levelling` — wear levelling + FATFS lifecycle (mount, write/read, format, verify, rewrite). Not a duplicate of `fatfs/getting_started`; use that for the minimal first demo.
* `littlefs` — LittleFS on flash (wear management is built into LittleFS; not the wear_levelling component).
* `custom_flash_driver` — implement a custom flash chip driver by overriding the default driver.
* `emmc` — use an eMMC chip with an ESP device.
* `nvs/nvs_bootloader` — read NVS data from bootloader code.
* `nvs/nvs_rw_blob` — read/write an integer and a blob in NVS across restarts.
* `nvs/nvs_rw_value` — read/write a single integer in NVS.
* `nvs/nvs_rw_value_cxx` — same as `nvs/nvs_rw_value` using the C++ NVS handle API.
* `nvs/nvs_console` — interactive NVS console.
* `nvs/nvs_statistics` — NVS used/available entry statistics.
* `nvs/nvs_iteration` — iterate NVS entries by type/namespace.
* `nvs/nvsgen` — NVS partition / image generation helpers.
* `partition_api` — partition API examples.
* `parttool` — common host-side partition tool operations.
* `sd_card` — SD card examples.
* `semihost_vfs` — semihosting VFS driver.
* `spiffs` — SPIFFS on flash.
* `spiffsgen` — build-time SPIFFS image generation with spiffsgen.py.
* `perf_benchmark` — storage performance benchmarking.

# More

See the [README.md](../README.md) file in the upper level [examples](../) directory for more information about examples.
