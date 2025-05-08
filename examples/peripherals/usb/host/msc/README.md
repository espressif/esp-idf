| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# USB Mass Storage Class example

## Overview

This example demonstrates usage of the MSC (Mass Storage Class) to access storage on a USB flash drive. Upon connection of the flash drive, it is mounted to the Virtual filesystem. The following example operations are then performed:

1. Print device info (capacity, sectors size, and count...).
2. List all folders and files in the root directory of the USB flash drive.
3. Create `esp` subdirectory (if not present already), as well as a `test.txt` file.
4. Run read/write benchmarks by transferring 1 MB of data to a `dummy` file.

> Note: This example currently supports only FAT-formatted drives. Other file systems, such as exFAT or NTFS, are not compatible with this example. Please ensure that your USB drive is formatted as FAT to avoid compatibility issues.

### USB Reconnections

The example is run in a loop so that it can demonstrate USB connection and reconnection handling. If you want to deinitialize the entire USB Host Stack, you can short GPIO0 to GND. GPIO0 is usually mapped to a BOOT button, thus pressing the button will deinitialize the stack.

### Hardware Required

* Development board with USB-OTG support
* A USB cable for power supply and programming
* A USB flash drive

### USB Host Limitations

#### ESP32-S2 & ESP32-S3
The USB OTG peripheral on ESP32-S2 and ESP32-S3 in host mode supports **up to 8 bidirectional endpoints**. Each of these endpoints can be configured as either IN or OUT.  

Since each USB Mass Storage Class (MSC) device typically requires **3 endpoints** (Control, BULK IN, and BULK OUT), and USB hubs also consume endpoints (Control, INTERRUPT IN), this limits the theoretical maximum number of connected MSC devices to **2**.

#### ESP32-P4
ESP32-P4 features a more advanced USB 2.0 OTG controller with **up to 16 bidirectional endpoints** in host mode. Given the same endpoint requirements per device, the theoretical maximum number of connected MSC devices is **4**.

### Example Limitations
The number of simultaneously connected MSC devices is now **determined by `CONFIG_FATFS_VOLUME_COUNT`**. This ensures consistency with the FAT filesystem configuration and prevents misalignment between the MSC device count and the available FAT volumes.

By default, `CONFIG_FATFS_VOLUME_COUNT` is set to **2**, meaning that up to **2 MSC devices** can be connected at the same time. If your application requires more devices, you can increase `CONFIG_FATFS_VOLUME_COUNT` accordingly, while keeping in mind **endpoint and memory constraints**.

#### How to configure `CONFIG_FATFS_VOLUME_COUNT` in menuconfig:
To change the maximum number of MSC devices, adjust `CONFIG_FATFS_VOLUME_COUNT` in **menuconfig**:
idf.py menuconfig → Component config → FAT Filesystem support → Number of FATFS volumes
Set this value to the desired number of MSC devices, ensuring it aligns with USB endpoint availability.

### Common Pin Assignments

Follow instructions in [examples/usb/README.md](../../README.md) for specific hardware setup.

Additionally, GPIO0 can be shorted to ground in order to deinitialize the USB stack.

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
I (323) example: Waiting for USB flash drive to be connected
I (3353) example: MSC device connected (usb_addr=3)
*** Device descriptor ***
bLength 18
bDescriptorType 1
bcdUSB 2.00
bDeviceClass 0x0
bDeviceSubClass 0x0
bDeviceProtocol 0x0
bMaxPacketSize0 64
idVendor 0xabcd
idProduct 0x1234
bcdDevice 1.00
iManufacturer 1
iProduct 2
iSerialNumber 3
bNumConfigurations 1
*** Configuration descriptor ***
bLength 9
bDescriptorType 2
wTotalLength 32
bNumInterfaces 1
bConfigurationValue 1
iConfiguration 0
bmAttributes 0x80
bMaxPower 100mA
        *** Interface descriptor ***
        bLength 9
        bDescriptorType 4
        bInterfaceNumber 0
        bAlternateSetting 0
        bNumEndpoints 2
        bInterfaceClass 0x8
        bInterfaceSubClass 0x6
        bInterfaceProtocol 0x50
        iInterface 0
                *** Endpoint descriptor ***
                bLength 7
                bDescriptorType 5
                bEndpointAddress 0x1    EP 1 OUT
                bmAttributes 0x2        BULK
                wMaxPacketSize 512
                bInterval 0
                *** Endpoint descriptor ***
                bLength 7
                bDescriptorType 5
                bEndpointAddress 0x81   EP 1 IN
                bmAttributes 0x2        BULK
                wMaxPacketSize 512
                bInterval 0
Device info:
         Capacity: 3839 MB
         Sector size: 512
         Sector count: 7864319
         PID: 0x1234
         VID: 0xABCD
         iProduct: UDisk
         iManufacturer: General
         iSerialNumber:
I (3763) example: ls command output for all connected devices:
I (3763) example: Listing contents of /usb0
/usb0/SYSTEM~1
/usb0/ESP
I (3773) example: Reading file
I (3773) example: Read from file '/usb0/esp/test.txt': 'Hello World!'
I (3803) example: Writing to file /usb0/esp/dummy
I (3943) example: Write speed 7.16 MiB/s
I (3953) example: Reading from file /usb0/esp/dummy
I (4093) example: Read speed 7.16 MiB/s
I (4103) example: Example finished, you can disconnect the USB flash drive (or connect another USB flash drive)
```
