| Supported Targets | ESP32-C5 | ESP32-H2 |
| ----------------- | -------- | -------- |

# Bluetooth LE Direction Finding Example (Periodic Sync with CTE) 

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example performs passive scanning on non-connectable, non-scannable extended advertisements, then establishes periodic synchronization with a specified advertiser containing CTE information. It listens to its periodic advertisements and prints the CTE IQ data.

It uses the Bluetooth controller and the Bluetooth LE host based on the NimBLE stack.

This example aims to understand how to enable CTE reception in BLE periodic synchronization advertisements and how to obtain CTE IQ reports.

To test this demonstration, use the specified extended advertisement named "Periodic ADV with CTE" as the periodic advertiser.


## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A CTE antenna array development board with ESP32_CTE_Antenna_Array_Board (e.g., ESP32-H2_BLE_AOA&AOD_Antenna_Array_Board V1.0).
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Configure the Project (Optional)

If needed, you can customize the synchronization matching method and antenna control GPIO settings through the configuration menu:

```bash
idf.py menuconfig
```

Navigate to the `Example Configuration` menu and do the following if required :

* Change the Periodic Sync Matching Method option.
* Configure the GPIO pin settings for antenna control.

> Note: These configurations are optional. Only modify them if you need to adjust the default behavior. 

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful periodic sync:

