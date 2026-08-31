| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# FATFS getting started (minimal) example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Purpose

Minimal FAT filesystem example for beginners: mount a FAT volume on internal SPI flash, write one file, read it back, then unmount.

Wear levelling is enabled by the mount API used here (`esp_vfs_fat_spiflash_mount_rw_wl`), which is required for safe read-write FAT on SPI flash.

## When to use this example

- You want the shortest path to store files with FATFS on internal SPI flash.
- You are learning the basic mount / write / read / unmount flow.

## What this example demonstrates

- Mount FAT on the `storage` partition with `esp_vfs_fat_spiflash_mount_rw_wl`
- Create and write a file with `fopen` / `fprintf`
- Read the file back with `fopen` / `fgets`
- Unmount with `esp_vfs_fat_spiflash_unmount_rw_wl`

Source: [main/fatfs_getting_started_main.c](./main/fatfs_getting_started_main.c)

## Requirements

- Internal SPI flash large enough for the example partition table (typically at least 4 MB modules)
- Partition label: `storage` (see [partitions_example.csv](./partitions_example.csv))

## How to use example

### Build and flash

```
idf.py -p PORT flash monitor
```

(Replace `PORT` with the serial port name. Exit the serial monitor with ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

```
...
I (321) example: Mounting FAT filesystem
I (331) example: Filesystem mounted
I (331) example: Opening file
I (731) example: File written
I (731) example: Reading file
I (741) example: Read from file: 'Hello World!'
I (741) example: Unmounting FAT filesystem
I (851) example: Done
...
```
