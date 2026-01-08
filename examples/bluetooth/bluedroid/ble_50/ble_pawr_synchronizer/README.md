| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | -------- | --------- | -------- |

# ESP-IDF PAwR Synchronizer Example

This example demonstrates **Periodic Advertising with Responses (PAwR)** on the synchronizer side.

- The device performs **extended scanning** and creates **periodic sync** to the advertiser.
- After sync is established, it selects a list of subevents to synchronize to via `esp_ble_gap_set_periodic_sync_subevent()`.
- For received subevents, it sends responses via `esp_ble_gap_set_periodic_adv_response_data()`.
- The response data includes the synchronizer's BLE address, allowing the advertiser to establish a connection.
- The device acts as a **GATTS server** with a service table, ready to accept connections from the advertiser.

## How to Use

### Pairing Test: Synchronizer + Advertiser

This example should be paired with [ble_pawr_advertiser](../ble_pawr_advertiser) or [ble_pawr_advertiser_conn](../ble_pawr_advertiser_conn) for testing.

**Test Setup:**
1. **Device A (Advertiser)**: Flash `ble_pawr_advertiser` or `ble_pawr_advertiser_conn` example
2. **Device B (Synchronizer)**: Flash this `ble_pawr_synchronizer` example

**Steps:**
1. Build and flash advertiser example to Device A:
   ```bash
   cd examples/bluetooth/bluedroid/ble_50/ble_pawr_advertiser
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
4. **Power on Device B** (synchronizer starts scanning)
5. Device B will automatically detect Device A by device name "ESP_EXTPAwR_ADV"
6. Device B creates periodic sync and selects subevents
7. Both devices exchange PAwR data

**Expected Log Hints:**

**On Synchronizer (Device B):**
- `Extended scanning start, status 0`
- `Local address cached: XX:XX:XX:XX:XX:XX, type = X`
- `Create sync with the peer device ESP_EXTPAwR_ADV`
- `Periodic advertising sync establish, status 0`
- `PA sync subevent, status 0`
- `create attribute table successfully, the number handle = X`
- `Service started, service_handle = X`
- `Set PA response data, status 0` (repeatedly when sending responses)
- `[Connection established], conn_id = 0xXXXX` (when advertiser connects)

**On Advertiser (Device A):**
- `PAwR advertising started successfully`
- `[Request] data: 0x00, subevt start:0, subevt count:5`
- `[Response] adv_handle:0, subevent:0, tx_status:0, num_rsp:1`
