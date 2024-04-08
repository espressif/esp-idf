| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# SPIFFS example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use SPIFFS with ESP32. Example does the following steps:

1. Use an "all-in-one" `esp_vfs_spiffs_register` function to:
    - initialize SPIFFS,
    - mount SPIFFS filesystem using SPIFFS library (and format, if the filesystem can not be mounted),
    - register SPIFFS filesystem in VFS, enabling C standard library and POSIX functions to be used.
2. Create a file using `fopen` and write to it using `fprintf`.
3. Rename the file. Before renaming, check if destination file already exists using `stat` function, and remove it using `unlink` function.
4. Open renamed file for reading, read back the line, and print it to the terminal.

SPIFFS partition size is set in partitions_example.csv file. See [Partition Tables](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/partition-tables.html) documentation for more information.

## How to use example

### Hardware required

This example does not require any special hardware, and can be run on any common development board.

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)
* Configure SPIFFS settings under "SPIFFS Example menu". See note about `esp_spiffs_check` function on [SPIFFS Filesystem](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/spiffs.html) page.

### Build and flash

Replace PORT with serial port name:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

Here is an example console output. In this case `format_if_mount_failed` parameter was set to `true` in the source code. SPIFFS was unformatted, so the initial mount has failed. SPIFFS was then formatted, and mounted again.

```
I (324) example: Initializing SPIFFS
W (324) SPIFFS: mount failed, -10025. formatting...
I (19414) example: Partition size: total: 896321, used: 0
I (19414) example: Opening file
I (19504) example: File written
I (19544) example: Renaming file
I (19584) example: Reading file
I (19584) example: Read from file: 'Hello World!'
I (19584) example: SPIFFS unmounted
```

To erase the contents of SPIFFS partition, run `idf.py erase-flash` command. Then upload the example again as described above.
