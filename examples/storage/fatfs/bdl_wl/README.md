| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# FatFS over BDL (Block Device Layer) - Wear-Levelling Stack

This example demonstrates mounting a FAT filesystem using the Block Device Layer (BDL) interface
instead of the legacy `wl_handle_t`-based API.

## BDL Stack

The BDL stack constructed in this example:

```
  FatFS (VFS + POSIX API)
       |
  diskio_bdl    (generic BDL diskio adapter)
       |
  WL BDL        (wear-levelling, via wl_get_blockdev())
       |
  Partition BDL (flash partition, via esp_partition_get_blockdev())
       |
  SPI Flash     (physical storage)
```

The key advantage of BDL is that **the same `diskio_bdl` adapter works with any BDL device**.
You can swap the bottom of the stack (e.g., use `sdmmc_get_blockdev()` for an SD card) without
changing the FatFS integration code.

## How to use example

### Build and flash

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

## Example output

```
I (321) example: Creating partition BDL for 'storage' partition
I (331) example:   Partition BDL: disk_size=1048576, erase_size=4096
I (331) example: Creating WL BDL on top of partition BDL
I (341) example:   WL BDL: disk_size=...., erase_size=4096
I (341) example: Mounting FAT filesystem via BDL
I (741) example: Filesystem mounted
I (741) example: Opening file
I (841) example: File written
I (841) example: Reading file
I (841) example: Read from file: 'Hello from FatFS over BDL!'
I (841) example: Unmounting FAT filesystem
I (941) example: Releasing BDL devices
I (941) example: Done
```
