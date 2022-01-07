| Supported Targets | ESP32-H2 |
| ----------------- | -------- |

# Light Coordinator Example 

(See the `README.md` file in the upper level 'examples' directory for more information about examples.)

This test code shows how to configure Zigbee coordinator device.

## Hardware Required

* One development board with ESP32-H2 SoC acting as Zigbee coordinator (loaded with light coordinator example)
* A USB cable for power supply and programming
* Choose another ESP32-H2 as Zigbee router (see [light bulb example](../light_bulb))
* Choose another ESP32-H2 as Zigbee end-device (see [light switch example](../light_switch))

## Configure the project

Before project configuration and build, make sure to set the correct chip target using `idf.py set-target esp32h2`.

## Build and Flash

Build the project, flash it to the board, and start the monitor tool to view the serial output by running `idf.py -p PORT flash monitor`.

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

As you run the example, you will see the following log:

light coodrinator:
I (9556) ESP_ZB_COORDINATOR: status: -1  
I (9556) ESP_ZB_COORDINATOR: Zigbee stack initialized  
I (9556) ESP_ZB_COORDINATOR: Start network formation  
I (10066) ESP_ZB_COORDINATOR: Joined network successfully (Extended PAN ID: f9:54:2d:01:a0:03:f7:84, PAN ID: 0xf5b5)  
I (10526) ESP_ZB_COORDINATOR: Network steering started  
I (11306) ESP_ZB_COORDINATOR: status: 0  
I (11806) ESP_ZB_COORDINATOR: status: 0  
I (11886) ESP_ZB_COORDINATOR: New device commissioned or rejoined (short: 0x434a)  
I (12386) ESP_ZB_COORDINATOR: status: 0  
I (12486) ESP_ZB_COORDINATOR: status: 0  
I (12896) ESP_ZB_COORDINATOR: status: 0  
I (12946) ESP_ZB_COORDINATOR: New device commissioned or rejoined (short: 0x09c2)  
I (13516) ESP_ZB_COORDINATOR: status: 0  
I (26686) ESP_ZB_COORDINATOR: status: 0  

## Light Control Functions

 * By toggling the switch button (BOOT) on the ESP32-H2 board loaded with `light switch` example, the LED on the board loaded with `light bulb` example will be on and off.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
