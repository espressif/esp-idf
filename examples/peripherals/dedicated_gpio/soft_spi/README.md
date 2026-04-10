| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# Example: SPI software emulation using dedicated/fast GPIOs

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This test code shows how to configure and use dedicated/fast GPIOs to emulate a full-duplex SPI bus. The ESP target will act as a master, so it will require 3 output pins (for the clock, MOSI and chip select) and 1 input pin (for MISO).

### Data sent on the bus

After configuring the I/Os, the example writes a buffer on MOSI line while receiving bytes, at the same time, on MISO line. The buffer to output on the bus can be modified in the function `emulate_spi` in the main source file.

### Note on routine placement

Due to the tight timing requirements of SW bit banging, the `asm_emulate_spi` function has been placed in IRAM. This circumvents the timing variations caused by cache misses/invalidation when executing a function placed in flash.

## How to use example

### Hardware Required

* A development board with a RISC-V Espressif SoC (e.g., ESP32-C3 or ESP32-C2)
* A USB cable for Power supply and programming
* Some jumper wires to connect the SPI device to the development board.

### Configure the project

#### Watchdog timers

Due to the strict timing requirements of the SPI emulation, the SPI emulation will cause the CPU to disable interrupts while bit banging in order to ensure it is not preempted (by a task or ISR). Therefore, this example disables both the *Interrupt Watchdog* and *Task Watchdog* by default (see `sdkconfig.defaults`) to prevent the non-preemptive emulation from triggering either watchdog timer. Note that this is normally not permitted by ESP-IDF.

### Build and flash the project

* Set the target of the project to a RISC-V-based one. For example:
```
idf.py set-target esp32c3
```
* Configure the GPIOs to use as the software SPI bus in the menu `Example configuration` of `menuconfig`. By default, clock, MOSI, chip select and MISO are assigned to GPIOs 0, 2, 3 and 4 respectively.
```
idf.py menuconfig
```
* Optional: change the array `tx_buffer` containing data to be outputted on MOSI line.
* Compile the example:
```
idf.py build
```
* Flash the example:
```
idf.py flash
```
* Plug the SPI device on the pins defined by the configuration.
* Power on the board and check the output:
```
idf.py monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

As you run the example, on success, you will see the following log:

```
Bytes received from the SPI device:
ff, ff, ff, ff
Emulation terminated: success
```

Of course, the bytes printed on screen depend on the data received from the connected SPI device.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
