| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# ULP-RISC-V Interrupt Handling:

This example demonstrates how the ULP-RISC-V coprocessor can register and handle interrupts. Currently this example supports handling of -
- Software triggered interrupts
- RTC IO triggered interrupts

The example keeps a count of the software interrupts triggered on the ULP RISC-V core and wakes up the main processor from deep sleep after a certain threshold.
Additionaly, it wakes up the main processor from deep sleep when a button is pressed and the GPIO interrupt is triggered.

## How to use example

### Hardware Required

The example can be run on any ESP32-S2 or ESP32-S3 based development board connected to a computer with a single USB cable for flashing and monitoring. The external interface should have 3.3V outputs. You may use e.g. 3.3V compatible USB-to-Serial dongle.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

By default, this example uses GPIO#0 which is connected to the "Boot" button on esp32-s2/s3 development boards. To trigger the GPIO interrupt on the ULP RISC-V core, press the Boot button.

## Example Output

Output from main CPU:

```
Not a ULP RISC-V wakeup, initializing it!
Entering in deep sleep

ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x5 (DSLEEP),boot:0x8 (SPI_FAST_FLASH_BOOT)
pro cpu reset by JTAG
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce3810,len:0x12b4
load:0x403c9700,len:0x4
load:0x403c9704,len:0xaf4
load:0x403cc700,len:0x2bcc
entry 0x403c9898
W (91) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
ULP RISC-V woke up the main CPU!
ULP RISC-V SW Interrupt triggered 5 times.
Entering in deep sleep

ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x5 (DSLEEP),boot:0x8 (SPI_FAST_FLASH_BOOT)
pro cpu reset by JTAG
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce3810,len:0x12b4
load:0x403c9700,len:0x4
load:0x403c9704,len:0xaf4
load:0x403cc700,len:0x2bcc
entry 0x403c9898
W (91) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
ULP RISC-V woke up the main CPU!
ULP RISC-V GPIO Interrupt triggered.
Entering in deep sleep

```
