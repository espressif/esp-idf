| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- |

# BLE Multi Adv Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example support legacy as well as extended advertisement for all phy. For multi advertising, random addresses are generated for each instance.

It starts 4 types of advertising:
* Non connectable extended
* Connectable extended
* Scannable legacy
* Legacy withe specified duration(5 sec)

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Configure the Project

Open the project configuration menu:
```
idf.py menuconfig
```

To configure number of advertising instances:

* Component config → Bluetooth → NimBLE Options → Enable BLE 5 feature → Maximum number of extended advertising instances

### Hardware Required

* A development board with ESP32-C3 SoC,ESP32-S3/ESP32-H2/ESP32-C2 SoC and BLE5.0 supported chips (e.g., ESP32-C3-DevKitC, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.


## Example Output

```
I (404) NimBLE_MULTI_ADV: BLE Host Task Started

I (414) NimBLE_MULTI_ADV: Instance 0 started
I (414) NimBLE_MULTI_ADV: Instance 1 started
I (434) NimBLE_MULTI_ADV: Instance 2 started
I (444) NimBLE_MULTI_ADV: Instance 3 started

I (454) main_task: Returned from app_main()
I (5404) NimBLE: advertise complete; reason=13
I (5404) NimBLE_MULTI_ADV: In ble_legacy_duration_cb, instance = 3

I (5404) NimBLE_MULTI_ADV: Instance 3 started
I (10384) NimBLE: advertise complete; reason=13
I (10384) NimBLE_MULTI_ADV: In ble_legacy_duration_cb, instance = 3

I (10394) NimBLE_MULTI_ADV: Instance 3 started

```
## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
