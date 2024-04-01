| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# About LittleFS

[LittleFS](https://github.com/littlefs-project/littlefs) is a file system designed for small devices like microcontrollers and embedded systems. It helps these devices manage and store data efficiently.
- LittleFS is specifically built to be lightweight and requires less storage space and processing power. It takes up very little memory, which is important for small devices with limited resources.
- It has features that ensure data integrity and reliability, so the files stored on the device are less likely to get corrupted or lost.
In a nutshell, LittleFS is a specialized file system that helps small devices effectively handle and store data, allowing them to perform tasks efficiently with minimal resources.

# License

The littlefs is provided under the BSD-3-Clause license. Please refer [littlefs license](https://github.com/littlefs-project/littlefs#license) for more details.


# LittleFS example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

`littlefs` component is managed in ESP-IDF using IDF component manager. This example uses a littlefs port for [ESP-IDF littlefs](https://components.espressif.com/components/joltwallet/littlefs) for which upstream repository is [esp_littlefs](https://github.com/joltwallet/esp_littlefs).

LittleFS partition generator is invoked from CMake build system. We use `littlefs_create_partition_image` in main/CMakeLists.txt to generate the partition image and flash it to the device at build time.

*Note: Currently Windows does not support `LittleFS partition generation` feature.*

This example demonstrates how to use LittleFS with ESP32. Example does the following steps:

1. Use an "all-in-one" `esp_vfs_littlefs_register` function to:
    - initialize LittleFS,
    - mount LittleFS filesystem using LittleFS library (and format, if the filesystem can not be mounted),
    - register LittleFS filesystem in VFS, enabling C standard library and POSIX functions to be used.
2. Create a file using `fopen` and write to it using `fprintf`.
3. Rename the file. Before renaming, check if destination file already exists using `stat` function, and remove it using `unlink` function.
4. Open renamed file for reading, read back the line, and print it to the terminal.

LittleFS partition size is set in partitions_demo_esp_littlefs.csv file. See [Partition Tables](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/partition-tables.html) documentation for more information.

## How to use example

### Hardware required

This example does not require any special hardware, and can be run on any common development board.

### Build and flash

Replace PORT with serial port name:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

Here is an example console output. In this case `format_if_mount_failed` parameter was set to `true` in the source code. LittleFS was unformatted, so the initial mount has failed. LittleFS was then formatted, and mounted again.

```
I (264) esp_littlefs: Initializing LittleFS
I (274) esp_littlefs: Partition size: total: 983040, used: 12288
I (274) esp_littlefs: Opening file
I (344) esp_littlefs: File written
I (344) esp_littlefs: Renaming file
I (354) esp_littlefs: Reading file
I (354) esp_littlefs: Read from file: 'Hello World!'
I (354) esp_littlefs: Reading from flashed filesystem example.txt
I (364) esp_littlefs: Read from file: 'Example text to compile into a LittleFS disk image to be flashed to the ESP device.'
I (374) esp_littlefs: LittleFS unmounted
I (374) main_task: Returned from app_main()
```

To erase the contents of LittleFS partition, run `idf.py erase-flash` command. Then upload the example again as described above.

[Here](https://github.com/joltwallet/esp_littlefs#performance) you can find performance results for LittleFS, SPIFFS and FATFS for your reference.
