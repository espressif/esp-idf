| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- |

# BLE Central PHY Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example performs below functionalities:

* Establishes a connection on LE 1M PHY and switch to LE 2M PHY once connection is established. Then perform GATT read operation against the specified peer. Disconnect the link once this is completed.

* Change the default LE PHY to 2M/Coded and establish a connection on that PHY. Then perform GATT read operation against the specified peer. Disconnect the link once this is completed.

* GATT operations performed:
  1. Read on LE PHY characteristic.
  2. Write blob on LE PHY characteristic.

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
I (456) NimBLE_BLE_PHY_CENT: BLE Host Task Started
I (456) NimBLE: Default LE PHY set successfully; tx_phy = 7, rx_phy = 7
I (456) NimBLE: GAP procedure initiated: extended discovery;

I (466) main_task: Returned from app_main()
I (6316) NimBLE: Attempting to connect to : 60 55 f9 f7 3e 23 with type 0

I (6316) NimBLE: GAP procedure initiated: extended connect;

I (6386) NimBLE: Connection established 1M PhY
I (6386) NimBLE:

I (6386) NimBLE: GATT procedure initiated: discover all services

I (6446) NimBLE: GATT procedure initiated: discover all characteristics;
I (6446) NimBLE: start_handle=1 end_handle=5

I (6636) NimBLE: GATT procedure initiated: discover all characteristics;
I (6636) NimBLE: start_handle=6 end_handle=9

I (6836) NimBLE: GATT procedure initiated: discover all characteristics;
I (6836) NimBLE: start_handle=10 end_handle=65535

I (7036) NimBLE: GATT procedure initiated: discover all descriptors;
I (7036) NimBLE: chr_val_handle=8 end_handle=9

I (7136) NimBLE: GATT procedure initiated: discover all descriptors;
I (7136) NimBLE: chr_val_handle=12 end_handle=65535

I (7236) NimBLE: Service discovery complete; status=0 conn_handle=1

I (7236) NimBLE: GATT procedure initiated: read;
I (7236) NimBLE: att_handle=12

I (7336) NimBLE: Read complete; status=261 conn_handle=1
I (7336) NimBLE:

I (7336) NimBLE: GATT procedure initiated: write long;
I (7346) NimBLE: att_handle=12 len=1000

I (7436) NimBLE: Write complete; status=261 conn_handle=1
I (7436) NimBLE: GAP procedure initiated: terminate connection; conn_handle=1 hci_reason=19

I (7446) NimBLE:

I (7486) NimBLE: disconnect; reason=534
I (7486) NimBLE:

I (7486) NimBLE:  Attempting to initiate connection on 2M PHY

I (7496) NimBLE: GAP procedure initiated: extended connect;

I (7596) NimBLE: Connection established on 2M Phy
I (7596) NimBLE:

I (7596) NimBLE: GATT procedure initiated: discover all services

I (7696) NimBLE: GATT procedure initiated: discover all characteristics;
I (7706) NimBLE: start_handle=1 end_handle=5

I (7896) NimBLE: GATT procedure initiated: discover all characteristics;
I (7896) NimBLE: start_handle=6 end_handle=9

I (8096) NimBLE: GATT procedure initiated: discover all characteristics;
I (8096) NimBLE: start_handle=10 end_handle=65535

I (8296) NimBLE: GATT procedure initiated: discover all descriptors;
I (8296) NimBLE: chr_val_handle=8 end_handle=9

I (8396) NimBLE: GATT procedure initiated: discover all descriptors;
I (8396) NimBLE: chr_val_handle=12 end_handle=65535

I (8496) NimBLE: Service discovery complete; status=0 conn_handle=1

I (8496) NimBLE: GATT procedure initiated: read;
I (8506) NimBLE: att_handle=12

I (8596) NimBLE: Read complete; status=261 conn_handle=1
I (8596) NimBLE:

I (8606) NimBLE: GATT procedure initiated: write long;
I (8606) NimBLE: att_handle=12 len=1000

I (8696) NimBLE: Write complete; status=261 conn_handle=1
I (8696) NimBLE: GAP procedure initiated: terminate connection; conn_handle=1 hci_reason=19

I (8706) NimBLE:

I (8746) NimBLE: disconnect; reason=534
I (8746) NimBLE:

I (8756) NimBLE:  Attempting to initiate connection on Coded PHY

I (8756) NimBLE: GAP procedure initiated: extended connect;

I (8876) NimBLE: Connection established on Coded Phy
I (8876) NimBLE:

I (8876) NimBLE: GATT procedure initiated: discover all services

I (8976) NimBLE: GATT procedure initiated: discover all characteristics;
I (8976) NimBLE: start_handle=1 end_handle=5

I (9176) NimBLE: GATT procedure initiated: discover all characteristics;
I (9176) NimBLE: start_handle=6 end_handle=9

I (9426) NimBLE: GATT procedure initiated: discover all characteristics;
I (9426) NimBLE: start_handle=10 end_handle=65535

I (9626) NimBLE: GATT procedure initiated: discover all descriptors;
I (9626) NimBLE: chr_val_handle=8 end_handle=9

I (9776) NimBLE: GATT procedure initiated: discover all descriptors;
I (9776) NimBLE: chr_val_handle=12 end_handle=65535

I (9876) NimBLE: Service discovery complete; status=0 conn_handle=1

I (9876) NimBLE: GATT procedure initiated: read;
I (9876) NimBLE: att_handle=12

I (9976) NimBLE: Read complete; status=261 conn_handle=1
I (9976) NimBLE:

I (9976) NimBLE: GATT procedure initiated: write long;
I (9976) NimBLE: att_handle=12 len=1000

I (10076) NimBLE: Write complete; status=261 conn_handle=1
I (10076) NimBLE: GAP procedure initiated: terminate connection; conn_handle=1 hci_reason=19

I (10076) NimBLE:

I (10126) NimBLE: disconnect; reason=534
I (10126) NimBLE:

```

## Intiating connection on 2M / Coded Phy

This example also provides a way to initiate connection directly on 2M / Coded PHY. For this, set the peripheral device address in EXAMPLE_PEER_ADDR setting via menuconfig. Also example needs to be modified to use the PHY that user needs to update.

In main.c file, in blecent_on_sync function, set the s_current_phy to the intended phy.

To create direct connection on 2M, set the s_current_phy to BLE_HCI_LE_PHY_2M_PREF_MASK.

To create direct connection on Coded Phy, set the s_current_phy to BLE_HCI_LE_PHY_CODED_PREF_MASK

Recompile application and flash the image.

Note: For this setting to work ,the remote device too should be advertising with either 2M auxillary phy OR Coded phy.

Also, since this mode initiates direct connection, ensure that peripheral is already advertising, before executing this application.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
