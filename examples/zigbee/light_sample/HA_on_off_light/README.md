| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | --------- | -------- | --------- |

# Light Bulb Example

This example demonstrates how to configure a Home Automation on/off light on a Zigbee Coordinator using ESP Zigbee SDK v2.

The ESP Zigbee SDK provides more examples and tools for productization:
* [ESP Zigbee SDK Docs](https://docs.espressif.com/projects/esp-zigbee-sdk)
* [ESP Zigbee SDK Repo](https://github.com/espressif/esp-zigbee-sdk)

## Hardware Required

* One 802.15.4-enabled development board running this example as a Zigbee Coordinator
* A USB cable for power supply and programming
* A second board running as a Zigbee Router/End Device (see [HA_on_off_switch](../HA_on_off_switch))

## Configure the project

Before project configuration and build, make sure to set the correct chip target using `idf.py set-target TARGET`.

## Erase the NVRAM

Before flash it to the board, it is recommended to erase NVRAM if user doesn't want to keep the previous examples or other projects stored info using `idf.py -p PORT erase-flash`

## Build and Flash

Build the project, flash it to the board, and start the monitor tool to view the serial output by running `idf.py -p PORT flash monitor`.

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

When the program starts, the board forms a Zigbee network and opens it for joining. As you run the example, you will see output similar to:

```
I (435) ON_OFF_LIGHT: Start ESP Zigbee Stack
I (475) ON_OFF_LIGHT: Initialize Zigbee stack
I (475) ON_OFF_LIGHT: Deferred driver initialization successful
I (485) ON_OFF_LIGHT: Device started up in factory-reset mode
I (795) ON_OFF_LIGHT: Formed network successfully: PAN ID(0x3051, EXT: 0x4831b7fffec183f0), Channel(13), Short Address(0x0000)
I (1175) ON_OFF_LIGHT: Network steering completed
I (3705) ON_OFF_LIGHT: New device commissioned or rejoined(short: 0xa515)
```

## Light Control Functions

* By toggling the switch button (BOOT) on the board loaded with the `HA_on_off_switch` example, the LED on this board will turn on and off.

```
I (26905) ON_OFF_LIGHT: ZCL SetAttributeValue message for endpoint(10) cluster(0x0006) server with status(0x00)
I (26905) ON_OFF_LIGHT: Set On/Off: 1
I (27535) ON_OFF_LIGHT: ZCL SetAttributeValue message for endpoint(10) cluster(0x0006) server with status(0x00)
I (27535) ON_OFF_LIGHT: Set On/Off: 0
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
