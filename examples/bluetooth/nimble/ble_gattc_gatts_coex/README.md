| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# BLE GATTC GATTS Coex Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This is a simple demo example to showcase a single code which exhibits functionality of both GATT client and GATT server. This example creates a GATT client and performs passive scan to connect to a peripheral device if the device advertises connectability and the device advertises support for the Alert Notification service (0x1811) as primary service UUID. This example also creates a GATT server and advertises, so a remote device can connect to it. If the device is not found within 120 seconds, the example will stop scanning. Upon disconnection in any role, the example ONLY advertises and accepts incoming connections.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

To test this demo, you can run a bleprph example first where gatt client connects to bleprph. Once a disconnect happens, you can use any third party phone and initiate a BLE connection. The same ble_gattc_gatts_coex example will accept incoming connection request. The client is free to perform GATT operations on this connection.

The test demo example is only to show how to use the API. The actual ble coex stability is not the criteria for this demo example.

Note :

* To install the dependency packages needed, please refer to the top level [README file](../../../README.md#running-test-python-script-pytest).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32 series SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Configure the Project

Open the project configuration menu:

```
### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful connection:

```
I (439) NimBLE: GAP procedure initiated: advertise;
I (439) NimBLE: disc_mode=2
I (449) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (459) NimBLE:

I (459) NimBLE: GAP procedure initiated: discovery;
I (459) NimBLE: own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1
I (469) NimBLE: duration=120000ms
I (469) NimBLE:

I (479) main_task: Returned from app_main()
I (489) NimBLE: GAP procedure initiated: connect;
I (489) NimBLE: peer_addr_type=0 peer_addr=
I (489) NimBLE: 84:f7:03:05:a5:f6
I (489) NimBLE:  scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=0 max_ce_len=0 own_addr_type=0
I (509) NimBLE:

I (629) NimBLE: Client connection established; status=0
I (629) NimBLE:

I (629) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (629) NimBLE: 84:f7:03:05:a5:f6
I (639) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (639) NimBLE: 84:f7:03:05:a5:f6
I (639) NimBLE: GATT procedure initiated: discover all services

I (829) NimBLE: GATT procedure initiated: discover all characteristics;
I (829) NimBLE: start_handle=1 end_handle=5

I (1029) NimBLE: GATT procedure initiated: discover all characteristics;
I (1029) NimBLE: start_handle=6 end_handle=13

I (1229) NimBLE: GATT procedure initiated: discover all characteristics;
I (1229) NimBLE: start_handle=14 end_handle=26

I (1529) NimBLE: GATT procedure initiated: discover all characteristics;
I (1529) NimBLE: start_handle=27 end_handle=65535

I (1729) NimBLE: GATT procedure initiated: discover all descriptors;
I (1729) NimBLE: chr_val_handle=8 end_handle=9

I (1829) NimBLE: GATT procedure initiated: discover all descriptors;
I (1829) NimBLE: chr_val_handle=18 end_handle=19

I (1929) NimBLE: GATT procedure initiated: discover all descriptors;
I (1929) NimBLE: chr_val_handle=23 end_handle=24

I (2029) NimBLE: GATT procedure initiated: discover all descriptors;
I (2029) NimBLE: chr_val_handle=29 end_handle=65535

I (2329) NimBLE: Service discovery complete; status=0 conn_handle=3

I (2329) NimBLE: GATT procedure initiated: read;
I (2329) NimBLE: att_handle=16

I (2429) NimBLE: Read complete; status=0 conn_handle=3
I (2429) NimBLE:  attr_handle=16 value=
I (2429) NimBLE: 0x00
I (2429) NimBLE:

I (2429) NimBLE: GATT procedure initiated: write;
I (2439) NimBLE: att_handle=26 len=2

I (2529) NimBLE: Write complete; status=270 conn_handle=3 attr_handle=26

I (2529) NimBLE: GATT procedure initiated: write;
I (2529) NimBLE: att_handle=24 len=2

I (2629) NimBLE: Subscribe complete; status=0 conn_handle=3 attr_handle=24

I (2629) NimBLE: GAP procedure initiated: terminate connection; conn_handle=3 hci_reason=19

I (2679) NimBLE: Disconnect

I (2679) NimBLE: GAP procedure initiated: stop advertising.

I (2679) NimBLE: GAP procedure initiated: advertise;
I (2679) NimBLE: disc_mode=2
I (2689) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (2699) NimBLE:

I (5329) NimBLE: Server connection established; status=0
I (5329) NimBLE:

I (5329) NimBLE:  peer_ota_addr_type=1 peer_ota_addr=
I (5329) NimBLE: 4c:4a:07:8d:4a:15
I (5329) NimBLE:  peer_id_addr_type=1 peer_id_addr=
I (5339) NimBLE: 4c:4a:07:8d:4a:15
I (17429) NimBLE: subscribe event; conn_handle=1 attr_handle=29 reason=1 prevn=0 curn=1 previ=0 curi=0
```


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
