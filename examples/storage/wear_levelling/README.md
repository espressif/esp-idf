# Wear levelling example

This example demonstrates how to use wear levelling library and FATFS library to store files in a partition inside SPI flash. Example does the following steps:

1. Use an "all-in-one" `esp_vfs_fat_spiflash_mount` function to:
    - find a partition in SPI flash,
    - initialize wear levelling library using this partition
    - mount FAT filesystem using FATFS library (and format the filesystem, if the filesystem can not be mounted),
    - register FAT filesystem in VFS, enabling C standard library and POSIX functions to be used.
2. Create a file using `fopen` and write to it using `fprintf`.
3. Open file for reading, read back the line, and print it to the terminal.

## Example output

Here is an typical example console output. 

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