```
I (372) main_task: Started on CPU0
I (372) main_task: Calling app_main()
I (382) BLE_INIT: Using main XTAL as clock source
I (382) BLE_INIT: ble controller commit:[c223b2b]
I (392) BLE_INIT: Bluetooth MAC: 60:55:f9:f7:44:a5
I (402) phy: phy_version: 322,2, 823e7f8, Mar  3 2025, 16:09:11
I (422) phy: libbtbb version: e9c8b26, Mar  3 2025, 16:09:24
I (422) CTE_SYNC_EXAMPLE: 

███████╗███████╗██████╗     ██████╗ ██╗     ███████╗                                                                      
██╔════╝██╔════╝██╔══██╗    ██╔══██╗██║     ██╔════╝                                                                      
█████╗  ███████╗██████╔╝    ██████╔╝██║     █████╗                                                                        
██╔══╝  ╚════██║██╔═══╝     ██╔══██╗██║     ██╔══╝                                                                        
███████╗███████║██║         ██████╔╝███████╗███████╗                                                                      
╚══════╝╚══════╝╚═╝         ╚═════╝ ╚══════╝╚══════╝                                                                      
                                                                                                                          
██████╗ ██╗██████╗ ███████╗ ██████╗████████╗██╗ ██████╗ ███╗   ██╗    ███████╗██╗███╗   ██╗██████╗ ██╗███╗   ██╗ ██████╗  
██╔══██╗██║██╔══██╗██╔════╝██╔════╝╚══██╔══╝██║██╔═══██╗████╗  ██║    ██╔════╝██║████╗  ██║██╔══██╗██║████╗  ██║██╔════╝  
██║  ██║██║██████╔╝█████╗  ██║        ██║   ██║██║   ██║██╔██╗ ██║    █████╗  ██║██╔██╗ ██║██║  ██║██║██╔██╗ ██║██║  ███╗ 
██║  ██║██║██╔══██╗██╔══╝  ██║        ██║   ██║██║   ██║██║╚██╗██║    ██╔══╝  ██║██║╚██╗██║██║  ██║██║██║╚██╗██║██║   ██║ 
██████╔╝██║██║  ██║███████╗╚██████╗   ██║   ██║╚██████╔╝██║ ╚████║    ██║     ██║██║ ╚████║██████╔╝██║██║ ╚████║╚██████╔╝ 
╚═════╝ ╚═╝╚═╝  ╚═╝╚══════╝ ╚═════╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝    ╚═╝     ╚═╝╚═╝  ╚═══╝╚═════╝ ╚═╝╚═╝  ╚═══╝ ╚═════╝  
                                                                                                                          

I (712) DF: bind gpio 1 to signal 57

I (712) DF: bind gpio 0 to signal 58

I (722) DF: bind gpio 2 to signal 59

I (722) DF: bind gpio 25 to signal 60

I (722) CTE_SYNC_EXAMPLE: BLE Host Task Started
I (732) NimBLE: Failed to restore IRKs from store; status=8

I (732) NimBLE: GAP procedure initiated: extended discovery; 

I (742) CTE_SYNC_EXAMPLE: Receive CTE Antenna Pattern Info:
I (752) CTE_SYNC_EXAMPLE: aoa slot:2
I (752) CTE_SYNC_EXAMPLE: pattern_len: 4
I (752) CTE_SYNC_EXAMPLE: pattern:
I (762) CTE_SYNC_EXAMPLE: 00 04 08 0c
I (762) CTE_SYNC_EXAMPLE: Local CTE Antenna Info:
I (762) CTE_SYNC_EXAMPLE:   switch_sampling_rates: 7
I (772) CTE_SYNC_EXAMPLE:   num_antennae: 4
I (772) CTE_SYNC_EXAMPLE:   max_switch_pattern_len: 16
I (782) CTE_SYNC_EXAMPLE:   max_cte_len: 20
I (782) main_task: Returned from app_main()
I (812) CTE_SYNC_EXAMPLE: Started periodic sync with device

I (1722) CTE_SYNC_EXAMPLE: Periodic Sync Established
I (1722) CTE_SYNC_EXAMPLE: Periodic Sync Event:
I (1722) CTE_SYNC_EXAMPLE:   Status: 0
I (1722) CTE_SYNC_EXAMPLE:   Sync Handle: 0
I (1722) CTE_SYNC_EXAMPLE:   SID: 2
I (1732) CTE_SYNC_EXAMPLE:   Adv PHY: 1M
I (1732) CTE_SYNC_EXAMPLE:   Interval: 640
I (1742) CTE_SYNC_EXAMPLE:   Clock Accuracy: 4
I (1742) CTE_SYNC_EXAMPLE:   Adv Addr: 10:21:34:8D:5A:C4
I (2522) CTE_SYNC_EXAMPLE: IQ Report | Sync Handle: 0  IQ num: 45  RSSI: -65  cte_type: 0  channel: 28
I (2522) CTE_SYNC_EXAMPLE: I: -48,-76,36,83,-18,-87,-2,85,-45,-37,-69,-104,-46,28,-16,25,34,35,69,100,45,-39,11,-35,-44,-32,-68,-97,-46,32,-7,29,41,32,70,93,40,-35,5,-35,-51,-34,-72,-101,48,
I (2532) CTE_SYNC_EXAMPLE: Q: -67,42,82,-26,-83,9,87,5,52,-28,16,-16,-41,-29,-66,-94,-45,34,-3,29,41,34,71,107,46,-29,13,-26,-45,-29,-70,-90,-47,37,-1,40,51,36,71,100,43,-32,-1,-27,-37,
I (3322) CTE_SYNC_EXAMPLE: IQ Report | Sync Handle: 0  IQ num: 45  RSSI: -62  cte_type: 0  channel: 5
I (3322) CTE_SYNC_EXAMPLE: I: 76,-15,-83,6,81,9,-81,-24,-36,-1,54,40,30,22,39,34,37,-1,-53,-43,-26,-24,-41,-38,-38,-2,48,42,24,23,42,36,38,1,-50,-39,-24,-24,-43,-36,-39,-5,46,39,46,
I (3332) CTE_SYNC_EXAMPLE: Q: -24,-81,11,82,1,-81,-15,79,-32,-25,-37,-39,-37,-2,51,40,26,22,39,32,39,-2,-52,-41,-29,-24,-43,-36,-42,-1,49,36,26,22,42,35,38,-1,-49,-41,-28,-25,-45,-38,17,
I (4122) CTE_SYNC_EXAMPLE: IQ Report | Sync Handle: 0  IQ num: 45  RSSI: -67  cte_type: 0  channel: 11
I (4122) CTE_SYNC_EXAMPLE: I: 75,6,-75,-19,71,38,-64,-44,-6,53,64,79,49,45,87,73,14,-47,-64,-74,-49,-39,-89,-67,-9,52,61,86,47,44,87,72,18,-45,-59,-77,-46,-39,-90,-72,-12,50,57,86,23,
I (4132) CTE_SYNC_EXAMPLE: Q: 1,-73,-11,72,30,-67,-41,59,-47,-41,-87,-77,-15,46,64,76,45,39,87,72,16,-51,-60,-84,-49,-43,-90,-72,-13,51,63,79,47,41,92,70,15,-50,-59,-79,-50,-44,-92,-69,42,
I (4922) CTE_SYNC_EXAMPLE: IQ Report | Sync Handle: 0  IQ num: 45  RSSI: -67  cte_type: 0  channel: 13
I (4922) CTE_SYNC_EXAMPLE: I: -27,62,44,-49,-52,41,63,-28,57,24,62,53,-14,-55,-59,-80,-44,-21,-63,-51,8,68,50,93,59,26,56,53,-2,-52,-54,-87,-53,-13,-64,-45,21,67,56,87,48,22,63,50,-41,
I (4932) CTE_SYNC_EXAMPLE: Q: 65,35,-57,-49,47,62,-35,-63,19,63,58,90,56,23,60,50,0,-57,-56,-85,-44,-14,-58,-47,17,56,61,79,43,17,68,48,-12,-66,-56,-90,-56,-19,-57,-56,7,51,60,87,-12,
I (5722) CTE_SYNC_EXAMPLE: IQ Report | Sync Handle: 0  IQ num: 45  RSSI: -70  cte_type: 0  channel: 20



```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
