| Supported Targets | ESP32-H2 |
| ----------------- | -------- |

# Light Bulb Example 

(See the `README.md` file in the upper level 'examples' directory for more information about examples.)

This test code shows how to configure Zigbee router device and use it as a light bulb

## Hardware Required

* One development board with ESP32-H2 SoC acting as Zigbee router (loaded with light bulb example)
* A USB cable for power supply and programming
* Choose another ESP32-H2 as Zigbee coordinator (see [light coordinator example](../light_coordinator))
* Choose another ESP32-H2 as Zigbee end-device (see [light switch example](../light_switch))

## Configure the project

Before project configuration and build, make sure to set the correct chip target using `idf.py set-target esp32h2`.

## Build and Flash

Build the project, flash it to the board, and start the monitor tool to view the serial output by running `idf.py -p PORT flash monitor`.

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

As you run the example, you will see the following log:

light bulb:
I (9638) ESP_ZB_LIGHT: status: 255  
I (9638) ESP_ZB_LIGHT: Zigbee stack initialized  
I (9648) ESP_ZB_LIGHT: Start network steering  
I (12368) ESP_ZB_LIGHT: Joined network successfully (Extended PAN ID: f9:54:2d:01:a0:03:f7:84, PAN ID: 0xf5b5)  
I (12398) ESP_ZB_LIGHT: status: 0  
I (18158) ESP_ZB_LIGHT: on/off attribute setting to 1  
I (19388) ESP_ZB_LIGHT: on/off attribute setting to 0  
I (20418) ESP_ZB_LIGHT: on/off attribute setting to 1  
I (21558) ESP_ZB_LIGHT: on/off attribute setting to 0  
I (22478) ESP_ZB_LIGHT: on/off attribute setting to 1  
I (23088) ESP_ZB_LIGHT: on/off attribute setting to 0  

## Light Control Functions

 * By toggling the switch button (BOOT) on the ESP32-H2 board loaded with the `light switch` example, the LED on this board loaded with `light bulb` example will be on and off.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
