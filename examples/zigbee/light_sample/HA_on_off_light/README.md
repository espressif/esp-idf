| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- | -------- |

# Light Bulb Example

This test code shows how to configure Zigbee end device and use it as HA on/off light bulb.

The ESP Zigbee SDK provides more examples and tools for productization:
* [ESP Zigbee SDK Docs](https://docs.espressif.com/projects/esp-zigbee-sdk)
* [ESP Zigbee SDK Repo](https://github.com/espressif/esp-zigbee-sdk)

## Hardware Required

* One development board with ESP32-H2 SoC acting as Zigbee end-device (loaded with HA_on_off_light example)
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

```
I (403) app_start: Starting scheduler on CPU0
I (408) main_task: Started on CPU0
I (408) main_task: Calling app_main()
I (428) phy: phy_version: 230,2, 9aae6ea, Jan 15 2024, 11:17:12
I (428) phy: libbtbb version: 944f18e, Jan 15 2024, 11:17:25
I (438) main_task: Returned from app_main()
I (548) ESP_ZB_ON_OFF_LIGHT: ZDO signal: ZDO Config Ready (0x17), status: ESP_FAIL
I (548) ESP_ZB_ON_OFF_LIGHT: Initialize Zigbee stack
W (548) rmt: channel resolution loss, real=10666666
I (558) gpio: GPIO[8]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
I (548) ESP_ZB_ON_OFF_LIGHT: Deferred driver initialization successful
I (568) ESP_ZB_ON_OFF_LIGHT: Device started up in  factory-reset mode
I (578) ESP_ZB_ON_OFF_LIGHT: Start network steering
I (3558) ESP_ZB_ON_OFF_LIGHT: Joined network successfully (Extended PAN ID: 74:4d:bd:ff:fe:63:f7:30, PAN ID: 0x13af, Channel:13, Short Address: 0x7c16)
I (10238) ESP_ZB_ON_OFF_LIGHT: Received message: endpoint(10), cluster(0x6), attribute(0x0), data size(1)
I (10238) ESP_ZB_ON_OFF_LIGHT: Light sets to On
I (10798) ESP_ZB_ON_OFF_LIGHT: Received message: endpoint(10), cluster(0x6), attribute(0x0), data size(1)
I (10798) ESP_ZB_ON_OFF_LIGHT: Light sets to Off
I (11228) ESP_ZB_ON_OFF_LIGHT: Received message: endpoint(10), cluster(0x6), attribute(0x0), data size(1)
I (11228) ESP_ZB_ON_OFF_LIGHT: Light sets to On
I (11618) ESP_ZB_ON_OFF_LIGHT: Received message: endpoint(10), cluster(0x6), attribute(0x0), data size(1)
I (11618) ESP_ZB_ON_OFF_LIGHT: Light sets to Off
```

## Light Control Functions

 * By toggling the switch button (BOOT) on the ESP32-H2 board loaded with the `HA_on_off_switch` example, the LED on this board loaded with `HA_on_off_light` example will be on and off.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
