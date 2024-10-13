| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

# Gateway Example

This example demonstrates how to build a Zigbee Gateway device.

The ESP Zigbee SDK provides more examples and tools for productization:
* [ESP Zigbee SDK Docs](https://docs.espressif.com/projects/esp-zigbee-sdk)
* [ESP Zigbee SDK Repo](https://github.com/espressif/esp-zigbee-sdk)

## Hardware Required

By default, two SoCs are required to run this example:
* An ESP32 series Wi-Fi SoC (ESP32, ESP32-C, ESP32-S, etc) loaded with this esp_zigbee_gateway example.
* An ESP32-H2 802.15.4 SoC loaded with [esp_zigbee_rcp](../esp_zigbee_rcp) example

Connect the two SoCs via UART, below is an example setup with ESP32-DevKitC and ESP32-H2-DevKitC:
![Zigbee_gateway](../../openthread/ot_br/image/thread-border-router-esp32-esp32h2.jpg)

ESP32 pin     | ESP32-H2 pin
------------- |-------------
   GND        |    G
   GPIO4 (RX) |    TX
   GPIO5 (TX) |    RX

* TX, RX pin can be also configured by user in esp_zigbee_gateway.h

The example could also run on a single SoC which supports both Wi-Fi and Zigbee (e.g., ESP32-C6), but since there is only one RF path in ESP32-C6, which means Wi-Fi and Zigbee can't receive simultaneously, it has a significant impact on performance. Hence the two SoCs solution is recommended.

## Configure the project

Before project configuration and build, make sure to set the correct chip target using `idf.py set-target <chip_name>`.

In order to run the example on single SoC which supports both Wi-Fi and Thread, the option `CONFIG_ESP_COEX_SW_COEXIST_ENABLE` and option `CONFIG_ZB_RADIO_NATIVE` should be enabled. The two options are enabled by default for ESP32-C6 target.

## Erase the NVRAM 

Before flash it to the board, it is recommended to erase NVRAM if user doesn't want to keep the previous examples or other projects stored info using `idf.py -p PORT erase-flash`

## Build and Flash

Build the project, flash it to the board, and start the monitor tool to view the serial output by running `idf.py -p PORT flash monitor`

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

As you run the example, you will see the following log:

esp_zigbee_gateway:
```
I (660) ESP_ZB_GATEWAY: status: -1
I (670) ESP_ZB_GATEWAY: Zigbee stack initialized
I (680) ESP_ZB_GATEWAY: Zigbee rcp device booted
I (1280) ESP_ZB_GATEWAY: Start network formation
I (3060) ESP_ZB_GATEWAY: Formed network successfully (Extended PAN ID: f9:54:2d:01:a0:03:f7:84, PAN ID: 0x8651, Channel:13, Short Address: 0x0000)
I (4060) ESP_ZB_GATEWAY: status: 0
I (4400) ESP_ZB_GATEWAY: Network steering started
```

## Gateway Functions

 * After Zigbee gateway starts up, it will read MAC ieee address and Zigbee stack version number from the Zigbee rcp and start working together with Zigbee rcp via UART communication to form a Zigbee network
 * More Gateway functionalities supporting Wi-Fi interaction will come later

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon
