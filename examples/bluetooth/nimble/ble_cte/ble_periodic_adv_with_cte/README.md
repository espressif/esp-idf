| Supported Targets | ESP32-C5 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | --------- | -------- |

# Bluetooth LE Direction Finding Example (Periodic Advertiser With CTE)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example starts periodic advertising with a non-resolvable private address and includes  Constant Tone Extension (CTE) signals.

It uses Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding how to use direction finding features use periodic advertisement and related NimBLE APIs.

To test this demo, BLE Periodic Sync With CTE app can be used as a locator.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Configure the project (Optional)

If you need to change the Direction Finding mode or configure antenna control GPIOs, run the configuration menu:

```bash
idf.py menuconfig
```

Navigate to the `Example Configuration` menu where you can:

* Change the `Direction Finding Mode` if needed.
* Set the GPIO pin configuration for antenna switching when using the `AoD (Antenna Switching with GPIO Encoding)` mode.

> Note: These configurations are optional. Only modify them if you need to adjust the default behavior. 


### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

There is this console output when periodic_adv is started:

```
I (364) main_task: Started on CPU0
I (364) main_task: Calling app_main()
I (374) BLE_INIT: Using main XTAL as clock source
I (374) BLE_INIT: ble controller commit:[c223b2b]
I (384) BLE_INIT: Bluetooth MAC: 74:4d:bd:60:1b:bb
I (394) phy: phy_version: 322,2, 823e7f8, Mar  3 2025, 16:09:11
I (414) phy: libbtbb version: e9c8b26, Mar  3 2025, 16:09:24
I (414) CTE_ADV_EXAMPLE: 
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
                                                                                                                          

I (704) CTE_ADV_EXAMPLE: DIRECTION_FINDING Example Periodic Adv AOA Mode
I (714) CTE_ADV_EXAMPLE: BLE Host Task Started
I (714) NimBLE: Failed to restore IRKs from store; status=8

I (724) CTE_ADV_EXAMPLE: Device Address: 
I (724) CTE_ADV_EXAMPLE: 10:21:34:8d:5a:c4
I (734) NimBLE: GAP procedure initiated: extended advertise; instance=1

I (734) CTE_ADV_EXAMPLE: Instance 1 started (periodic)
I (744) main_task: Returned from app_main()

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
