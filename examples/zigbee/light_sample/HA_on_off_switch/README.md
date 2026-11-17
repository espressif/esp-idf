| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | --------- | -------- | --------- |

# Light Switch Example

This example demonstrates how to configure a Home Automation On/Off switch on a Zigbee End Device.

The ESP Zigbee SDK provides more examples and tools for productization:
* [ESP Zigbee SDK Docs](https://docs.espressif.com/projects/esp-zigbee-sdk)
* [ESP Zigbee SDK Repo](https://github.com/espressif/esp-zigbee-sdk)

## Hardware Required

* One 802.15.4-enabled development board running this example.
* A second board running as a Zigbee Coordinator (see the [HA_on_off_light](../HA_on_off_light/) example).
* A USB cable for power supply and programming.

## Configure the project

Before project configuration and build, set the correct chip target using `idf.py set-target TARGET`.

## Erase the NVRAM

Before flashing the board, erase the NVRAM with `idf.py -p PORT erase-flash` if you do not want to retain
information stored by previous examples or projects.

## Build and Flash

Build the project, flash it to the board, and start the monitor tool to view the serial output by running
`idf.py -p PORT flash monitor`.

(To exit the serial monitor, type ``Ctrl-]``.)

## Application Functions

When the program starts, the board attempts to join an available Zigbee network:

```
I (427) ON_OFF_SWITCH: Start ESP Zigbee Stack
I (467) ON_OFF_SWITCH: Initialize Zigbee stack
I (467) ON_OFF_SWITCH: Deferred driver initialization successful
I (467) ON_OFF_SWITCH: Device started up in factory-reset mode
I (1727) ON_OFF_SWITCH: Joined network successfully: PAN ID(0x3051, EXT: 0x4831b7fffec183f0), Channel(13), Short Address(0xe5e0)
```

After joining, the switch finds and binds a Home Automation light:

```
I (1727) ON_OFF_SWITCH: Attempt to find HA light device
I (1827) ON_OFF_SWITCH: Attempt to bind HA light device (short address: 0x0000)
I (1847) ON_OFF_SWITCH: Bound HA light device successfully
```

Pressing the `BOOT` button sends an On/Off Toggle command to the bound device:

```
I (24447) ON_OFF_SWITCH: Sent ZCL On/Off Toggle request
I (24517) ON_OFF_SWITCH: Received ZCL Default Response with status(0x00)
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub.
