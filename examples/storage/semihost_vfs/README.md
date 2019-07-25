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

This example does not require any special hardware, and can be run on any common development board.
This example requires [OpenOCD](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/index.html#run-openocd).
NOTE: In order to run this example you need OpenOCD version `v0.10.0-esp32-20190313` or later.

Run OpenOCD using command:
```
bin/openocd -s share/openocd/scripts -f interface/ftdi/esp32_devkitj_v1.cfg -c 'set ESP_SEMIHOST_BASEDIR '$IDF_PATH/examples/storage/semihost_vfs/data -f board/esp-wroom-32.cfg
```
This command also configures OpenOCD to expose example project `data` subdirectory to the target's semihosting VFS driver.

### Build and flash

Replace PORT with serial port name:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

There are two types of outputs produced by example: 
1. File `esp32_stdout.txt` in the host directory mounted to the target:

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

2. On the boards console:

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

