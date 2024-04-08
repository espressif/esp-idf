| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Wear levelling example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use wear levelling library and FATFS library to store files in a partition inside SPI flash. Example does the following steps:

1. Use an "all-in-one" `esp_vfs_fat_spiflash_mount_rw_wl` function to:
    - find a partition in SPI flash,
    - initialize wear levelling library using this partition
    - mount FAT filesystem using FATFS library (and format the filesystem, if the filesystem can not be mounted),
    - register FAT filesystem in VFS, enabling C standard library and POSIX functions to be used.
2. Create a file using `fopen` and write to it using `fprintf`.
3. Open file for reading, read back the line, and print it to the terminal.

Wear levelling partition size is set in partitions_example.csv file. See [Partition Tables](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/partition-tables.html) documentation for more information.

## How to use example

### Hardware required

This example does not require any special hardware, and can be run on any common development board.

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with serial port name.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

Here is a typical example console output.

```
I (280) example: Mounting FAT filesystem
W (440) vfs_fat_spiflash: f_mount failed (13)
I (440) vfs_fat_spiflash: Formatting FATFS partition, allocation unit size=4096
I (660) vfs_fat_spiflash: Mounting again
I (660) example: Opening file
I (910) example: File written
I (910) example: Reading file
I (920) example: Read from file: 'written using ESP-IDF v3.1-dev-171-gf9ad17eee-dirty'
I (920) example: Unmounting FAT filesystem
I (1000) example: Done
```

To erase the contents of wear levelling partition, run `idf.py erase-flash` command. Then upload the example again as described above.
