| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |

# Light Bulb Example

This test code shows how to configure Zigbee end device and use it as HA on/off light bulb.

The ESP Zigbee SDK provides more examples and tools for productization:
* [ESP Zigbee SDK Docs](https://docs.espressif.com/projects/esp-zigbee-sdk)
* [ESP Zigbee SDK Repo](https://github.com/espressif/esp-zigbee-sdk)

## Hardware Required

* One development board with ESP32-H2 SoC acting as Zigbee end device (loaded with HA_on_off_light example)
* A USB cable for power supply and programming
* Choose another ESP32-H2 as Zigbee coordinator (see [HA_on_off_switch example](../HA_on_off_switch))

## Configure the project

Before project configuration and build, make sure to set the correct chip target using `idf.py --preview set-target TARGET` command.

## Erase the NVRAM

Before flash it to the board, it is recommended to erase NVRAM if user doesn't want to keep the previous examples or other projects stored info using `idf.py -p PORT erase-flash`

## Build and Flash

Build the project, flash it to the board, and start the monitor tool to view the serial output by running `idf.py -p PORT flash monitor`.

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

As you run the example, you will see the following log:

light bulb:
```
I (918) ESP_ZB_LIGHT: status: 255
I (901) ESP_ZB_LIGHT: Zigbee stack initialized
I (901) ESP_ZB_LIGHT: Start network steering
I (2611) ESP_ZB_LIGHT: Joined network successfully (Extended PAN ID: aa:98:48:01:a0:03:f7:84, PAN ID: 0x0e8b)
I (5651) ESP_ZB_LIGHT: on/off light set to 1
I (6631) ESP_ZB_LIGHT: on/off light set to 0
I (7331) ESP_ZB_LIGHT: on/off light set to 1
I (8251) ESP_ZB_LIGHT: on/off light set to 0
I (9111) ESP_ZB_LIGHT: on/off light set to 1
I (9671) ESP_ZB_LIGHT: on/off light set to 0
```

## Light Control Functions

 * By toggling the switch button (BOOT) on the ESP32-H2 board loaded with the `HA_on_off_switch` example, the LED on this board loaded with `HA_on_off_light` example will be on and off.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
