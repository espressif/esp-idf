| Supported Targets | ESP32 |
| ----------------- | ----- |

# BLE Mesh example

This example is part of upstream mynewt-nimble implementation and not maintained by Espressif.

IDF maintains ESP-BLE-MESH as the official Mesh solution. Please refer to ESP-BLE-MESH guide at:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/esp-ble-mesh/ble-mesh-index.html

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example implements Bluetooth Mesh node that supports On/Off and Level models.
It has suport for both Advertising Bearer and GATT Bearer.

For more information on NimBLE MESH, please visit [NimBLE_MESH](https://mynewt.apache.org/latest/network/mesh/index.html#bluetooth-mesh).

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.
This example can be starting step to get basic understanding on how to build BLE MESH node.
To test this demo, any BLE mesh provisioner app can be used.


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

* Select `Enable BLE mesh functionality` under `Component config --> Bluetooth --> Enable NimBLE host stack`.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

`idf.py -p PORT flash monitor`

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

There is this console output on successful BLE provisioning:
```
I (285) BTDM_INIT: BT controller compile version [8e87ec7]
I (285) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (355) phy: phy_version: 4000, b6198fa, Sep  3 2018, 15:11:06, 0, 0
GAP procedure initiated: stop advertising.
I (625) NimBLE_MESH: Bluetooth initialized

GAP procedure initiated: discovery; own_addr_type=1 filter_policy=0 passive=1 limited=0 filter_duplicates=0 duration=forever
I (895) NimBLE_MESH: Mesh initialized

GAP procedure initiated: advertise; disc_mode=0 adv_channel_map=0 own_addr_type=1 adv_filter_policy=0 adv_itvl_min=160 adv_itvl_max=160
GAP procedure initiated: stop advertising.
GAP procedure initiated: advertise; disc_mode=2 adv_channel_map=0 own_addr_type=1 adv_filter_policy=0 adv_itvl_min=160 adv_itvl_max=240
proxy_connected: conn_handle 0
GAP procedure initiated: advertise; disc_mode=2 adv_channel_map=0 own_addr_type=1 adv_filter_policy=0 adv_itvl_min=1600 adv_itvl_max=1920
proxy_complete_pdu: Mesh Provisioning PDU
prov_invite: Attention Duration: 5 seconds
GATT procedure initiated: notify; att_handle=20
proxy_complete_pdu: Mesh Provisioning PDU
prov_start: Algorithm:   0x00
prov_start: Public Key:  0x00
prov_start: Auth Method: 0x02
prov_start: Auth Action: 0x00
prov_start: Auth Size:   0x04
I (6985) NimBLE_MESH: OOB Number: 5228

proxy_complete_pdu: Mesh Provisioning PDU
prov_pub_key: Remote Public Key: f56c5d5396a4d09cf1ea52e8217eba3b881202e73d09e9c4955903d5836d51b2117176fa5887869ddd5a2985dce9f706d3e4c2729dd9d45edeb86bcbebe4721c
GATT procedure initiated: notify; att_handle=20
proxy_complete_pdu: Mesh Provisioning PDU
prov_confirm: Remote Confirm: ec7a9c169d23408abe051beca357abc1
GATT procedure initiated: notify; att_handle=20
proxy_complete_pdu: Mesh Provisioning PDU
prov_random: Remote Random: 05ca403997576097eb430588bf2b8448
GATT procedure initiated: notify; att_handle=20
proxy_complete_pdu: Mesh Provisioning PDU
GATT procedure initiated: notify; att_handle=20
bt_mesh_provision: Primary Element: 0x0002
GAP procedure initiated: stop advertising.
I (11885) NimBLE_MESH: Local node provisioned, primary address 0x0002

GAP procedure initiated: advertise; disc_mode=2 adv_channel_map=0 own_addr_type=1 adv_filter_policy=0 adv_itvl_min=1600 adv_itvl_max=1920
GAP procedure initiated: stop advertising.
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
