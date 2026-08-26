| Supported Targets | ESP32-C5 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- | -------- | --------- | -------- | --------- |

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
I (...) CTE_ADV_EXAMPLE: DIRECTION_FINDING Example Periodic Adv AOA Mode
I (...) CTE_ADV_EXAMPLE: BLE Host Task Started
I (...) CTE_ADV_EXAMPLE: Device Address:
I (...) CTE_ADV_EXAMPLE: xx:xx:xx:xx:xx:xx
I (...) NimBLE: GAP procedure initiated: extended advertise; instance=0

I (...) CTE_ADV_EXAMPLE: Instance 0 started (periodic)
I (...) main_task: Returned from app_main()
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
