| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Console Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example illustrates the usage of the [Console Component](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/console.html#console) to create an interactive shell on a ESP chip. The interactive shell running on the ESP chip can then be controlled/interacted with over a serial interface. This example supports UART and USB interfaces.

The interactive shell implemented in this example contains a wide variety of commands, and can act as a basis for applications that require a command-line interface (CLI).

## How to use example

This example can be used on boards with UART and USB interfaces. The sections below explain how to set up the board and configure the example.

### Using with UART

When UART interface is used, this example can run on any commonly available Espressif development board. UART interface is enabled by default (`CONFIG_ESP_CONSOLE_UART_DEFAULT` option in menuconfig). No extra configuration is required.

### Using with USB_SERIAL_JTAG

*NOTE: We recommend to disable the secondary console output on chips with USB_SERIAL_JTAG since the secondary serial is output-only and would not be very useful when using a console application. This is why the secondary console output is deactivated per default (CONFIG_ESP_CONSOLE_SECONDARY_NONE=y)*

On chips with USB_SERIAL_JTAG peripheral, console example can be used over the USB serial port.

* First, connect the USB cable to the USB_SERIAL_JTAG interface.
* Second, run `idf.py menuconfig` and enable `CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG` option.

For more details about connecting and configuring USB_SERIAL_JTAG (including pin numbers), see the IDF Programming Guide:
* [ESP32-C3 USB_SERIAL_JTAG](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-guides/usb-serial-jtag-console.html)
* [ESP32-C6 USB_SERIAL_JTAG](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/api-guides/usb-serial-jtag-console.html)
* [ESP32-S3 USB_SERIAL_JTAG](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-guides/usb-serial-jtag-console.html)
* [ESP32-H2 USB_SERIAL_JTAG](https://docs.espressif.com/projects/esp-idf/en/stable/esp32h2/api-guides/usb-serial-jtag-console.html)

### Using with USB CDC (USB_OTG peripheral)

USB_OTG peripheral can also provide a USB serial port which works with this example.

* First, connect the USB cable to the USB_OTG peripheral interface.
* Second, run `idf.py menuconfig` and enable `CONFIG_ESP_CONSOLE_USB_CDC` option.

For more details about connecting and configuring USB_OTG (including pin numbers), see the IDF Programming Guide:
* [ESP32-S2 USB_OTG](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s2/api-guides/usb-otg-console.html)
* [ESP32-S3 USB_OTG](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-guides/usb-otg-console.html)

### Other configuration options

This example has an option to store the command history in Flash. This option is enabled by default.

To disable this, run `idf.py menuconfig` and disable `CONFIG_CONSOLE_STORE_HISTORY` option.

### Configure the project

```
idf.py menuconfig
```

* Enable/Disable storing command history in flash and load the history in a next example run. Linenoise line editing library provides functions to save and load
  command history. If this option is enabled, initializes a FAT filesystem and uses it to store command history.
  * `Example Configuration > Store command history in flash`

* Accept/Ignore empty lines inserted into the console. If an empty line is inserted to the console, the Console can either ignore empty lines (the example would continue), or break on emplty lines (the example would stop after an empty line).
  * `Example Configuration > Ignore empty lines inserted into the console`

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Enter the `help` command get a full list of all available commands. The following is a sample session of the Console Example where a variety of commands provided by the Console Example are used. Note that GPIO15 is connected to GND to remove the boot log output.

```
This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
Ctrl+C will terminate the console environment.
esp32> help
help
help  [<string>]
  Print the summary of all registered commands if no arguments are given,
  otherwise print summary of given command.
      <string>  Name of command

free 
  Get the current size of free heap memory

heap 
  Get minimum size of free heap memory that was available during program
  execution

version 
  Get version of chip and SDK

restart 
  Software reset of the chip

...
...

esp32> free
257200
esp32> deep_sleep -t 1000
I (146929) cmd_system_sleep: Enabling timer wakeup, timeout=1000000us
I (619) heap_init: Initializing. RAM available for dynamic allocation:
I (620) heap_init: At 3FFAE2A0 len 00001D60 (7 KiB): DRAM
I (626) heap_init: At 3FFB7EA0 len 00028160 (160 KiB): DRAM
I (645) heap_init: At 3FFE0440 len 00003BC0 (14 KiB): D/IRAM
I (664) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (684) heap_init: At 40093EA8 len 0000C158 (48 KiB): IRAM

This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
esp32> join --timeout 10000 test_ap test_password
I (182639) connect: Connecting to 'test_ap'
I (184619) connect: Connected
esp32> free
212328
esp32> restart
I (205639) cmd_system_common: Restarting
I (616) heap_init: Initializing. RAM available for dynamic allocation:
I (617) heap_init: At 3FFAE2A0 len 00001D60 (7 KiB): DRAM
I (623) heap_init: At 3FFB7EA0 len 00028160 (160 KiB): DRAM
I (642) heap_init: At 3FFE0440 len 00003BC0 (14 KiB): D/IRAM
I (661) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (681) heap_init: At 40093EA8 len 0000C158 (48 KiB): IRAM

This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
Ctrl+C will terminate the console environment.
esp32>

```

## Troubleshooting

### Line Endings

The line endings in the Console Example are configured to match particular serial monitors. Therefore, if the following log output appears, consider using a different serial monitor (e.g. Putty for Windows) or modify the example's [UART configuration](#Configuring-UART).

```
This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
Your terminal application does not support escape sequences.
Line editing and history features are disabled.
On Windows, try using Putty instead.
esp32>
```

### No USB port appears

On Windows 10, macOS, Linux, USB CDC devices do not require additional drivers to be installed.

If the USB serial port doesn't appear in the system after flashing the example, check the following:

* Check that the USB device is detected by the OS.
  VID/PID pair for ESP32-S2 is 303a:0002.

  - On Windows, check the Device Manager
  - On macOS, check USB section in the System Information utility
  - On Linux, check `lsusb` output

* If the device is not detected, check the USB cable connection (D+, D-, and ground should be connected)

## Example Breakdown

### Configuring UART

The ``initialize_console_library()`` function in the example configures some aspects of UART relevant to the operation of the console.

- **Line Endings**: The default line endings are configured to match those expected/generated by common serial monitor programs, such as `screen`, `minicom`, and the `esp-idf-monitor` included in the SDK. The default behavior for these commands are:
    - When 'enter' key is pressed on the keyboard, `CR` (0x13) code is sent to the serial device.
    - To move the cursor to the beginning of the next line, serial device needs to send `CR LF` (0x13 0x10) sequence.

### Line editing

The main source file of the example illustrates how to use `linenoise` library, including line completion, hints, and history.

### Commands

Several commands are registered using `esp_console_cmd_register()` function. See the `register_wifi()` and `register_system()` functions in `cmd_wifi.c` and `cmd_system.c` files.

### Command handling

Main loop inside `app_main()` function illustrates how to use `linenoise` and `esp_console_run()` to implement read/eval loop.

### Argument parsing

Several commands implemented in `cmd_wifi.c` and `cmd_system.c` use the Argtable3 library to parse and check the arguments.

### Command history

Each time a new command line is obtained from `linenoise`, it is written into history and the history is saved into a file in flash memory. On reset, history is initialized from that file.
