| Supported Targets | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

# UART OTA Example

This example demonstrates how to use UART to perform an Over-the-Air (OTA) firmware update on an ESP32-based device. It transfers firmware data via UART using DMA and applies the update seamlessly.

## How to use example

### Hardware Required

The example can be run on any development board, that is based on the Espressif SoC. The board shall be connected to a computer with a single USB cable for flashing and monitoring. The external interface should have 3.3V outputs. You may use e.g. 3.3V compatible USB-to-Serial dongle.

### Software Required

1. Partition Table:

The example requires a valid partition table with an OTA partition. Update your partitions.csv or equivalent partition table to include an OTA data partition. Below is an example of a partition table with OTA support:

```
# Name,   Type, SubType, Offset,   Size, Flags
nvs,      data, nvs,     0x9000,   0x6000,
factory,  app,  factory, 0x10000,  1M,
ota_0,    app,  ota_0,   0x110000, 1M,
ota_1,    app,  ota_1,   0x210000, 1M,
```

You can also use `CONFIG_PARTITION_TABLE_TWO_OTA_LARGE` if you don't want to define a partition table by yourself.

2. Firmware Binary:

Prepare the firmware binary (.bin file) you want to flash via OTA. This binary should be compatible with the ESP32 platform and must match the partition table configuration.
Example:

*1.* Build the firmware binary via:

```
idf.py build
```

*2.* Locate the binary file in the build/ directory (e.g., build/your_project.bin)

### Setup the Hardware

Connect the external serial interface to the board as follows.

```

                    +-------------------+                          +-------------------+
                    |   ESP Chip        |                          | External UART Pin |
                    |   Interface       |                          |                   |
                    +-------------------+                          +-------------------+
                            |                                            |
                            |                                            |
Receive Data (RxD):         + GPIO9 <------------------------------> TxD + 
                            |                                            |
Ground:                     + GND <--------------------------------> GND +
                            |                                            |

```

### Configure the project

Use the command below to configure project using Kconfig menu as showed in the table above.
The default Kconfig values can be changed such as: UART_BAUD_RATE, UART_PORT_NUM, UART_RX_IO (Refer to Kconfig file).

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

Then transmit the prepared bin to ESP-chips. Please note that the bin should be in one packet transaction which means there should not have any interval during the transaction in this example because it uses idle to judge whether the packet finishes or not.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (277) main_task: Started on CPU0
I (287) main_task: Calling app_main()
I (287) uhci-example: UHCI initialized
I (447) uhci-example: OTA process started
I (18047) uhci-example: Received size: 150032
I (18047) uhci-example: Total received size: 150032
I (18047) esp_image: segment 0: paddr=001d0020 vaddr=3c020020 size=06748h ( 26440) map
I (18057) esp_image: segment 1: paddr=001d6770 vaddr=3fc8b400 size=0111ch (  4380)
I (18067) esp_image: segment 2: paddr=001d7894 vaddr=40380000 size=08784h ( 34692)
I (18077) esp_image: segment 3: paddr=001e0020 vaddr=42000020 size=11d98h ( 73112) map
....
I (18147) uhci-example: OTA update successful. Rebooting...
.... (the following part depends on your bin, this output depends on example/get-started/hello_world)
I (268) sleep_gpio: Enable automatic switching of GPIO sleep configuration
I (275) main_task: Started on CPU0
I (275) main_task: Calling app_main()
Hello world!
This is esp32c3 chip with 1 CPU core(s), WiFi/BLE, silicon revision v0.4, 4MB external flash
Minimum free heap size: 333524 bytes
Restarting in 10 seconds...
Restarting in 9 seconds...
Restarting in 8 seconds...
```



