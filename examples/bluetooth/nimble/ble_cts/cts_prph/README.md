| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

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
I (357) BLE_INIT: BT controller compile version [59725b5]
I (357) BLE_INIT: Bluetooth MAC: 60:55:f9:68:c4:fa
I (367) phy_init: phy_version 1110,9c20f0a,Jul 27 2023,10:42:54
I (407) NimBLE_cts_PRPH: BLE Host Task Started
I (407) NimBLE: GAP procedure initiated: stop advertising.

I (407) NimBLE: Failed to restore IRKs from store; status=8

I (407) NimBLE: Device Address:
I (417) NimBLE: 60:55:f9:68:c4:fa
I (417) NimBLE:

I (417) NimBLE: GAP procedure initiated: advertise;
I (427) NimBLE: disc_mode=2
I (427) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (437) NimBLE:

I (447) main_task: Returned from app_main()

I (27317) NimBLE: GAP procedure initiated: advertise;
I (27317) NimBLE: disc_mode=2
I (27327) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (27337) NimBLE:

I (31987) NimBLE: connection established; status=0

I (53297) NimBLE: subscribe event; cur_notify=1
 value handle; val_handle=12

 I (68317) NimBLE: subscribe event; cur_notify=0
  value handle; val_handle=12

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
