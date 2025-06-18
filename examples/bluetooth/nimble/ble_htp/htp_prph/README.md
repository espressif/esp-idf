| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# BLE Heart Rate Measurement Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example creates GATT server demonstrating standard Health Thermometer profile. It simulates temperature measurement and notifies to client when the notifications are enabled.

It advertises support for the Health Thermometer service (0x1809) as primary service UUID.

When internmediate temperature characteristic is subscribed, the timer resets and it sends notifications to client every 1 sec.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host

This example aims at understanding notification subscriptions and sending notifications.

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

This console output can be observed when blehr is connected to client and client enables notifications:

```
I (356) BLE_INIT: BT controller compile version [921f253]
I (356) phy_init: phy_version 950,f732b06,Feb 15 2023,18:57:12
I (396) BLE_INIT: Bluetooth MAC: 70:04:1d:13:66:fe

I (396) NimBLE_BLE_Health_Therm: BLE Host Task Started
I (396) NimBLE: GAP procedure initiated: stop advertising.

I (406) NimBLE: Failed to restore IRKs from store; status=8

I (406) NimBLE: Device Address: 
I (416) NimBLE: 70:04:1d:13:66:fe
I (416) NimBLE: 

I (416) NimBLE: GAP procedure initiated: advertise; 
I (426) NimBLE: disc_mode=2
I (426) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (436) NimBLE: 

I (446) main_task: Returned from app_main()
I (6116) NimBLE: connection established; status=0

I (7946) NimBLE: subscribe event; cur_notify=0
 value handle; val_handle=12

I (7956) BLE_GAP_SUBSCRIBE_EVENT: conn_handle from subscribe=1

I (8046) NimBLE: subscribe event; cur_notify=1
 value handle; val_handle=17

I (8056) BLE_GAP_SUBSCRIBE_EVENT: conn_handle from subscribe=1
I (9056) NimBLE: GATT procedure initiated: notify; 
I (9056) NimBLE: att_handle=17


```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
