| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# BLE Central Proximity Sensor Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example creates GATT client and performs passive scan, it then connects to peripheral device if the device advertises connectability and the device advertises support for the Link Loss service (0x1803) as primary service UUID.

It alerts the application when link gets disconnected.

After connection it enables bonding and link encryprion if the `Enable Link Encryption` flag is set in the example config.

It performs following GATT operations against the specified peer:

* Writes the alert level characteristic of link loss service.

* After the write operation is completed, reads the tx power level characteristic.

* It continuously calculated the path loss. If the path loss exceeds high threshold, it writes to alert level characteristic of immediate alert service of peripheral to start alerting.

* If the path loss drops below the low threshold, it writes to alert level characteristic of immediate alert service of peripheral to stop alerting.

If the peer does not support a required service, characteristic, or descriptor, then the peer lied when it claimed support for the Link Loss service! When this happens, or if a GATT procedure fails, this function immediately terminates the connection.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding BLE service discovery, connection, encryption and characteristic operations.

To test this demo, use any BLE GATT server app that advertises support for the Link Loss service (0x1803) and includes it in the GATT database.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C2/ESP32-C3/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Configure the Project

Open the project configuration menu: 

```bash
idf.py menuconfig
```

In the `Example Configuration` menu:

* Change the `Peer Address` option if needed.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful connection:

