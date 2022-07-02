| Supported Targets | ESP32-H2 |
| ----------------- | -------- |

# Light Bulb Example 

(See the `README.md` file in the upper level 'examples' directory for more information about examples.)

This test code shows how to configure Zigbee end device and use it as a light bulb

## Hardware Required

* One development board with ESP32-H2 SoC acting as Zigbee end device (loaded with light bulb example)
* A USB cable for power supply and programming
* Choose another ESP32-H2 as Zigbee coordinator (see [light switch example](../light_switch))

## Configure the project

Before project configuration and build, make sure to set the correct chip target using `idf.py set-target esp32h2`.

## Build and Flash

Build the project, flash it to the board, and start the monitor tool to view the serial output by running `idf.py -p PORT flash monitor`.

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

As you run the example, you will see the following log:

light bulb:
I (918) ESP_ZB_LIGHT: status: 255
I (901) ESP_ZB_LIGHT: Zigbee stack initialized
I (901) ESP_ZB_LIGHT: Start network steering
I (2611) ESP_ZB_LIGHT: Joined network successfully (Extended PAN ID: aa:98:48:01:a0:03:f7:84, PAN ID: 0x0e8b)
I (5651) ESP_ZB_LIGHT: on/off attribute setting to 1
I (6631) ESP_ZB_LIGHT: on/off attribute setting to 0
I (7331) ESP_ZB_LIGHT: on/off attribute setting to 1
I (8251) ESP_ZB_LIGHT: on/off attribute setting to 0
I (9111) ESP_ZB_LIGHT: on/off attribute setting to 1
I (9671) ESP_ZB_LIGHT: on/off attribute setting to 0

## Light Control Functions

 * By toggling the switch button (BOOT) on the ESP32-H2 board loaded with the `light switch` example, the LED on this board loaded with `light bulb` example will be on and off.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
