| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# USB_OTG CDC Console Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example is similar to the [advanced console example](../advanced/README.md), but instead of the UART it uses the USB CDC port provided by USB_OTG peripheral for console output.
The example uses the [Console Component](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/console.html#console) to create an interactive shell.

The interactive shell implemented in this example contains a wide variety of commands, and can act as a basis for applications that require a command-line interface (CLI).

## How to use example

### Hardware Required

This example can run on an ESP32-S2 or ESP32-S3 development board with USB port attached to the built-in USB_OTG controller.

For more details about connecting and configuring USB_OTG (including pin numbers), see the IDF Programming Guide:
* [ESP32-S2 USB_OTG](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s2/api-guides/usb-otg-console.html)
* [ESP32-S3 USB_OTG](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-guides/usb-otg-console.html)

### Build and Flash

Build the project, by running:

```
idf.py build
```

First time flashing can be done over USB or over UART.

#### Flashing the project for the first time using UART

This is the same procedure described in the Getting Started Guide. Flashing is done using a USB-Serial adapter included on the development board. Determine the device name of the serial port (`PORT`) and run:

```
idf.py -p PORT flash
```

#### Flashing the project for the first time using USB

We can also use the built-in USB bootloader in the ESP chip to flash the program for the first time.

1. Connect the development board using a USB breakout cable to the computer.
2. Put the chip into bootloader (download) mode, by holding "Boot" button (connected to GPIO0) and pressing "Reset" button.
3. A USB serial port device will appear in the system:
   - On Windows, it will be one of COM ports
   - On Linux, it will by /dev/ttyACMn (n is some number)
   - On macOS, it will be /dev/tty.usbmodemN (N is some number)

  If the port doesn't appear, check that the board has entered the bootloader mode, and that the USB cable is connected correctly.

4. Flash the board:

```
idf.py -p PORT flash
```

### View the console output over USB

Once the board is flashed for the first time, a USB serial port will appear in the system.

**Note: if you have used the step above to flash the board over USB, note that the port name may change after flashing!**

To start the monitor, run:

```
idf.py -p PORT monitor
```

To exit the serial monitor, type ``Ctrl-]``.

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Enter the `help` command get a full list of all available commands. The following is a sample session of the Console Example where a variety of commands provided by the Console Example are used. Note that GPIO15 is connected to GND to remove the boot log output.

```
This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
esp32s2> help
help
  Print the list of registered commands

free
  Get the current size of free heap memory

heap
  Get minimum size of free heap memory that was available during program execution

version
  Get version of chip and SDK

restart
  Software reset of the chip

tasks
  Get information about running tasks

(remaining output of the help command skipped here)

esp32s2> free
237824
I (39864) cmd_system: Restarting
I (185) spi_flash: detected chip: generic
I (186) spi_flash: flash io: dio
I (186) cpu_start: Starting scheduler on PRO CPU.

This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
esp32s2>
```

## Troubleshooting

### Line Endings

The line endings in the Console Example are configured to match particular serial monitors. Therefore, if the following log output appears, consider using a different serial monitor (e.g. Putty for Windows).

```
This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
Your terminal application does not support escape sequences.
Line editing and history features are disabled.
On Windows, try using Putty instead.
esp32s2>
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
