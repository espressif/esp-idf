| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# USB CDC-ACM Host Driver Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use the CDC-ACM Host Driver to allow an ESP chip to communicate with a USB CDC-ACM device.

## How to use example

### Hardware Required

Two ESP boards that have USB-OTG supported. One will act as USB host and the other as USB device.  
Connect USB_D+, USB_D-, GND and +5V signals of USB host to USB device.

#### Pin Assignment

See common pin assignments for USB Device examples from [upper level](../../../README.md#common-pin-assignments).

### Build and Flash

1. Build and flash [tusb_serial_device example](../../../device/tusb_serial_device) to USB device board.
2. Build this project and flash it to the USB host board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

After the flashing you should see the output at idf monitor:

```
...
I (256) USB-CDC: USB Host installed
I (256) USB-CDC: Opening CDC ACM device 0x303A:0x4001
...
Device descriptor is printed here
...
I (1666) USB-CDC: Data received
I (1666) USB-CDC: 0x3ffc4c20   41 54 0d                                          |AT.|
I (2666) USB-CDC: Data received
I (2666) USB-CDC: 0x3ffc4c20   41 54 2b 47 53 4e 0d                              |AT+GSN.|
I (3666) USB-CDC: Setting up line coding
I (3666) USB-CDC: Line Get: Rate: 115200, Stop bits: 0, Parity: 0, Databits: 8
I (3666) USB-CDC: Line Set: Rate: 9600, Stop bits: 1, Parity: 1, Databits: 7
I (3666) USB-CDC: Line Get: Rate: 9600, Stop bits: 1, Parity: 1, Databits: 7
I (3676) Example finished successfully!
...

```
