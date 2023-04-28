| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# ULP-RISC-V bitbanged UART transmit:

This example demonstrates how to program the ULP-RISC-V coprocessor to bitbang an UART TX line. This can be used to log output directly from the ULP, even when the main CPU is in deep sleep.


## How to use example

### Hardware Required

The example can be run on any ESP32-S2 or ESP32-S3 based development board connected to a computer with a single USB cable for flashing and monitoring. The external interface should have 3.3V outputs. You may use e.g. 3.3V compatible USB-to-Serial dongle.

### Setup the Hardware

Connect the external serial interface to the board as follows.

```
  -----------------------------------------------------------------------------------------
  | Target chip Interface | Kconfig Option     | Default ESP Pin      | External UART Pin |
  | ----------------------|--------------------|----------------------|--------------------
  | Transmit Data (TxD)   | EXAMPLE_UART_TXD   | GPIO4                | RxD               |
  | Ground                | n/a                | GND                  | GND               |
  -----------------------------------------------------------------------------------------
```
Note: Some GPIOs can not be used with certain chips because they are reserved for internal use. Please refer to UART documentation for selected target.

### Configure the project

Use the command below to configure project using Kconfig menu as showed in the table above. The baudrate can be changed with by configuring `ULP_RISCV_UART_BAUDRATE` under `components` -> `ULP`, but it is recommended to keep the baudrate low, as driver will not be able to accurately output higher rates.

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

Output from main CPU:

```
Not a ULP wakeup, initializing it!
Entering in deep sleep
```

Output from ULP-RISCV:

``
Hello World from ULP-RISCV!
Cnt: 0x00000000
Hello World from ULP-RISCV!
Cnt: 0x00000001
Hello World from ULP-RISCV!
Cnt: 0x00000002
Hello World from ULP-RISCV!
Cnt: 0x00000003
Hello World from ULP-RISCV!
Cnt: 0x00000004
``

## Troubleshoot

If the received output contains garbled characters then try to lower the baudrate (ULP_RISCV_UART_BAUDRATE) using `menuconfig`.
