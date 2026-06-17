| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# BLE Current Time Service Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example creates GATT server demonstrating standard Current Time Service.

It advertises support for the Current Time Service(0x1805) as primary service UUID.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host


## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This console output can be observed when client is connected to server :

```
I (833) NimBLE_CTS_PRPH: BLE Host Task Started
I (833) main_task: Returned from app_main()
I (883) NimBLE: Device Address:
I (883) NimBLE: b0:b2:1c:b4:74:0e
I (883) NimBLE:

I (883) NimBLE: GAP procedure initiated: advertise;
I (883) NimBLE: disc_mode=2
I (883) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (893) NimBLE:

I (1123) NimBLE: connection established; status=0
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
