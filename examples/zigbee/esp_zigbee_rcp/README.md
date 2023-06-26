| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |

# Rcp Example

This test code shows how to configure Zigbee rcp (radio co-processor) device. Rcp doesn't function alone, it needs to work together with Zigbee gateway (see [esp_zigbee_gateway example](../esp_zigbee_gateway))

## Hardware Required

* One development board with ESP32-H2 SoC acting as Zigbee rcp (loaded with esp_zigbee_rcp example)
* A USB cable for power supply and programming
* Choose ESP32 or ESP32-S3 as Zigbee gateway. The connection and setup refer to the Zigbee gateway example for setup details (see [esp_zigbee_gateway example](../esp_zigbee_gateway))
* TX, RX pin can be also configured by user in esp_zigbee_rcp.h

## Configure the project

Before project configuration and build, make sure to set the correct chip target using `idf.py --preview set-target TARGET` command.

## Erase the NVRAM

Before flash it to the board, it is recommended to erase NVRAM if user doesn't want to keep the previous examples or other projects stored info using `idf.py -p PORT erase-flash`

## Build and Flash

Build the project, flash it to the board by running `idf.py -p <PORT> build flash`

## Rcp Functions

 * After rcp starts up, it will send its own MAC ieee address and Zigbee stack version number to the Zigbee gateway and start working together with Zigbee gateway via UART communication
 * For more log info please see Zigbee gateway side

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon
