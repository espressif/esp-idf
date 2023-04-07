| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |

# Light Switch Example

This test code shows how to configure Zigbee Coordinator and use it as an HA on/off_switch.

The ESP Zigbee SDK provides more examples and tools for productization:
* [ESP Zigbee SDK Docs](https://docs.espressif.com/projects/esp-zigbee-sdk)
* [ESP Zigbee SDK Repo](https://github.com/espressif/esp-zigbee-sdk)

## Hardware Required

* One development board with ESP32-H2 SoC acting as Zigbee Coordinator (loaded with HA_on_off_switch)
* A USB cable for power supply and programming
* Choose another ESP32-H2 as Zigbee end-device (see [HA_on_off_light](../HA_on_off_light/))

## Configure the project

Before project configuration and build, make sure to set the correct chip target using `idf.py --preview set-target TARGET` command.

## Erase the NVRAM

Before flash it to the board, it is recommended to erase NVRAM if user doesn't want to keep the previous examples or other projects stored info using `idf.py -p PORT erase-flash`

## Build and Flash

Build the project, flash it to the board, and start the monitor tool to view the serial output by running `idf.py -p PORT flash monitor`.

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

As you run the example, you will see the following log:

light switch:
```
I (318) gpio: GPIO[9]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:2
I (328) system_api: Base MAC address is not set
I (328) system_api: read default base MAC address from EFUSE
I (408) phy: libbtbb version: 6c47ec3, Mar 16 2022, 18:54:24
I (408) phy: phy_version: 101, bb2a234, Mar 16 2022, 18:54:11
I (818) ESP_ZB_ON_OFF_SWITCH: status: 255
I (818) ESP_ZB_ON_OFF_SWITCH: Zigbee stack initialized
I (818) ESP_ZB_ON_OFF_SWITCH: Start network formation
I (1318) ESP_ZB_ON_OFF_SWITCH: Formed network successfully (Extended PAN ID: ff:fc:7c:c0:f0:bd:97:10, PAN ID: 0x88e7)
I (1778) ESP_ZB_ON_OFF_SWITCH: status: 0
I (5528) ESP_ZB_ON_OFF_SWITCH: status: 0
I (6038) ESP_ZB_ON_OFF_SWITCH: status: 0
I (6068) ESP_ZB_ON_OFF_SWITCH: New device commissioned or rejoined (short: 0x2878)
I (6098) ESP_ZB_ON_OFF_SWITCH: User find cb: address:0x2878, endpoint:10
I (6638) ESP_ZB_ON_OFF_SWITCH: status: 0
I (6678) ESP_ZB_ON_OFF_SWITCH: status: 0
I (8168) ESP_ZB_ON_OFF_SWITCH: send move to on_off toggle command
I (8898) ESP_ZB_ON_OFF_SWITCH: send move to on_off toggle command
I (9458) ESP_ZB_ON_OFF_SWITCH: send move to on_off toggle command
I (10088) ESP_ZB_ON_OFF_SWITCH: send move to on_off toggle command
I (10588) ESP_ZB_ON_OFF_SWITCH: send move to on_off toggle command
I (11098) ESP_ZB_ON_OFF_SWITCH: send move to on_off toggle command
```

## Light Control Functions

  * By toggling the switch button (BOOT) on this board, the LED on the board loaded with the `HA_on_off_light` example will turn on and off.


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
