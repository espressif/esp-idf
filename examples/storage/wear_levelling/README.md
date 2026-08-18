| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Wear levelling + FATFS example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Purpose

Demonstrate the wear levelling library together with FATFS on an internal SPI flash partition: mount, write/read files, format the FAT volume through the wear-levelling path, confirm files are gone, then write again and unmount.

## When to use this example

- You need the wear levelling + FATFS lifecycle, especially the format step (`esp_vfs_fat_spiflash_format_rw_wl`).
- You want to see mount → write/read → format → verify → rewrite → unmount in one flow.

Use a different example when you need:

- the shortest first FATFS demo → [fatfs/getting_started](../fatfs/getting_started/)
- FATFS on an external SPI flash chip → [fatfs/ext_flash](../fatfs/ext_flash/)
- broader FATFS file operations → [fatfs/fs_operations](../fatfs/fs_operations/)

### Wear levelling vs other storage

- **FATFS on SPI flash (this example):** use the wear_levelling component via `esp_vfs_fat_spiflash_mount_rw_wl` so repeated writes do not wear out the same flash sectors.
- **LittleFS:** wear management is part of the filesystem design; see [littlefs](../littlefs/) (does not use this wear_levelling component).
- **NVS:** flash wear is handled inside NVS; see [nvs](../nvs/) for keys/values, not general files.

## What this example demonstrates

1. Mount with the all-in-one API `esp_vfs_fat_spiflash_mount_rw_wl`, which:
   - finds the `storage` partition in SPI flash
   - initializes the wear levelling library on that partition
   - mounts FATFS (and formats if mount fails when `format_if_mount_failed` is set)
   - registers the volume in VFS so POSIX / C library file APIs work
2. Write a file (`fopen` / `fprintf`) and read it back
3. Format the mounted volume with `esp_vfs_fat_spiflash_format_rw_wl`
4. Verify the previous file no longer exists (`stat`)
5. Write and read a new file after format
6. Unmount with `esp_vfs_fat_spiflash_unmount_rw_wl`

Source: [main/wear_levelling_example_main.c](./main/wear_levelling_example_main.c)

Partition size is set in [partitions_example.csv](./partitions_example.csv). See [Partition Tables](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/partition-tables.html).

## Requirements

- No special hardware; any common development board with enough internal flash for the partition table
- Partition label: `storage`

## How to use example

### Hardware required

This example does not require any special hardware, and can be run on any common development board.

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace `PORT` with the serial port name. Exit the serial monitor with ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

To wipe the wear-levelling / FAT contents and start clean, run `idf.py erase-flash`, then flash the example again.

## Example output

Typical console output (IDs and IDF version string vary):

```
I (280) example: Mounting FAT filesystem
W (440) vfs_fat_spiflash: f_mount failed (13)
I (440) vfs_fat_spiflash: Formatting FATFS partition, allocation unit size=4096
I (660) vfs_fat_spiflash: Mounting again
I (660) example: Opening file
I (910) example: File written
I (910) example: Reading file
I (920) example: Read from file: 'hello world, from ESP-IDF <version>'
I (920) example: file does not exist, format done
I (920) example: Opening file
I (950) example: File written
I (950) example: Reading file
I (960) example: Read from file: 'nihao shijie, from ESP-IDF <version>'
I (960) example: Unmounting FAT filesystem
I (1000) example: Done
```

The `f_mount failed` / formatting lines appear on a fresh (unformatted) partition; later runs may skip that path if the filesystem already exists.
