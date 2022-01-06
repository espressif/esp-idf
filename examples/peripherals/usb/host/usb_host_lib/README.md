| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# USB Host Library Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates the basic usage of the [USB Host Library API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/peripherals/usb_host.html) by implementing a pseudo class driver and a Host Library daemon task. The example does the following:

1. Install Host Library and register a client
2. Waits for a device connection
3. Prints the device's information (such as device/configuration/string descriptors)
4. Waits for the device to disconnect
5. Deregister the client and uninstall the Host Library

The example demonstrates the following aspects of the USB Host Library API:

- How to use the Library API to:
    - Install and uninstall the USB Host Library
    - Run the library event handler function a daemon task
    - How to handle library events
- How to use the Client API from a client task to:
    - Register and deregister a client of the USB Host Library
    - Run the client event handler functions
    - How to handle client events via various callbacks
    - Open and close a device
    - Get a device's descriptors

## How to use example

### Hardware Required

An ESP board that supports USB-OTG. The example uses the ESP's internal USB PHY, however the internal USB PHY's pins will need to be connected to a USB port (i.e., a USB breakout board) as follows:

- GND and 5V signals of the ESP board to the GND and 5V lines of the USB port
- GPIO 19 to D-
- GPIO 20 to D+

### Configure the project

```
idf.py menuconfig
```

* The USB Host Stack has a maximum supported transfer size for control transfer during device enumeration. This size is specified via the USB_HOST_CONTROL_TRANSFER_MAX_SIZE configuration option and has a default value of 256 bytes. Therefore, if devices with length config/string descriptors are used, users may want to increase the size of this configuration.

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
I (261) cpu_start: Starting scheduler on PRO CPU.
I (267) DAEMON: Installing USB Host Library
I (297) CLASS: Registering Client
I (5067) CLASS: Opening device at address 1
I (5067) CLASS: Getting device information
I (5067) CLASS: 	Full speed
I (5067) CLASS: 	bConfigurationValue 1
I (5067) CLASS: Getting device descriptor
*** Device descriptor ***
bLength 18
bDescriptorType 1
bcdUSB 2.00
bDeviceClass 0xef
bDeviceSubClass 0x2
bDeviceProtocol 0x1
bMaxPacketSize0 64
idVendor 0x303a
idProduct 0x1001
bcdDevice 1.00
iManufacturer 1
iProduct 2
iSerialNumber 3
bNumConfigurations 1
I (5097) CLASS: Getting config descriptor
*** Configuration descriptor ***
bLength 9
bDescriptorType 2
wTotalLength 98
bNumInterfaces 3
bConfigurationValue 1
iConfiguration 0
bmAttributes 0xc0
bMaxPower 500mA
	*** Interface descriptor ***
	bLength 9
	bDescriptorType 4
	bInterfaceNumber 0
	bAlternateSetting 0
	bNumEndpoints 1
	bInterfaceClass 0x0
	iInterface 0
		*** Endpoint descriptor ***
		bLength 7
		bDescriptorType 5
		bEndpointAddress 0x82	EP 2 IN
		bmAttributes 0x3	INT
		wMaxPacketSize 64
		bInterval 1
	*** Interface descriptor ***
	bLength 9
	bDescriptorType 4
	bInterfaceNumber 1
	bAlternateSetting 0
	bNumEndpoints 2
	bInterfaceClass 0x0
	iInterface 0
		*** Endpoint descriptor ***
		bLength 7
		bDescriptorType 5
		bEndpointAddress 0x1	EP 1 OUT
		bmAttributes 0x2	BULK
		wMaxPacketSize 64
		bInterval 1
		*** Endpoint descriptor ***
		bLength 7
		bDescriptorType 5
		bEndpointAddress 0x81	EP 1 IN
		bmAttributes 0x2	BULK
		wMaxPacketSize 64
		bInterval 1
	*** Interface descriptor ***
	bLength 9
	bDescriptorType 4
	bInterfaceNumber 2
	bAlternateSetting 0
	bNumEndpoints 2
	bInterfaceClass 0x1
	iInterface 0
		*** Endpoint descriptor ***
		bLength 7
		bDescriptorType 5
		bEndpointAddress 0x2	EP 2 OUT
		bmAttributes 0x2	BULK
		wMaxPacketSize 64
		bInterval 1
		*** Endpoint descriptor ***
		bLength 7
		bDescriptorType 5
		bEndpointAddress 0x83	EP 3 IN
		bmAttributes 0x2	BULK
		wMaxPacketSize 64
		bInterval 1
I (5227) CLASS: Getting Manufacturer string descriptor
Espressif
I (5237) CLASS: Getting Product string descriptor
USB JTAG/serial debug unit
I (5247) CLASS: Getting Serial Number string descriptor
7C:DF:A1:E0:10:50
```

## Troubleshooting

To obtain more debug, users should set the [log level](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/system/log.html) to debug via menuconfig.

### Failing Enumeration

```
I (262) cpu_start: Starting scheduler on PRO CPU.
I (268) DAEMON: Installing USB Host Library
I (298) CLASS: Registering Client
E (2748) HUB: Short string desc corrupt
E (2748) HUB: Stage failed: CHECK_SHORT_MANU_STR_DESC
```

The log output demonstrates a device that has failed. The Hub Driver will output some error logs indicating which stage of enumeration has failed.

### Blank String Descriptors

The current USB Host Library will automatically cache the Manufacturer, Product, and Serial Number string descriptors of the device during enumeration. However, when fetching the string descriptors, the USB Host Library will only fetch those strings descriptors of they of LANGID code 0x0409 (i.e., English - United States). Therefore, if the example does not print a particular descriptor, it is likely that the string descriptor was not cached during enumeration.