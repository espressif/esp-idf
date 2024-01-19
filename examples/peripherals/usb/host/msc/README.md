| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# USB Mass Storage Class example

## Overview

This example demonstrates usage of the MSC (Mass Storage Class) to access storage on a USB flash drive. Upon connection of the flash drive, it is mounted to the Virtual filesystem. The following example operations are then performed:

1. Print device info (capacity, sectors size, and count...)
2. List all folders and files in the root directory of the USB flash drive
3. Create `ESP` subdirectory (if not present already), as well as a `text.txt` file
4. Run read/write benchmarks by transferring 1 MB of data to a `dummy` file


### USB Reconnections

The example is run in a loop so that it can demonstrate USB connection and reconnection handling. If you want to deinitialize the entire  USB Host Stack, you can short GPIO0 to GND. GPIO0 is usually mapped to a BOOT button, thus pressing the button will deinitialize the stack.


### Hardware Required

* Development board with USB capable ESP SoC (ESP32-S2/ESP32-S3)
* A USB cable for Power supply and programming
* A USB flash drive

### Common Pin Assignments

If your board doesn't have a USB A connector connected to the dedicated GPIOs, 
you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

```
ESP BOARD    USB CONNECTOR (type A)
                   --
                  | || VCC
[GPIO19]  ------> | || D-
[GPIO20]  ------> | || D+
                  | || GND
                   --
```

Additionally, GPIO0 can be shorted to ground in order to deinitialize USB stack. 

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
...
I (380) example: Waiting for USB flash drive to be connected
I (790) example: MSC device connected
...
Device info:
         Capacity: 29339 MB
         Sector size: 512
         Sector count: 60088319
         PID: 0x5595
         VID: 0x0781
         iProduct:  SanDisk 3.2Gen1
         iManufacturer:  USB
         iSerialNumber: 0401545df64623a907abf299bae54c9
I (990) example: ls command output:
SYSTEM~1
ESP
DUMMY
I (1000) example: Reading file
I (1010) example: Read from file '/usb/esp/test.txt': 'Hello World!'
I (1030) example: Writing to file /usb/esp/dummy
I (2160) example: Write speed 0.93 MiB/s
I (2160) example: Reading from file /usb/esp/dummy
I (3110) example: Read speed 1.10 MiB/s
I (3140) example: Example finished, you can disconnect the USB flash drive
```
