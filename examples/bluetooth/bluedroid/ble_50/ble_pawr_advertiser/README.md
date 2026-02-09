| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | -------- | --------- | -------- |

# BLE Periodic Advertiser With Response (PAwR) Advertiser Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates **Periodic Advertising with Responses (PAwR)** on the advertiser side.

This example starts PAwR advertising with configurable subevents and response slots.

It uses Bluetooth controller and Bluedroid stack based BLE host.

This example aims at understanding PAwR advertisement and related Bluedroid APIs.

## How to Test

### Pairing Test: Advertiser + Synchronizer

This example should be paired with [ble_pawr_synchronizer](../ble_pawr_synchronizer) for testing.

**Test Setup:**
1. **Device A (Advertiser)**: Flash `ble_pawr_advertiser` example
2. **Device B (Synchronizer)**: Flash `ble_pawr_synchronizer` example

**Steps:**
1. Power on Device A first (advertiser starts broadcasting)
2. Power on Device B (synchronizer starts scanning)
3. Device B will automatically detect and sync to Device A
4. Both devices will exchange PAwR data

**Expected Behavior:**
- Advertiser sends periodic advertising with subevents
- Synchronizer receives subevents and sends responses
- Advertiser receives response reports

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Configure the project

Open the project configuration menu:

```bash
idf.py menuconfig
```

In the `Example Configuration` menu:

* Select I/O capabilities of device from `Example Configuration --> I/O Capability`, default is `Just_works`.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (504) BLE_PAWR_ADV: Device Address: 40:4c:ca:46:1f:e2
I (514) BLE_PAWR_ADV: Extended advertising params set, status 0, instance 0
I (524) BLE_PAWR_ADV: Extended advertising data set, status 0, instance 0
I (534) BLE_PAWR_ADV: Extended advertising start, status 0, instance num 1
I (544) BLE_PAWR_ADV: Periodic advertising params set, status 0, instance 0
I (554) BLE_PAWR_ADV: Periodic advertising start, status 0, instance 0
I (564) BLE_PAWR_ADV: PAwR advertising started successfully
I (574) BLE_PAWR_ADV: BLE PAwR Advertiser example started
I (664) BLE_PAWR_ADV: [Request] data: 0x00, subevt start:0, subevt count:5
I (814) BLE_PAWR_ADV: [Request] data: 0x05, subevt start:5, subevt count:2
I (964) BLE_PAWR_ADV: [Request] data: 0x07, subevt start:7, subevt count:2
I (1114) BLE_PAWR_ADV: [Request] data: 0x09, subevt start:9, subevt count:2
I (1163054) BLE_PAWR_ADV: [Response] adv_handle:0, subevent:0, tx_status:0, num_rsp:1
I (1163054) BLE_PAWR_ADV:   Response slot:114, data_length:1
I (1163054) BLE_PAWR_ADV:   data: 0x8c, 0x0
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.

