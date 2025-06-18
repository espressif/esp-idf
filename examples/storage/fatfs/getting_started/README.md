| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# FATFS minimal example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates the minimal setup required to store persistent data on SPI Flash using the FAT filesystem.
Beware that the minimal required size of the flash is 4 MB.

## How to use example

### Build and flash

To run the example, type the following command:

```CMake
# CMake
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

Here is the example's console output:

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

The logic of the example is contained in a [single source file](./main/fatfs_getting_started_main.c),
and it should be relatively simple to match points in its execution with the log outputs above.
