| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# USB MIDI Class Example


This example provides basic USB Host Midi functionality by implementing a midi class driver and a Host Library task. The example does the following:

1. Install Host Library and register a client
2. Waits for a device connection
3. Prints the device's information (such as device/configuration/string descriptors)
4. Claims Interface with more that zero endpoints from device
5. Prints received bytes, if midi-device sent new data

The example heavily follows the usb_host_lib example (https://github.com/espressif/esp-idf/tree/master/examples/peripherals/usb/host/usb_host_lib).

## How to use example

### Hardware Required

- Development board with USB capable ESP SoC (ESP32-S2/ESP32-S3)
- USB cable for Power supply and programming
- USB OTG Cable
- MIDI-device (e.g. KORG NANOKONTROL2)

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (314) app_start: Starting scheduler on CPU0
I (319) app_start: Starting scheduler on CPU1
I (319) main_task: Started on CPU0
I (329) main_task: Calling app_main()
I (329) DAEMON: Installing USB Host Library
I (369) MIDI DRIVER: Registering Client
I (2109) MIDI DRIVER: Opening device at address 1
I (2109) MIDI DRIVER: Getting device information
I (2109) MIDI DRIVER:   Full speed
I (2109) MIDI DRIVER:   bConfigurationValue 1
I (2119) MIDI DRIVER: Getting device descriptor
*** Device descriptor ***
bLength 18
bDescriptorType 1
bcdUSB 1.10
bDeviceClass 0x0
bDeviceSubClass 0x0
bDeviceProtocol 0x0
bMaxPacketSize0 8
idVendor 0xfc02
idProduct 0x101
bcdDevice 2.50
iManufacturer 0
iProduct 2
iSerialNumber 0
bNumConfigurations 1
I (2139) MIDI DRIVER: Getting config descriptor
*** Configuration descriptor ***
bLength 9
bDescriptorType 2
wTotalLength 101
bNumInterfaces 2
bConfigurationValue 1
iConfiguration 0
bmAttributes 0x80
bMaxPower 100mA
        *** Interface descriptor ***
        bLength 9
        bDescriptorType 4
        bInterfaceNumber 0
        bAlternateSetting 0
        bNumEndpoints 0
        bInterfaceClass 0x1
        bInterfaceSubClass 0x1
        bInterfaceProtocol 0x0
        iInterface 0
        *** Interface descriptor ***
        bLength 9
        bDescriptorType 4
        bInterfaceNumber 1
        bAlternateSetting 0
        bNumEndpoints 2
        bInterfaceClass 0x1
        bInterfaceSubClass 0x3
        bInterfaceProtocol 0x0
        iInterface 0
                *** Endpoint descriptor ***
                bLength 9
                bDescriptorType 5
                bEndpointAddress 0x81   EP 1 IN
                bmAttributes 0x2        BULK
                wMaxPacketSize 8
                bInterval 0
                *** Endpoint descriptor ***
                bLength 9
                bDescriptorType 5
                bEndpointAddress 0x2    EP 2 OUT
                bmAttributes 0x2        BULK
                wMaxPacketSize 8
                bInterval 0
I (2229) MIDI DRIVER: Getting interface config
Interface Number: 1, Alternate Setting: 0 
endpoint address: 129 , mps: 8
I (2239) MIDI DRIVER: Getting Product string descriptor
USB MIDI Interface
I (2249) MIDI DRIVER: Claiming Interface
I (2249) MIDI DRIVER: Configuring usb-transfer object
set transfer parameters
I (4429) MIDI DRIVER: Received new data: 

15 248 0 0 
I (4429) MIDI DRIVER: Received new data: 

8 140 0 0 
I (6159) MIDI DRIVER: Received new data: 

11 176 14 60 
I (6199) MIDI DRIVER: Received new data: 

11 176 32 15 
I (6229) MIDI DRIVER: Received new data: 

11 176 2 32 
```
