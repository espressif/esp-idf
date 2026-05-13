| Supported Targets | ESP32-C5 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- | -------- | --------- | -------- | --------- |

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
I (...) CTE_SYNC_EXAMPLE: BLE Host Task Started
I (...) NimBLE: GAP procedure initiated: extended discovery;

I (...) CTE_SYNC_EXAMPLE: Receive CTE Antenna Pattern Info:
I (...) CTE_SYNC_EXAMPLE: aoa slot:2
I (...) CTE_SYNC_EXAMPLE: pattern_len: 4
I (...) CTE_SYNC_EXAMPLE: Started periodic sync with device

I (...) CTE_SYNC_EXAMPLE: pattern:
I (...) CTE_SYNC_EXAMPLE: 00 04 08 0c
I (...) CTE_SYNC_EXAMPLE: Local CTE Antenna Info:
I (...) CTE_SYNC_EXAMPLE:   switch_sampling_rates: 7
I (...) CTE_SYNC_EXAMPLE:   num_antennae: 4
I (...) CTE_SYNC_EXAMPLE:   max_switch_pattern_len: 16
I (...) CTE_SYNC_EXAMPLE:   max_cte_len: 20
I (...) main_task: Returned from app_main()

I (...) CTE_SYNC_EXAMPLE: Periodic Sync Established
I (...) CTE_SYNC_EXAMPLE: Periodic Sync Event:
I (...) CTE_SYNC_EXAMPLE:   Status: 0
I (...) CTE_SYNC_EXAMPLE:   Sync Handle: 0
I (...) CTE_SYNC_EXAMPLE:   SID: 2
I (...) CTE_SYNC_EXAMPLE:   Adv PHY: 1M
I (...) CTE_SYNC_EXAMPLE:   Interval: ...
I (...) CTE_SYNC_EXAMPLE:   Clock Accuracy: ...
I (...) CTE_SYNC_EXAMPLE:   Adv Addr: xx:xx:xx:xx:xx:xx

I (...) CTE_SYNC_EXAMPLE: IQ Report | Sync Handle: 0  IQ num: ...  RSSI: ...  cte_type: 0  channel: ...
I (...) CTE_SYNC_EXAMPLE: I: <iq_i_samples...>
I (...) CTE_SYNC_EXAMPLE: Q: <iq_q_samples...>
I (...) CTE_SYNC_EXAMPLE: IQ Report | Sync Handle: 0  IQ num: ...  RSSI: ...  cte_type: 0  channel: ...
I (...) CTE_SYNC_EXAMPLE: I: <iq_i_samples...>
I (...) CTE_SYNC_EXAMPLE: Q: <iq_q_samples...>
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
