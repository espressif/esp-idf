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
I (459) NimBLE_BLE_PHY_PRPH: BLE Host Task Started
I (459) NimBLE: Device Address:
I (459) NimBLE: 60:55:f9:f7:3e:23
I (469) NimBLE:

I (469) NimBLE: Default LE PHY set successfully
I (479) NimBLE: GAP procedure initiated: extended advertise; instance=1

I (479) uart: queue free spaces: 8
I (489) main_task: Returned from app_main()
I (599) NimBLE: connection established; status=0
I (599) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (599) NimBLE: 60:55:f9:f7:3e:23
I (599) NimBLE:  our_id_addr_type=0 our_id_addr=
I (609) NimBLE: 60:55:f9:f7:3e:23
I (609) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (619) NimBLE: 60:55:f9:f7:3e:25
I (619) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (619) NimBLE: 60:55:f9:f7:3e:25
I (629) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (639) NimBLE:

I (639) NimBLE: advertise complete; reason=0
I (1749) NimBLE: disconnect; reason=531
I (1749) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (1749) NimBLE: 60:55:f9:f7:3e:23
I (1749) NimBLE:  our_id_addr_type=0 our_id_addr=
I (1759) NimBLE: 60:55:f9:f7:3e:23
I (1759) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (1759) NimBLE: 60:55:f9:f7:3e:25
I (1769) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (1769) NimBLE: 60:55:f9:f7:3e:25
I (1779) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (1789) NimBLE:

I (1789) NimBLE: GAP procedure initiated: extended advertise; instance=1

I (1809) NimBLE: connection established; status=0
I (1809) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (1809) NimBLE: 60:55:f9:f7:3e:23
I (1819) NimBLE:  our_id_addr_type=0 our_id_addr=
I (1819) NimBLE: 60:55:f9:f7:3e:23
I (1819) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (1829) NimBLE: 60:55:f9:f7:3e:25
I (1829) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (1839) NimBLE: 60:55:f9:f7:3e:25
I (1839) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (1849) NimBLE:

I (1859) NimBLE: advertise complete; reason=0
I (3009) NimBLE: disconnect; reason=531
I (3009) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (3009) NimBLE: 60:55:f9:f7:3e:23
I (3009) NimBLE:  our_id_addr_type=0 our_id_addr=
I (3019) NimBLE: 60:55:f9:f7:3e:23
I (3019) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (3029) NimBLE: 60:55:f9:f7:3e:25
I (3029) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (3039) NimBLE: 60:55:f9:f7:3e:25
I (3039) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (3049) NimBLE:

I (3059) NimBLE: GAP procedure initiated: extended advertise; instance=1

I (3079) NimBLE: connection established; status=0
I (3079) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (3079) NimBLE: 60:55:f9:f7:3e:23
I (3079) NimBLE:  our_id_addr_type=0 our_id_addr=
I (3089) NimBLE: 60:55:f9:f7:3e:23
I (3089) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (3099) NimBLE: 60:55:f9:f7:3e:25
I (3099) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (3109) NimBLE: 60:55:f9:f7:3e:25
I (3109) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (3119) NimBLE:

I (3129) NimBLE: advertise complete; reason=0
I (4379) NimBLE: disconnect; reason=531
I (4379) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (4379) NimBLE: 60:55:f9:f7:3e:23
I (4379) NimBLE:  our_id_addr_type=0 our_id_addr=
I (4389) NimBLE: 60:55:f9:f7:3e:23
I (4389) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (4399) NimBLE: 60:55:f9:f7:3e:25
I (4399) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (4409) NimBLE: 60:55:f9:f7:3e:25
I (4409) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (7872) NimBLE:

```

## Accepting connection directly on 2M / Coded Phy

The example can also be configured to advertise directly on 2M auxillary PHY or CODED PHY

In main.c file, in bleprph_on_sync function, set the s_current_phy to the intended phy.

To advertise, using 2M Phy, set the s_current_phy to BLE_HCI_LE_PHY_2M_PREF_MASK.

To advertise, using Coded Phy, set the s_current_phy to BLE_HCI_LE_PHY_CODED_PREF_MASK

Recompile application and flash the image.

Since , in this mode, the central will initiate direct connection, ensure that peripheral is already advertising, before initiating connection from the central device.


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
