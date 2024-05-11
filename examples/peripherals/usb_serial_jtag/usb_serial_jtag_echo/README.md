| Supported Targets | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- |

# USB SERIAL JTAG Echo Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to utilize USB_SERIAL_JTAG interfaces by echoing back to the sender any data received on USB_SERIAL_JTAG.

## How to use example

### Hardware Required

The example can be run on development board that supports usb_serial_jtag, that is based on the Espressif SoC. The board shall be connected to a computer with a single USB cable for flashing and monitoring with UART port. The usb_serial_jtag port on board can be be connected to computer with another USB cable to get the echo.

### Configure the project

Use the command below to configure project using Kconfig menu as showed in the table above.
The default Kconfig values can be changed such as disable the `ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG`
```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Type some characters in the terminal connected to the external serial interface. As result you should see echo in the same terminal which you used for typing the characters.

For example, If I type `hi espressif`, `See you again!`, `Echo a very long buffer. Assume this buffer is very large and you can see whole buffer`

And you can get the log with:

```
I (296) main_task: Started on CPU0
I (296) main_task: Calling app_main()
I (296) main_task: Returned from app_main()
I (13346) Recv str: : 0x408117b8   68 69 20 65 73 70 72 65  73 73 69 66 0d 0a        |hi espressif..|
I (16606) Recv str: : 0x408117b8   53 65 65 20 79 6f 75 20  61 67 61 69 6e 21 0d 0a  |See you again!..|
I (18726) Recv str: : 0x408117b8   45 63 68 6f 20 61 20 76  65 72 79 20 6c 6f 6e 67  |Echo a very long|
I (18726) Recv str: : 0x408117c8   20 62 75 66 66 65 72 2e  20 41 73 73 75 6d 65 20  | buffer. Assume |
I (18726) Recv str: : 0x408117d8   74 68 69 73 20 62 75 66  66 65 72 20 69 73 20 76  |this buffer is v|
I (18736) Recv str: : 0x408117e8   65 72 79 20 6c 61 72 67  65 20 61 6e 64 20 79 6f  |ery large and yo|
I (18746) Recv str: : 0x408117b8   75 20 63 61 6e 20 73 65  65 20 77 68 6f 6c 65 20  |u can see whole |
I (18756) Recv str: : 0x408117c8   62 75 66 66 65 72 0d 0a                           |buffer..|
```

## Troubleshooting

Note that you are not supposed to see the echo in the terminal if usb_serial_jtag port is used for flashing and monitoring.
