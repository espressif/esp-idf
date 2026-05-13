| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

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
I (751) NimBLE_HTP_PRPH: BLE Host Task Started
I (751) main_task: Returned from app_main()
I (791) NimBLE: Device Address:
I (791) NimBLE: b0:b2:1c:b4:6b:4e
I (791) NimBLE:

I (801) NimBLE: GAP procedure initiated: advertise;
I (801) NimBLE: disc_mode=2
I (801) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (811) NimBLE:

I (4441) NimBLE: connection established; status=0

I (7641) NimBLE: subscribe event; cur_notify=0
 value handle; val_handle=16

I (7641) BLE_GAP_SUBSCRIBE_EVENT: conn_handle from subscribe=0
I (7741) NimBLE: subscribe event; cur_notify=1
 value handle; val_handle=21

I (7741) BLE_GAP_SUBSCRIBE_EVENT: conn_handle from subscribe=0
I (8741) NimBLE: GATT procedure initiated: notify;
I (8741) NimBLE: att_handle=21

I (8741) NimBLE: GATT procedure initiated: notify;
I (8741) NimBLE: att_handle=21

I (8741) NimBLE: Notification sent successfully
I (9741) NimBLE: GATT procedure initiated: notify;
I (9741) NimBLE: att_handle=21

I (9741) NimBLE: GATT procedure initiated: notify;
I (9741) NimBLE: att_handle=21

I (9741) NimBLE: Notification sent successfully
I (10741) NimBLE: GATT procedure initiated: notify;
I (10741) NimBLE: att_handle=21

I (10741) NimBLE: GATT procedure initiated: notify;
I (10741) NimBLE: att_handle=21

I (10741) NimBLE: Notification sent successfully
I (11741) NimBLE: GATT procedure initiated: notify;
I (11741) NimBLE: att_handle=21

I (11741) NimBLE: GATT procedure initiated: notify;
I (11741) NimBLE: att_handle=21

I (11741) NimBLE: Notification sent successfully
I (12741) NimBLE: GATT procedure initiated: notify;
I (12741) NimBLE: att_handle=21


```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
