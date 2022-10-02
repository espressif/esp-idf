| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# BLE Central PHY Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example performs below functionalities:

* Establishes a connection on LE 1M PHY and switch to LE 2M PHY once connection is established. Then perform GATT read operation against the specified peer. Disconnect the link once this is completed.

* Change the default LE PHY to 2M/Coded and establish a connection on that PHY. Then perform GATT read operation against the specified peer. Disconnect the link once this is completed.

This example aims at understanding how to establish connections on preferred PHY and changing LE PHY once the connection is established.

To test this demo, use any BLE GATT server app that advertises support for the LE PHY service (0xABF2) and includes it in the GATT database. Also make sure device supports extended advertising.

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
I (315) BTDM_INIT: BT controller compile version [05195c9]
I (315) phy_init: phy_version 912,d001756,Jun  2 2022,16:28:07
I (355) system_api: Base MAC address is not set
I (355) system_api: read default base MAC address from EFUSE
I (355) BTDM_INIT: Bluetooth MAC: 84:f7:03:08:4d:8e

I (355) NimBLE_BLE_PHY_CENT: BLE Host Task Started
I (465) NimBLE: Connection established
I (465) NimBLE:

I (465) NimBLE: Prefered LE PHY set to LE_PHY_2M successfully
I (465) NimBLE: GATT procedure initiated: discover all services

I (565) NimBLE: GATT procedure initiated: discover all characteristics;
I (565) NimBLE: start_handle=1 end_handle=5

I (765) NimBLE: GATT procedure initiated: discover all characteristics;
I (765) NimBLE: start_handle=6 end_handle=9

I (965) NimBLE: GATT procedure initiated: discover all characteristics;
I (965) NimBLE: start_handle=10 end_handle=65535

I (1015) NimBLE: LE PHY Update completed; status=0 conn_handle=1 tx_phy=2 rx_phy = 2

I (1165) NimBLE: GATT procedure initiated: discover all descriptors;
I (1165) NimBLE: chr_val_handle=8 end_handle=9

I (1265) NimBLE: GATT procedure initiated: discover all descriptors;
I (1265) NimBLE: chr_val_handle=12 end_handle=65535

I (1365) NimBLE: Service discovery complete; status=0 conn_handle=1

I (1365) NimBLE: GATT procedure initiated: read;
I (1365) NimBLE: att_handle=12

I (1375) NimBLE: GAP procedure initiated: terminate connection; conn_handle=1 hci_reason=19

I (1415) NimBLE: disconnect; reason=534
I (1415) NimBLE:

I (1415) NimBLE: Default LE PHY set successfully; tx_phy = 2, rx_phy = 2
I (1505) NimBLE: Connection established
I (1505) NimBLE:

I (1505) NimBLE: GATT procedure initiated: discover all services

I (1615) NimBLE: GATT procedure initiated: discover all characteristics;
I (1615) NimBLE: start_handle=1 end_handle=5

I (1815) NimBLE: GATT procedure initiated: discover all characteristics;
I (1815) NimBLE: start_handle=6 end_handle=9

I (2015) NimBLE: LE PHY Update completed; status=0 conn_handle=1 tx_phy=2 rx_phy = 2

I (2015) NimBLE: GATT procedure initiated: discover all characteristics;
I (2025) NimBLE: start_handle=10 end_handle=65535

I (2215) NimBLE: GATT procedure initiated: discover all descriptors;
I (2215) NimBLE: chr_val_handle=8 end_handle=9

I (2315) NimBLE: GATT procedure initiated: discover all descriptors;
I (2315) NimBLE: chr_val_handle=12 end_handle=65535

I (2415) NimBLE: Service discovery complete; status=0 conn_handle=1

I (2415) NimBLE: GATT procedure initiated: read;
I (2415) NimBLE: att_handle=12

I (2425) NimBLE: GAP procedure initiated: terminate connection; conn_handle=1 hci_reason=19

I (2465) NimBLE: disconnect; reason=534
I (2465) NimBLE:

I (2465) NimBLE: Default LE PHY set successfully; tx_phy = 4, rx_phy = 4
I (2555) NimBLE: Connection established
I (2555) NimBLE:

I (2555) NimBLE: GATT procedure initiated: discover all services

I (2665) NimBLE: GATT procedure initiated: discover all characteristics;
I (2665) NimBLE: start_handle=1 end_handle=5

I (2865) NimBLE: GATT procedure initiated: discover all characteristics;
I (2865) NimBLE: start_handle=6 end_handle=9

I (3065) NimBLE: LE PHY Update completed; status=0 conn_handle=1 tx_phy=3 rx_phy = 3

I (3065) NimBLE: GATT procedure initiated: discover all characteristics;
I (3075) NimBLE: start_handle=10 end_handle=65535

I (3265) NimBLE: GATT procedure initiated: discover all descriptors;
I (3265) NimBLE: chr_val_handle=8 end_handle=9

I (3365) NimBLE: GATT procedure initiated: discover all descriptors;
I (3365) NimBLE: chr_val_handle=12 end_handle=65535

I (3465) NimBLE: Service discovery complete; status=0 conn_handle=1

I (3465) NimBLE: GATT procedure initiated: read;
I (3465) NimBLE: att_handle=12

I (3475) NimBLE: GAP procedure initiated: terminate connection; conn_handle=1 hci_reason=19

I (3515) NimBLE: disconnect; reason=534
I (3515) NimBLE:

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
