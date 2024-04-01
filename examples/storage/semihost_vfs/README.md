| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Semihosting VFS driver example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use semihosting VFS driver with ESP32. Example does the following steps:

1. Uses `esp_vfs_semihost_register` function to register exposed host directory in VFS, enabling C standard library and POSIX functions to be used.
2. Redirects `stdout` from the UART to the file on the host using `freopen`.
3. Prints several messages to the redirected.
4. Switches back to UART `stdout` using `freopen`.
5. Opens text file on the host.
6. Reads the file and prints its content on stdout.

## How to use example

### Hardware and tools required

This example requires a development board with JTAG interface, for example:

- ESP32-Wrover-Kit, ESP32-Ethernet-Kit for ESP32
- ESP32-S2-Kaluga for ESP32-S2
- For ESP32-C3 or ESP32-S3, any board with the built-in USB interface (USB_SERIAL_JTAG)
- ESP-Prog as an external JTAG adapter with any other development board

This example also requires [OpenOCD](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html#run-openocd) to be set up.

### Build and flash

1. Replace PORT with serial port name and run this command to build and flash the example:

   ```
   idf.py -p PORT flash
   ```

   See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


2. Go to `data` subdirectory of the project and run OpenOCD.

   ```
   cd data
   openocd -f board/esp32-wrover-kit-3.3v.cfg
   ```

   Note that you need to use the correct configuration file for your board after `-f` option in the above command. Please refer to the list of configuration files available for [ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/jtag-debugging/tips-and-quirks.html#jtag-debugging-tip-openocd-configure-target), [ESP32-S2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-guides/jtag-debugging/tips-and-quirks.html#jtag-debugging-tip-openocd-configure-target), [ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/jtag-debugging/tips-and-quirks.html#jtag-debugging-tip-openocd-configure-target), [ESP32-C3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/jtag-debugging/tips-and-quirks.html#jtag-debugging-tip-openocd-configure-target).

3. With OpenOCD still running, open another console or terminal and run IDF monitor there:

   ```
   idf.py monitor
   ```

   (To exit the serial monitor, type ``Ctrl-]``.)

### Overriding the base directory for semihosting

When the example application calls `esp_vfs_semihost_register("/host")`, the path `/host` on the ESP target is mapped to the semihosting _base directory_. By default, this is the directory where OpenOCD program is started from. In the instructions above, OpenOCD is started in `data` subdirectory of the example project.

When debugging with Xtensa based SoCs (ESP32, ESP32-S2, ESP32-S3) it is possible to override the semihosting base directory using an additional flag of `openocd` command. For example, on Linux and macOS:

```
openocd -c "set ESP_SEMIHOST_BASEDIR $IDF_PATH/examples/storage/semihost_vfs/data" -f board/esp32-wrover-kit-3.3v.cfg
```

or on Windows:

```
openocd -c "set ESP_SEMIHOST_BASEDIR %IDF_PATH%/examples/storage/semihost_vfs/data" -f board/esp32-wrover-kit-3.3v.cfg
```

The above command will set `ESP_SEMIHOST_BASEDIR` variable to `examples/storage/semihost_vfs/data` subdirectory of ESP-IDF. With that, it is not necessary to run OpenOCD from that specific directory.

> Note: This feature is not available for RISC-V based SoCs (ESP32-C3, ESP32-H2). To set the semihosting base directory, change into the required directory before running `openocd` command.

## Example output

There are two outputs produced by example:

1. The example creates and writes `esp32_stdout.txt` file in the `data` directory of the project:

   ```
   W (274) example: Switched to semihosted stdout
   Semihosted stdout write 0
   Semihosted stdout write 1
   Semihosted stdout write 2
   ...
   Semihosted stdout write 98
   Semihosted stdout write 99
   W (274) example: Switch to UART stdout
   ```

2. The example reads [data/host_file.txt](data/host_file.txt) and prints its contents to the serial console:

   ```
   W (274) example: Switch to semihosted stdout
   W (274) example: Switched back to UART stdout
   I (274) example: Wrote 2798 bytes
   ====================== HOST DATA START =========================
   The following are the graphical (non-control) characters defined by
   ISO 8859-1 (1987).  Descriptions in words aren't all that helpful,
   but they're the best we can do in text.  A graphics file illustrating
   the character set should be available from the same archive as this
   file.

   Hex Description                 Hex Description

   20  SPACE
   ...
   7D  RIGHT CURLY BRACKET         FD  SMALL LETTER Y WITH ACUTE
   7E  TILDE                       FE  SMALL LETTER THORN (Icelandic)
                                   FF  SMALL LETTER Y WITH DIAERESIS
   ====================== HOST DATA END =========================
   I (694) example: Read 6121 bytes
   ```

