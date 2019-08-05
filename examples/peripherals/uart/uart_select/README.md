# UART Select Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The UART select example is for demonstrating the use of `select()` for
synchronous I/O multiplexing on the UART interface. The example waits for a
character from UART using `select()` until a blocking read without delay or a
successful non-blocking read is possible.

Please note that the same result can be achieved by using `uart_read_bytes()`
but the use of `select()` allows to use it together with other virtual
file system (VFS) drivers, e.g. LWIP sockets.

This example can be used to develop applications for non-blocking read and write from/to various sources (UART,
sockets, ...) where a ready resource can be served without being blocked by a busy resource.

For a more comprehensive example please refer to `system/select`.

## How to use example

### Hardware Required

The example can be run on any ESP32 development board connected to a PC with a single USB cable for communication
through UART.

### Configure the project

```
idf.py menuconfig
```
or
```
idf.py menuconfig
```

* Set serial port under Serial Flasher Options.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```
or
```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

You can see a similar output after flashing and monitoring the application:

```
...
I (276) cpu_start: Pro cpu start user code
I (294) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (10295) uart_select_example: Timeout has been reached and nothing has been received
I (15295) uart_select_example: Timeout has been reached and nothing has been received
I (20295) uart_select_example: Timeout has been reached and nothing has been received
```

You can push any key on your keyboard to see the result of a successful detection by `select()` and subsequent
blocking read (without delay). The following output shows the result of pushing `a` on the keyboard:

```
...
I (15295) uart_select_example: Timeout has been reached and nothing has been received
I (20295) uart_select_example: Timeout has been reached and nothing has been received
I (20945) uart_select_example: Received: a
I (25955) uart_select_example: Timeout has been reached and nothing has been received
...
```
