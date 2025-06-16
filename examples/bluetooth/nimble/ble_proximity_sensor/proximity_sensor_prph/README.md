| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# BLE Peripheral Proximity Sensor Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example creates GATT server demonstrating standard Proximity Sensor profile. It notifies the application when the distance between devices increases.

It advertises support for the Link Loss service (0x1803) as primary service UUID.

The `ble_prox_prph_task` start alerting to application as soon as a write command is received from central regarding exceeding path loss. The alert stops when path loss is below low_threshold value.
 
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

This console output can be observed when proximity_sensor_prph is connected to client:

```
I (356) BLE_INIT: BT controller compile version [a186b41]
I (356) phy_init: phy_version 970,1856f88,May 10 2023,17:44:12
I (416) BLE_INIT: Bluetooth MAC: 84:f7:03:05:a5:f6

I (416) NimBLE_PROX_PRPH: BLE Host Task Started
I (416) NimBLE: GAP procedure initiated: stop advertising.

I (426) NimBLE: Device Address: 
I (426) NimBLE: 84:f7:03:05:a5:f6
I (436) NimBLE: 

I (436) NimBLE: GAP procedure initiated: advertise; 
I (446) NimBLE: disc_mode=2
I (446) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (456) NimBLE: 

I (456) main_task: Returned from app_main()
I (4546) NimBLE: connection established; status=0

I (3576) NimBLE: Path loss = -112
I (8576) NimBLE: Path loss = -112
I (13626) NimBLE: Path loss = -120
I (18626) NimBLE: Path loss = -53
I (18626) NimBLE: Path loss exceeded threshold, starting alert for device with conn_handle 1
I (19406) NimBLE: Path loss increased for device connected with conn_handle 1
I (20406) NimBLE: Path loss increased for device connected with conn_handle 1
I (21406) NimBLE: Path loss increased for device connected with conn_handle 1
I (22406) NimBLE: Path loss increased for device connected with conn_handle 1
I (23406) NimBLE: Path loss increased for device connected with conn_handle 1
I (23676) NimBLE: Path loss = -90
I (24406) NimBLE: Path loss increased for device connected with conn_handle 1
I (25406) NimBLE: Path loss increased for device connected with conn_handle 1
I (26406) NimBLE: Path loss increased for device connected with conn_handle 1
I (27406) NimBLE: Path loss increased for device connected with conn_handle 1
I (28406) NimBLE: Path loss increased for device connected with conn_handle 1
I (28676) NimBLE: Path loss = -119
I (28676) NimBLE: Path loss lower than threshold, stopping alert for device with conn_handle 1
I (33676) NimBLE: Path loss = -119
I (38726) NimBLE: Path loss = -119
I (43726) NimBLE: Path loss = -119

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
