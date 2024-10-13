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

```
I (388) main_task: Calling app_main()
I (398) gpio: GPIO[9]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:2
I (398) phy_init: phy_version 220,2dbbbe7,Sep 25 2023,20:39:25
I (478) phy: libbtbb version: 90c587c, Sep 25 2023, 20:39:57
I (488) ESP_ZB_ON_OFF_SWITCH: ZDO signal: ZDO Config Ready (0x17), status: ESP_FAIL
I (488) ESP_ZB_ON_OFF_SWITCH: Zigbee stack initialized
I (488) ESP_ZB_ON_OFF_SWITCH: Start network formation
I (498) main_task: Returned from app_main()
I (998) ESP_ZB_ON_OFF_SWITCH: ZDO signal: NWK Permit Join (0x36), status: ESP_OK
I (998) ESP_ZB_ON_OFF_SWITCH: Formed network successfully (Extended PAN ID: 60:55:f9:00:00:f6:07:b4, PAN ID: 0x2a74, Channel:13)
I (1468) ESP_ZB_ON_OFF_SWITCH: ZDO signal: NWK Permit Join (0x36), status: ESP_OK
I (1468) ESP_ZB_ON_OFF_SWITCH: Network steering started
I (14228) ESP_ZB_ON_OFF_SWITCH: ZDO signal: NWK Device Associated (0x12), status: ESP_OK
I (14728) ESP_ZB_ON_OFF_SWITCH: ZDO signal: ZDO Device Update (0x30), status: ESP_OK
I (14788) ESP_ZB_ON_OFF_SWITCH: New device commissioned or rejoined (short: 0xe399)
I (14858) ESP_ZB_ON_OFF_SWITCH: Found light
I (14858) ESP_ZB_ON_OFF_SWITCH: Try to bind On/Off
I (14858) ESP_ZB_ON_OFF_SWITCH: Bound successfully!
I (14858) ESP_ZB_ON_OFF_SWITCH: The light originating from address(0xe399) on endpoint(10)
I (15338) ESP_ZB_ON_OFF_SWITCH: ZDO signal: ZDO Device Authorized (0x2f), status: ESP_OK
I (15408) ESP_ZB_ON_OFF_SWITCH: ZDO signal: NWK Permit Join (0x36), status: ESP_OK
I (35838) ESP_ZB_ON_OFF_SWITCH: ZDO signal: NLME Status Indication (0x32), status: ESP_OK
I (38548) ESP_ZB_ON_OFF_SWITCH: Send 'on_off toggle' command
I (39598) ESP_ZB_ON_OFF_SWITCH: Send 'on_off toggle' command
I (41148) ESP_ZB_ON_OFF_SWITCH: Send 'on_off toggle' command
```

## Light Control Functions

  * By toggling the switch button (BOOT) on this board, the LED on the board loaded with the `HA_on_off_light` example will turn on and off.


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
