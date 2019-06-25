# BLE peripheral example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example creates GATT server and then starts advertising, waiting to be connected to a GATT client.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding GATT database configuration, advertisement and SMP related NimBLE APIs.

It also demonstrates security features of NimBLE stack. SMP parameters like I/O capabilities of device, Bonding flag, MITM protection flag and Secure Connection only mode etc., can be configured through menuconfig options.

To test this demo, any BLE scanner app can be used.


## How to use example

### Configure the project

```
make menuconfig
```

* Set serial port under Serial Flasher Options.

* Select I/O capabilities of device from 'Example Configuration > I/O Capability', default is 'Just_works'.

* Enable/Disable other security related parameters 'Bonding, MITM option, secure connection(SM SC)' from 'Example Configuration'.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
make -j4 flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

There is this console output when bleprph is connected and characteristic is read:

```

I (118) BTDM_INIT: BT controller compile version [fe7ced0]
I (118) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
W (128) phy_init: failed to load RF calibration data (0xffffffff), falling back to full calibration
I (268) phy: phy_version: 4100, 6fa5e27, Jan 25 2019, 17:02:06, 0, 2
I (508) NimBLE_BLE_PRPH: BLE Host Task Started
I (508) uart: queue free spaces: 8
GAP procedure initiated: stop advertising.
Device Address: xx:xx:xx:xx:xx:xx
GAP procedure initiated: advertise; disc_mode=2 adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
connection established; status=0 handle=0 our_ota_addr_type=0 our_ota_addr=xx:xx:xx:xx:xx:xx our_id_addr_type=0 our_id_addr=xx:xx:xx:xx:xx:xx peer_ota_addr_type=1 peer_ota_addr=xx:xx:xx:xx:xx:xx peer_id_addr_type=1 peer_id_addr=xx:xx:xx:xx:xx:xx conn_itvl=39 conn_latency=0 supervision_timeout=500 encrypted=0 authenticated=0 bonded=0

connection updated; status=0 handle=0 our_ota_addr_type=0 our_ota_addr=xx:xx:xx:xx:xx:xx our_id_addr_type=0 our_id_addr=xx:xx:xx:xx:xx:xx peer_ota_addr_type=1 peer_ota_addr=xx:xx:xx:xx:xx:xx peer_id_addr_type=1 peer_id_addr=xx:xx:xx:xx:xx:xx conn_itvl=6 conn_latency=0 supervision_timeout=500 encrypted=0 authenticated=0 bonded=0

I (50888) NimBLE_BLE_PRPH: PASSKEY_ACTION_EVENT started

I (50888) NimBLE_BLE_PRPH: Passkey on device's display: xxxxxx
I (50888) NimBLE_BLE_PRPH: Accept or reject the passkey through console in this format -> key Y or key N
key Y
I (50898) NimBLE_BLE_PRPH: ble_sm_inject_io result: 0

encryption change event; status=0 handle=0 our_ota_addr_type=0 our_ota_addr=xx:xx:xx:xx:xx:xx our_id_addr_type=0 our_id_addr=xx:xx:xx:xx:xx:xx peer_ota_addr_type=1 peer_ota_addr=xx:xx:xx:xx:xx:xx peer_id_addr_type=1
peer_id_addr=xx:xx:xx:xx:xx:xx conn_itvl=6 conn_latency=0 supervision_timeout=500 encrypted=1 authenticated=1 bonded=1

connection updated; status=0 handle=0 our_ota_addr_type=0 our_ota_addr=xx:xx:xx:xx:xx:xx our_id_addr_type=0 our_id_addr=xx:xx:xx:xx:xx:xx
peer_ota_addr_type=1 peer_ota_addr=xx:xx:xx:xx:xx:xx peer_id_addr_type=1 peer_id_addr=xx:xx:xx:xx:xx:xx conn_itvl=39 conn_latency=0 supervision_timeout=500 encrypted=1 authenticated=1 bonded=1

```

