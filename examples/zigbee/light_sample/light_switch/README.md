| Supported Targets | ESP32-H2 |
| ----------------- | -------- |

# Light Switch Example 

(See the `README.md` file in the upper level 'examples' directory for more information about examples.)

This test code shows how to configure Zigbee end device and use it as a light switch

## Hardware Required

* One development board with ESP32-H2 SoC acting as Zigbee end device (loaded with light switch example)
* A USB cable for power supply and programming
* Choose another ESP32-H2 as Zigbee coordinator (see [light coordinator example](../light_coordinator))
* Choose another ESP32-H2 as Zigbee router (see [light bulb example](../light_bulb))

## Configure the project

Before project configuration and build, make sure to set the correct chip target using `idf.py set-target esp32h2`.

## Build and Flash

Build the project, flash it to the board, and start the monitor tool to view the serial output by running `idf.py -p PORT flash monitor`.

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

As you run the example, you will see the following log:

light switch: 
I (9423) gpio: GPIO[9]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:2  
I (9523) ESP_ZB_SWITCH: status: -1  
I (9523) ESP_ZB_SWITCH: Zigbee stack initialized  
I (9523) ESP_ZB_SWITCH: Start network steering  
I (11263) ESP_ZB_SWITCH: Joined network successfully (Extended PAN ID: f9:54:2d:01:a0:03:f7:84, PAN ID: 0xf5b5)  
I (13433) ESP_ZB_SWITCH: Found bulb addr: 0x434a ep: 10  
I (16433) ESP_ZB_SWITCH: Send ON/OFF toggle command  
I (17663) ESP_ZB_SWITCH: Send ON/OFF toggle command  
I (18693) ESP_ZB_SWITCH: Send ON/OFF toggle command  
I (19833) ESP_ZB_SWITCH: Send ON/OFF toggle command  
I (20753) ESP_ZB_SWITCH: Send ON/OFF toggle command  
I (21363) ESP_ZB_SWITCH: Send ON/OFF toggle command  

## Light Control Functions

  * By toggling the switch button (BOOT) on this board, the LED on the board loaded with the `light bulb` example will be on and off.


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
