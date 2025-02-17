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
I (441) main_task: Started on CPU0                                                                                                                                                                                                                                                        
I (441) main_task: Calling app_main()                                                                                                        
I (461) phy: phy_version: 230,2, 9aae6ea, Jan 15 2024, 11:17:12       
I (461) phy: libbtbb version: 944f18e, Jan 15 2024, 11:17:25                                                                                 
I (471) main_task: Returned from app_main()                                                                                                  
I (601) ESP_ZB_ON_OFF_SWITCH: ZDO signal: ZDO Config Ready (0x17), status: ESP_FAIL                                                          
I (601) ESP_ZB_ON_OFF_SWITCH: Initialize Zigbee stack                                                                                        
I (611) gpio: GPIO[9]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:2                                                 
I (611) ESP_ZB_ON_OFF_SWITCH: Deferred driver initialization successful                                                                      
I (621) ESP_ZB_ON_OFF_SWITCH: Device started up in  factory-reset mode                                                                       
I (621) ESP_ZB_ON_OFF_SWITCH: Start network formation                                                                                        
W (781) ESP_ZB_ON_OFF_SWITCH: Network(0x13af) closed, devices joining not allowed.                                                           
I (781) ESP_ZB_ON_OFF_SWITCH: Formed network successfully (Extended PAN ID: 74:4d:bd:ff:fe:63:f7:30, PAN ID: 0x13af, Channel:13, Short Address: 0x0000)
I (1391) ESP_ZB_ON_OFF_SWITCH: Network(0x13af) is open for 180 seconds                                                                       
I (1391) ESP_ZB_ON_OFF_SWITCH: Network steering started                                                                                      
I (9561) ESP_ZB_ON_OFF_SWITCH: ZDO signal: NWK Device Associated (0x12), status: ESP_OK                                                      
I (9561) ESP_ZB_ON_OFF_SWITCH: ZDO signal: ZDO Device Update (0x30), status: ESP_OK                                                          
I (9601) ESP_ZB_ON_OFF_SWITCH: New device commissioned or rejoined (short: 0x7c16)                                                           
I (9671) ESP_ZB_ON_OFF_SWITCH: Found light                            
I (9671) ESP_ZB_ON_OFF_SWITCH: Try to bind On/Off                                                                                            
I (9681) ESP_ZB_ON_OFF_SWITCH: Bound successfully!                                                                                           
I (9681) ESP_ZB_ON_OFF_SWITCH: The light originating from address(0x7c16) on endpoint(10)                                                    
I (9751) ESP_ZB_ON_OFF_SWITCH: ZDO signal: ZDO Device Authorized (0x2f), status: ESP_OK                                                      
I (9781) ESP_ZB_ON_OFF_SWITCH: Network(0x13af) is open for 180 seconds                                                                       
I (16451) ESP_ZB_ON_OFF_SWITCH: Send 'on_off toggle' command                                                                                 
I (17011) ESP_ZB_ON_OFF_SWITCH: Send 'on_off toggle' command                                                                                 
I (17441) ESP_ZB_ON_OFF_SWITCH: Send 'on_off toggle' command                                                                                 
I (17831) ESP_ZB_ON_OFF_SWITCH: Send 'on_off toggle' command
```

## Light Control Functions

  * By toggling the switch button (BOOT) on this board, the LED on the board loaded with the `HA_on_off_light` example will turn on and off.


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
