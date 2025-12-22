| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | -------- | --------- | -------- |

# BLE Periodic Advertiser With Response (PAwR) Advertiser Connection Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates PAwR connection functionality as PAwR advertiser.

It uses Bluetooth controller and Bluedroid stack based BLE host.

This example aims at understanding how to establish connections using PAwR synchronization and related Bluedroid APIs.

## How to Test

### Pairing Test: Advertiser with Connection + Synchronizer

This example should be paired with [ble_pawr_synchronizer](../ble_pawr_synchronizer) for testing.

**Test Setup:**
1. **Device A (Advertiser with Connection)**: Flash `ble_pawr_advertiser_conn` example
2. **Device B (Synchronizer)**: Flash `ble_pawr_synchronizer` example

**Steps:**
1. Build and flash advertiser example to Device A:
   ```bash
   cd examples/bluetooth/bluedroid/ble_50/ble_pawr_advertiser_conn
   idf.py set-target <chip_name>
   idf.py -p <PORT_A> build flash monitor
   ```

2. Build and flash synchronizer example to Device B:
   ```bash
   cd examples/bluetooth/bluedroid/ble_50/ble_pawr_synchronizer
   idf.py set-target <chip_name>
   idf.py -p <PORT_B> build flash monitor
   ```

3. **Power on Device A first** (advertiser starts broadcasting)
4. **Power on Device B** (synchronizer starts scanning and syncing)
5. Device B sends responses containing connection information
6. Device A receives responses and extracts peer address
7. Device A establishes connection using `esp_ble_gattc_aux_open_with_pawr_synced()`

**Expected Behavior:**
- Advertiser sends periodic advertising with subevents
- Synchronizer receives subevents and sends responses containing its BLE address
- Advertiser receives response reports with connection data
- Advertiser establishes BLE connection to synchronizer using `esp_ble_gattc_aux_open_with_pawr_synced()`
- Advertiser automatically discovers GATT services and characteristics
- Advertiser configures notify to receive data from synchronizer

**Response Data Format:**
The synchronizer sends response data with the following format:
- `data[0]` = 0x00
- `data[1]` = 0x02  
- `data[2-7]` = Synchronizer's BLE address (6 bytes, high to low: addr[5], addr[4], addr[3], addr[2], addr[1], addr[0])
- `data[8+]` = Additional data (optional)

**GATT Service:**
The synchronizer provides a GATT service (UUID: 0x00FF) with:
- Characteristic UUID: 0xFF01
- Properties: Read, Write, Notify
- CCCD for notify configuration

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
I (562) BLE_PAWR_ADV_CONN: Device Address: 40:4c:ca:46:1f:e2
I (572) BLE_PAWR_ADV_CONN: Extended advertising params set, status 0, instance 0
I (582) BLE_PAWR_ADV_CONN: Extended advertising data set, status 0, instance 0
I (592) BLE_PAWR_ADV_CONN: Extended advertising start, status 0, instance num 1
I (602) BLE_PAWR_ADV_CONN: Periodic advertising params set, status 0, instance 0
I (612) BLE_PAWR_ADV_CONN: Periodic advertising start, status 0, instance 0
I (622) BLE_PAWR_ADV_CONN: PAwR advertising started successfully
I (632) BLE_PAWR_ADV_CONN: BLE PAwR Advertiser Connection example started
I (642) BLE_PAWR_ADV_CONN: [Request] data: 0x00, subevt start:0, subevt count:5
I (1842) BLE_PAWR_ADV_CONN: [Request] data: 0x17, subevt start:3, subevt count:2
I (1872) BLE_PAWR_ADV_CONN: [Response] adv_handle:0, subevent:0, tx_status:0, num_rsp:1
I (1872) BLE_PAWR_ADV_CONN:   Response slot:2, data_length:10
I (1872) BLE_PAWR_ADV_CONN:   data: 0x00, 0x02, 0x5e, 0x02, 0xf6, 0xf9, 0x55, 0x60, 0x00, 0x00
I (1882) BLE_PAWR_ADV_CONN: Connection create sent, adv handle = 0, subevent = 0
I (2192) BLE_PAWR_ADV_CONN: [Connection established], conn_id = 0x0000, status = 0x00
I (2202) BLE_PAWR_ADV_CONN: Connection Information:
I (2202) BLE_PAWR_ADV_CONN:   Connection ID: 0x0000
I (2212) BLE_PAWR_ADV_CONN:   Remote Address: 60:55:f9:f6:02:5e
I (2222) BLE_PAWR_ADV_CONN: GATTC open successfully, conn_id = 0x0000, MTU 23
I (2232) BLE_PAWR_ADV_CONN: Service discover complete, conn_id 0
I (2242) BLE_PAWR_ADV_CONN: Service search result, conn_id = 0x0000, is_primary 1
I (2252) BLE_PAWR_ADV_CONN: PAwR Sync Service found
I (2262) BLE_PAWR_ADV_CONN: Characteristic found, handle = X, properties = 0xXX
I (2272) BLE_PAWR_ADV_CONN: Notification register successfully, handle = X
I (2282) BLE_PAWR_ADV_CONN: Descriptor write successfully, notify enabled
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.

