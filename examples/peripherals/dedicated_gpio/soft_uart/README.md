| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Example: UART software emulation using dedicated/fast GPIOs

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to emulate a UART bus using dedicated/fast GPIOs. The ESP target (acting as a UART device) will send and receive characters on the UART bus using a TX pin and an RX pin respectively. The TX and RX pins can be configured `menuconfig`.

The baudrate will determine has fast bits are transferred on the bus, and the baudrate is configurable via `menuconfig`. Due to UART being an asynchronous protocol, the timing of I/O read/writes have to be very precise. This example is hard coded to emulates an 8N1 (8-bit characters, no parity bit, 1 stop bit) UART bus, thus this format cannot be configured via `menuconfig`.

### Data sent on the bus

After configuring the I/Os and the baudrate, the example sends characters on the TX line before waiting for characters on the RX line. Reading will stop once `\r` or `\n` is sent.
The message to send first and the size of the receive buffer can be changed in the function `emulate_uart` in the source file.

### Note on routine placement

Due to the tight timing requirements of SW bit banging, the `asm_emulate_uart` function has been placed in IRAM. This circumvents the timing variations caused by cache misses/invalidation when executing a function placed in flash.

## How to use example

### Hardware Required

* A development board with one of the supported chips (see the list above.)
* A USB cable for Power supply and programming
* Some jumper wires to connect the UART to an external UART-to-USB adapter.

### Configure the project

#### Watchdog timers

Due to the strict timing requirements of the UART emulation, the UART emulation will cause the CPU to disable interrupts while bit banging in order to ensure it is not preempted (by a task or ISR). Therefore, this example disables both the *Interrupt Watchdog* and *Task Watchdog* by default (see `sdkconfig.defaults`) to prevent the non-preemptive emulation from triggering either watchdog timer. Note that this is normally not permitted by ESP-IDF.

### Build and flash the project

* Set the target of the project to a compatible one. For example:
```
idf.py set-target esp32c3
```
* Configure the GPIOs to use as the software UART bus in the menu `Example configuration` of `menuconfig`. By default, TX and RX are assigned to GPIOs 0, 2 respectively.
```
idf.py menuconfig
```
* Optional: update the message to send and the read buffer size accordingly in the source code.
* Compile the example:
```
idf.py build
```
* Flash the example:
```
idf.py flash
```
* Plug the external UART-to-USB adapter on the pins defined by the configuration and open a monitor on the computer it is connected to.
* Power on the Espressif board and check the output:
```
idf.py monitor
```
* Type some characters on the monitor opened on the emulated UART bus after you see the message:
```
Type a message and press enter to terminate
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

As you run the example, you will see the following log on the external software UART monitor:
```
Type a message and press enter to terminate
```

After typing "Hello world!" followed by "Enter:, the Espressif target monitor should show:
```
Received on emulated UART: "Hello world!"
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
