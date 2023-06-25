| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# USB Mass Storage Class example

## Overview

This example demonstrates usage of Mass Storage Class to get access to storage on USB memory stick.
Upon connection of USB stick, storage is mounted to Virtual filesystem. Example then creates `ESP` subdirectory(if not present already), as well as `text.txt` file. Its content is then repetitively printed to monitor until USB stick is manually ejected. User can decide whether or not to deinitialize the whole 
USB stack or not by shorting GPIO10 to ground. When GPIO10 is left unconnected USB stack is not deinitialized, USB stick can be plugged-in again. 

### Hardware Required

* Development board with USB capable ESP SoC (ESP32-S2/ESP32-S3)
* A USB cable for Power supply and programming
* A USB memory stick

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

Additionally, GPIO10 can be shorted to ground in order to deinitialize USB stack after ejecting USB stick. 

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
I (274) cpu_start: Starting scheduler on PRO CPU.
I (339) APP: Waiting for USB stick to be connected
Device info:
         PID: 0x5678 
         VID: 0xFFFF 
         iProduct: Disk 2.0 
         iManufacturer: USB 
         iSerialNumber: 92072836B2589224378 
I (719) APP: Writing file
I (749) APP: Reading file
I (749) APP: Read from file: 'Hello World!'
I (759) APP: Done
```
