| Supported Targets | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- |

# USB CDC-ACM Host Driver Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates how to use the [CDC-ACM Host Driver](https://components.espressif.com/components/espressif/usb_host_cdc_acm) to enable an ESP chip to communicate with a USB CDC-ACM (Communication Device Class - Abstract Control Model) device. CDC-ACM is a USB device class specification that allows USB devices to appear as serial ports, commonly used by USB-to-UART converters and virtual COM port devices.

The example performs the following operations:

1. Installs the USB Host Library and CDC-ACM driver
2. Waits for a CDC-ACM device to be connected
3. Opens the first available CDC-ACM device found
4. Prints the device descriptor information
5. Sends test data to the device
6. Receives data from the device (handled via callback)
7. Demonstrates line coding commands (get/set baud rate, data bits, stop bits, parity)
8. Demonstrates control line state commands (DTR/RTS)
9. Waits for device disconnection and repeats the process

## How to use example

### Hardware Required

* Development board with USB-OTG support that will act as USB host
* USB CDC-ACM device. This can be:
  - A USB-to-UART converter (e.g., CP210x, FTDI FT23x, CH34x)
  - Another ESP development board configured as a USB serial device (see [tusb_serial_device example](../../../device/tusb_serial_device))
  - Another ESP development board connected with USB-Serial-JTAG
  - Any other USB device that implements the CDC-ACM class

#### Pin Assignment

Follow instructions in [examples/usb/README.md](../../../README.md) for specific hardware setup.

### Build and Flash

1. Prepare the USB CDC device:
   - If using a USB-to-UART converter, no preparation is needed
   - If using another ESP board as a USB device, build and flash the [tusb_serial_device example](../../../device/tusb_serial_device) to that board first

2. Connect the CDC device to the USB Host port

3. Build this project and flash it to the USB host board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

After flashing and connecting a CDC-ACM device, you should see output similar to the following in the idf monitor:

```
I (256) USB-CDC: Installing USB Host
I (256) USB-CDC: Installing CDC-ACM driver
I (256) USB-CDC: Opening CDC ACM device...

...
I (1174) USB-CDC: Data received
I (1174) USB-CDC: 0x4ffbf640   43 44 43 20 74 65 73 74  20 73 74 72 69 6e 67 21  |CDC test string!|
I (1274) USB-CDC: Testing control line state command
I (1294) USB-CDC:       - Control line state set to DTR=false, RTS=true
I (1314) USB-CDC: Testing line coding commands
I (1314) USB-CDC:       - Line Coding Get: Rate: 115200, Stop bits: 0, Parity: 0, Databits: 8
I (1314) USB-CDC:       - Line Set: Rate: 115200, Stop bits: 0, Parity: 0, Databits: 8
I (1314) USB-CDC: Example finished successfully! You can reconnect the device to run again.
```

## Troubleshooting

### Device Not Detected

If the CDC-ACM device is not detected, check:

1. **USB connection:** Ensure the device is properly connected to the USB host port
2. **Device compatibility:** Verify that the device implements the CDC-ACM class. Some USB-to-UART converters may use proprietary drivers
3. **Power supply:** Ensure the USB host port provides adequate power for the connected device
4. **Cable quality:** Use a quality USB cable that supports data transfer (not charge-only cables)

### No Data Received

If data is sent but not received:

1. **Check device connection:** Ensure the device is still connected and enumerated
2. **Verify device functionality:** Test the device with another host to confirm it's working
3. **Check line coding:** Some devices may require specific line coding settings before they start transmitting data
4. **Increase log level:** Set the log level to debug via `idf.py menuconfig` to get more detailed information

### Line Coding Commands Not Supported

If you see warnings like:
```
W (xxx) USB-CDC: Line coding get not supported
W (xxx) USB-CDC: Control line state set not supported
```

This is normal for some CDC-ACM devices that don't support these optional commands. The example will continue to work for basic data transfer operations.
