| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# UART REPL Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use REPL console on a different UART than the default one.
It also shows how to connect these two UART together, either for testing or for sending commands
without any human interaction.

## How to use example

### Hardware Required

The example can be run on any ESP board that have at least 2 UARTs. The development board shall be connected to a
PC with a single USB cable for flashing and monitoring. If you are willing to monitor the console UART, you may use
a 3.3V compatible USB-to-Serial dongle on its GPIO pin.

### Setup the Hardware

No external connection is needed in order to run the example. However, as stated before, if you are willing to see what
is going on on the second UART (console UART), you can connect pins CONSOLE_UART_TX_PIN (5 by default) and
CONSOLE_UART_RX_PIN (4 by default) to a Serial-to-USB adapter.

### Configure the project

The default values, located at the top of `main/uart_repl_example_main.c` can be changed such as:
DEFAULT_UART_CHANNEL, CONSOLE_UART_CHANNEL, DEFAULT_UART_RX_PIN, DEFAULT_UART_TX_PIN, CONSOLE_UART_RX_PIN,
CONSOLE_UART_TX_PIN, UARTS_BAUD_RATE, TASK_STACK_SIZE, and READ_BUF_SIZE.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view default UART's serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The example will set up the default UART to use DEFAULT_UART_RX_PIN and DEFAULT_UART_TX_PIN. Then, it will set up
the REPL console on the second UART. Finally, it will connect both UARTs together in order to let default UART
be able to send commands and receive replies to and from the console UART.

Here is a diagram of what UARTs will look like:

```
                  UART default      UART console

USB monitoring <------ TX -----------> RX----+
                                             v
                                       Parse command
                                     and output result
                                             |                 Optional 3.3V
                       RX <----------- TX<---+  (----------->) Serial-to-USB
                                                                  Adapter
```

If everything goes fine, the output on default UART should be "Result: Success". Else, it should be "Result: Failure".