```
I (358) BLE_INIT: BT controller compile version [a186b41]
I (358) phy_init: phy_version 970,1856f88,May 10 2023,17:44:12
I (418) BLE_INIT: Bluetooth MAC: 7c:df:a1:66:a5:c6

I (418) NimBLE_PROX_CENT: BLE Host Task Started
I (418) NimBLE: GAP procedure initiated: stop advertising.

I (418) NimBLE: GAP procedure initiated: discovery; 
I (428) NimBLE: own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1 
I (438) NimBLE: duration=forever
I (438) NimBLE: 

I (448) main_task: Returned from app_main()
I (478) NimBLE: GAP procedure initiated: connect; 
I (478) NimBLE: peer_addr_type=0 peer_addr=
I (488) NimBLE: 84:f7:03:05:a5:f6
I (488) NimBLE:  scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=0 max_ce_len=0 own_addr_type=0
I (498) NimBLE: 

I (548) NimBLE: Connection established 
I (548) NimBLE: 

I (548) NimBLE: Connection secured

I (558) NimBLE: encryption change event; status=1288 
I (558) NimBLE: GATT procedure initiated: discover all services

I (658) NimBLE: GATT procedure initiated: discover all characteristics; 
I (658) NimBLE: start_handle=1 end_handle=4

I (858) NimBLE: GATT procedure initiated: discover all characteristics; 
I (858) NimBLE: start_handle=5 end_handle=8

I (1058) NimBLE: GATT procedure initiated: discover all characteristics; 
I (1058) NimBLE: start_handle=9 end_handle=65535

I (1258) NimBLE: GATT procedure initiated: discover all descriptors; 
I (1258) NimBLE: chr_val_handle=3 end_handle=4

I (1358) NimBLE: GATT procedure initiated: discover all descriptors; 
I (1358) NimBLE: chr_val_handle=7 end_handle=8

I (1458) NimBLE: GATT procedure initiated: discover all descriptors; 
I (1458) NimBLE: chr_val_handle=11 end_handle=65535

I (1558) NimBLE: Service discovery complete; status=0 conn_handle=1

I (1558) NimBLE: GATT procedure initiated: write; 
I (1558) NimBLE: att_handle=3 len=1
I (1658) NimBLE: Write alert level char completed; status=0 conn_handle=1

I (3707) NimBLE: Read on tx power level char completed; status=0 conn_handle=1
I (3707) NimBLE:  attr_handle=11 value=
I (3707) NimBLE: 0x80

I (5427) NimBLE: Connection handle : 1
I (5427) NimBLE: Current RSSI = -16
I (5427) NimBLE: path loss = -112 pwr lvl = -128 rssi = -16
I (5427) NimBLE: GATT procedure initiated: write no rsp; 
I (5427) NimBLE: att_handle=7 len=4

I (5437) NimBLE: Write to alert level characteristis done

I (10447) NimBLE: Connection handle : 1
I (10447) NimBLE: Current RSSI = -16
I (10447) NimBLE: path loss = -112 pwr lvl = -128 rssi = -16
I (10447) NimBLE: GATT procedure initiated: write no rsp; 
I (10457) NimBLE: att_handle=7 len=4

I (10457) NimBLE: Write to alert level characteristis done

I (15467) NimBLE: Connection handle : 1
I (15467) NimBLE: Current RSSI = -8
I (15467) NimBLE: path loss = -120 pwr lvl = -128 rssi = -8
I (15467) NimBLE: GATT procedure initiated: write no rsp; 
I (15477) NimBLE: att_handle=7 len=4

I (15477) NimBLE: Write to alert level characteristis done

I (20487) NimBLE: Connection handle : 1
I (20487) NimBLE: Current RSSI = -75
I (20487) NimBLE: path loss = -53 pwr lvl = -128 rssi = -75
I (20487) NimBLE: GATT procedure initiated: write no rsp; 
I (20497) NimBLE: att_handle=7 len=4

I (20497) NimBLE: Write to alert level characteristis done

I (25507) NimBLE: Connection handle : 1
I (25507) NimBLE: Current RSSI = -38
I (25507) NimBLE: path loss = -90 pwr lvl = -128 rssi = -38
I (25507) NimBLE: GATT procedure initiated: write no rsp; 
I (25517) NimBLE: att_handle=7 len=4

I (25517) NimBLE: Write to alert level characteristis done

I (30527) NimBLE: Connection handle : 1
I (30527) NimBLE: Current RSSI = -9
I (30527) NimBLE: path loss = -119 pwr lvl = -128 rssi = -9
I (30527) NimBLE: GATT procedure initiated: write no rsp; 
I (30537) NimBLE: att_handle=7 len=4

I (30537) NimBLE: Write to alert level characteristis done

I (35547) NimBLE: Connection handle : 1
I (35547) NimBLE: Current RSSI = -9
I (35547) NimBLE: path loss = -119 pwr lvl = -128 rssi = -9
I (35547) NimBLE: GATT procedure initiated: write no rsp; 
I (35557) NimBLE: att_handle=7 len=4

I (35557) NimBLE: Write to alert level characteristis done

I (40567) NimBLE: Connection handle : 1
I (40567) NimBLE: Current RSSI = -9
I (40567) NimBLE: path loss = -119 pwr lvl = -128 rssi = -9
I (40567) NimBLE: GATT procedure initiated: write no rsp; 
I (40577) NimBLE: att_handle=7 len=4

I (40577) NimBLE: Write to alert level characteristis done

I (45587) NimBLE: Connection handle : 1
I (45587) NimBLE: Current RSSI = -9
I (45587) NimBLE: path loss = -119 pwr lvl = -128 rssi = -9
I (45587) NimBLE: GATT procedure initiated: write no rsp; 
I (45597) NimBLE: att_handle=7 len=4

I (45597) NimBLE: Write to alert level characteristis done

I (50607) NimBLE: Connection handle : 1
I (50607) NimBLE: Current RSSI = -9
I (50607) NimBLE: path loss = -119 pwr lvl = -128 rssi = -9
I (50607) NimBLE: GATT procedure initiated: write no rsp; 
I (50617) NimBLE: att_handle=7 len=4

I (50617) NimBLE: Write to alert level characteristis done

I (8957) NimBLE: GAP procedure initiated: discovery; 
I (8967) NimBLE: own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1 
I (8977) NimBLE: duration=forever
I (8977) NimBLE: 

I (10407) NimBLE: Link lost for device with conn_handle 0
I (15407) NimBLE: Link lost for device with conn_handle 0

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
