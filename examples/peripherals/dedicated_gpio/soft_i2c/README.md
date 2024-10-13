| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Example: Software I2C Master via Dedicated/Fast GPIOs

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to configure and use dedicated/fast GPIOs to emulate an I2C master. The I2C master requires 2 bi-directional pins for SCL (clock) and SDA (data) operating in open-drain mode, thus requires further configuration compared to other buses like SPI or UART.

### Data sent on the bus

After configuring the I/Os, the example performs a write-read transaction on the bus.
The device address, the write buffer and read buffer size can be configured in the function `emulate_i2c` in the main source file.
It is also possible to provide a NULL as a write or read buffer in order to perform a single `read` or `write` on the bus.

### Note on function placement

Due to the tight timing requirements of SW bit banging, the `emulate_i2c_transfer` function has been placed in IRAM. This circumvents the timing variations caused by cache misses/invalidation when executing a function placed in flash.

## How to use example

### Hardware Required

* A development board with an Espressif SoC
* A USB cable for Power supply and programming
* Some jumper wires to connect the I2C device to the development board.

### Configure the project

#### Watchdog timers

Due to the strict timing requirements of the I2C emulation, the I2C emulation will cause the CPU to disable interrupts while bit banging in order to ensure it is not preempted (by a task or ISR). Therefore, this example disables both the *Interrupt Watchdog* and *Task Watchdog* by default (see `sdkconfig.defaults`) to prevent the non-preemptive emulation from triggering either watchdog timer. Note that this is normally not permitted by ESP-IDF.

#### Bus frequency

The SCL frequency implemented is around 200-300KHz. Indeed, the example uses the internal pull-ups for SCL and SDA lines. These resistors are rather high, more than 10kOhms. If you wish to reach higher frequencies, please add external pull-up resistors (1kOhms or 2kOhms) and update `set_scl` and `set_sda` functions, in `i2c.c`, to reduce the delay.

### Build and flash the project

* Set the target of the project to a supported one. For example:
```
idf.py set-target esp32c3
```
* Configure the GPIOs to use as the software I2C bus in the menu `Example configuration` of `menuconfig`. By default, SCL and SDA are assigned to GPIOs 0, 2 respectively for RISC-V.
```
idf.py menuconfig
```
* Optional: update the `device_address`, write/read buffer array accordingly in the source code.
* Compile the example:
```
idf.py build
```
* Flash the example:
```
idf.py flash
```
* Plug the I2C device on the pins defined by the configuration.
* Power on the board and check the output:
```
idf.py monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

As you run the example, on success, you will see the following log:

```
Bytes received from the I2C device:
ff, ff
Emulation terminated: success
```

Of course, the bytes printed on screen depend on the data received from the connected I2C device.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
