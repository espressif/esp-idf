| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- |

# BLE Peripheral PHY Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example performs advertises on different PHY, accepts connection from client app and gets disconnected. It then advertises on different PHY and does the same.

This example aims at understanding how to establish connections on preferred PHY and changing LE PHY once the connection is established.

To test this demo, use any BLE GATT client app that supports extended advertising.

Note :

* Make sure to run `python -m pip install --user -r $IDF_PATH/requirements.txt -r $IDF_PATH/tools/ble/requirements.txt` to install the dependency packages needed.
* Currently this Python utility is only supported on Linux (BLE communication is via BLuez + DBus).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32-C3 SoC (e.g., ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Configure the Project

Open the project configuration menu:

```bash
idf.py menuconfig
```

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful connection:

```
I (322) BTDM_INIT: BT controller compile version [05195c9]
I (322) phy_init: phy_version 912,d001756,Jun  2 2022,16:28:07
I (362) system_api: Base MAC address is not set
I (362) system_api: read default base MAC address from EFUSE
I (362) BTDM_INIT: Bluetooth MAC: 84:f7:03:05:a5:f6

I (362) NimBLE_BLE_PHY_PRPH: BLE Host Task Started
I (372) NimBLE: Device Address:
I (372) NimBLE: 84:f7:03:05:a5:f6
I (382) NimBLE:

I (382) uart: queue free spaces: 8
I (4782) NimBLE: connection established; status=0
I (4782) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (4782) NimBLE: 84:f7:03:05:a5:f6
I (4782) NimBLE:  our_id_addr_type=0 our_id_addr=
I (4792) NimBLE: 84:f7:03:05:a5:f6
I (4792) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (4802) NimBLE: 84:f7:03:08:4d:8e
I (4802) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (4812) NimBLE: 84:f7:03:08:4d:8e
I (4812) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (4822) NimBLE:

I (4822) NimBLE: advertise complete; reason=0
I (4832) NimBLE: LE PHY Update completed; status=0 conn_handle=1 tx_phy=1 rx_phy = 0

I (5322) NimBLE: LE PHY Update completed; status=0 conn_handle=1 tx_phy=2 rx_phy = 2

I (5732) NimBLE: disconnect; reason=531
I (5732) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (5732) NimBLE: 84:f7:03:05:a5:f6
I (5732) NimBLE:  our_id_addr_type=0 our_id_addr=
I (5742) NimBLE: 84:f7:03:05:a5:f6
I (5742) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (5752) NimBLE: 84:f7:03:08:4d:8e
I (5752) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (5762) NimBLE: 84:f7:03:08:4d:8e
I (5762) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (5772) NimBLE:

I (5772) NimBLE: Default LE PHY set successfully
I (5822) NimBLE: connection established; status=0
I (5822) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (5822) NimBLE: 84:f7:03:05:a5:f6
I (5822) NimBLE:  our_id_addr_type=0 our_id_addr=
I (5832) NimBLE: 84:f7:03:05:a5:f6
I (5832) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (5842) NimBLE: 84:f7:03:08:4d:8e
I (5842) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (5852) NimBLE: 84:f7:03:08:4d:8e
I (5852) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (5862) NimBLE:

I (5862) NimBLE: advertise complete; reason=0
I (5872) NimBLE: LE PHY Update completed; status=0 conn_handle=1 tx_phy=1 rx_phy = 0

I (6322) NimBLE: LE PHY Update completed; status=0 conn_handle=1 tx_phy=2 rx_phy = 2

I (6782) NimBLE: disconnect; reason=531
I (6782) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (6782) NimBLE: 84:f7:03:05:a5:f6
I (6782) NimBLE:  our_id_addr_type=0 our_id_addr=
I (6792) NimBLE: 84:f7:03:05:a5:f6
I (6792) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (6802) NimBLE: 84:f7:03:08:4d:8e
I (6802) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (6812) NimBLE: 84:f7:03:08:4d:8e
I (6812) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (6822) NimBLE:

I (6822) NimBLE: Default LE PHY set successfully
I (6872) NimBLE: connection established; status=0
I (6872) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (6872) NimBLE: 84:f7:03:05:a5:f6
I (6872) NimBLE:  our_id_addr_type=0 our_id_addr=
I (6882) NimBLE: 84:f7:03:05:a5:f6
I (6882) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (6892) NimBLE: 84:f7:03:08:4d:8e
I (6892) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (6902) NimBLE: 84:f7:03:08:4d:8e
I (6902) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (6912) NimBLE:

I (6912) NimBLE: advertise complete; reason=0
I (6922) NimBLE: LE PHY Update completed; status=0 conn_handle=1 tx_phy=1 rx_phy = 0

I (7372) NimBLE: LE PHY Update completed; status=0 conn_handle=1 tx_phy=3 rx_phy = 3

I (7832) NimBLE: disconnect; reason=531
I (7832) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (7832) NimBLE: 84:f7:03:05:a5:f6
I (7832) NimBLE:  our_id_addr_type=0 our_id_addr=
I (7842) NimBLE: 84:f7:03:05:a5:f6
I (7842) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (7852) NimBLE: 84:f7:03:08:4d:8e
I (7852) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (7862) NimBLE: 84:f7:03:08:4d:8e
I (7862) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (7872) NimBLE:


```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
