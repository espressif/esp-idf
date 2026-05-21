| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

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
I (...) NimBLE_BLE_PHY_PRPH: BLE Host Task Started
I (...) NimBLE: Device Address:
I (...) NimBLE: xx:xx:xx:xx:xx:xx
I (...) NimBLE:
I (...) NimBLE: Default LE PHY set successfully
I (...) NimBLE: GAP procedure initiated: extended advertise; instance=0
I (...) uart: queue free spaces: 8
I (...) main_task: Returned from app_main()

I (...) NimBLE: advertise complete; reason=0
I (...) NimBLE: connection established; status=0
I (...) NimBLE: handle=0 ... conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0
I (...) NimBLE:

I (...) NimBLE: disconnect; reason=531
I (...) NimBLE: handle=0 ... conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0
I (...) NimBLE:

I (...) NimBLE: GAP procedure initiated: extended advertise; instance=0
I (...) NimBLE: advertise complete; reason=0
I (...) NimBLE: connection established; status=0
I (...) NimBLE: ...
I (...) NimBLE: disconnect; reason=531
I (...) NimBLE: ...
I (...) NimBLE: GAP procedure initiated: extended advertise; instance=0
I (...) NimBLE: ...

```

## Accepting connection directly on 2M / Coded Phy

The example can also be configured to advertise directly on 2M auxiliary PHY or CODED PHY

In main.c file, in bleprph_on_sync function, set the s_current_phy to the intended phy.

To advertise, using 2M Phy, set the s_current_phy to BLE_HCI_LE_PHY_2M_PREF_MASK.

To advertise, using Coded Phy, set the s_current_phy to BLE_HCI_LE_PHY_CODED_PREF_MASK

Recompile application and flash the image.

Since , in this mode, the central will initiate direct connection, ensure that peripheral is already advertising, before initiating connection from the central device.


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
