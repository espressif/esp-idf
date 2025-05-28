| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |



# BLE CSC(Cycling Speed and Cadence) Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This Bluetooth Low Energy (BLE) Cycling Speed and Cadence (CSC) profile is designed to provide real-time measurements of a cyclist's speed and cadence. It allows cycling-related devices, such as bike sensors or fitness trackers, to communicate this data wirelessly to a receiving device, such as a smartphone or cycling computer. The major tasks of the BLE CSC profile include:

Connection Establishment: This example creates GATT server and then starts advertising, waiting to be connected to a GATT client.

Service Discovery: Once the connection is established, the client device performs service discovery to identify the available services and characteristics supported by the CSC Sensor. The CSC profile typically utilizes the Generic Attribute Profile (GATT) for this purpose.

CSC Measurement: The CSC Sensor periodically sends CSC Measurement notifications to the client device. These notifications contain the current speed and cadence data measured by the sensor. The client device can subscribe to these notifications to receive real-time updates. The `blecsc_simulate_speed_and_cadence()` function is used to calculate simulated CSC measurements, including speed and cadence. 


It uses ESP's Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding CSC measurements, GATT database configuration, handling GATT reads and writes, handling subscribe events and understanding advertisement.


Note :

* To install the dependency packages needed, please refer to the top level [README file](../../../README.md#running-test-python-script-ttfw).

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

In the `Example Configuration` menu:

* Select I/O capabilities of device from `Example Configuration --> I/O Capability`, default is `Just_works`.
* Enable/Disable other security related parameters `Bonding, MITM option, secure connection(SM SC)`.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

## There is this console output when blecsc starts advertising

```
I (424) NimBLE_BLE_CSC: BLE Host Task Started
I (424) NimBLE: GAP procedure initiated: stop advertising.

I (424) NimBLE: Failed to restore IRKs from store; status=8

I (434) NimBLE: GAP procedure initiated: advertise;
I (434) NimBLE: disc_mode=2
I (434) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (444) NimBLE:

I (454) main_task: Returned from app_main()
I (1424) NimBLE: CSC simulated values: speed = 1 kph, cadence = 21

I (1524) NimBLE: CSC simulated values: speed = 2 kph, cadence = 22

I (1624) NimBLE: CSC simulated values: speed = 3 kph, cadence = 23

I (1724) NimBLE: CSC simulated values: speed = 4 kph, cadence = 24

I (1824) NimBLE: CSC simulated values: speed = 5 kph, cadence = 25

I (1924) NimBLE: CSC simulated values: speed = 6 kph, cadence = 26

I (2024) NimBLE: CSC simulated values: speed = 7 kph, cadence = 27

I (2124) NimBLE: CSC simulated values: speed = 8 kph, cadence = 28

I (2224) NimBLE: CSC simulated values: speed = 9 kph, cadence = 29

I (2324) NimBLE: CSC simulated values: speed = 10 kph, cadence = 30

```

## This is the Console output when blecsc is connected and sends notifications.

```
I (8974) NimBLE: connection established; status=0

I (9024) NimBLE: CSC simulated values: speed = 7 kph, cadence = 97

I (9124) NimBLE: CSC simulated values: speed = 8 kph, cadence = 98

I (9224) NimBLE: CSC simulated values: speed = 9 kph, cadence = 99
I (26454) NimBLE: csc measurement notify state = 1

I (26534) NimBLE: CSC simulated values: speed = 7 kph, cadence = 32

I (26534) NimBLE: GATT procedure initiated: notify;
I (26534) NimBLE: att_handle=3

I (26634) NimBLE: CSC simulated values: speed = 8 kph, cadence = 33

I (26634) NimBLE: GATT procedure initiated: notify;
I (26634) NimBLE: att_handle=3

I (26734) NimBLE: CSC simulated values: speed = 9 kph, cadence = 34

I (26734) NimBLE: GATT procedure initiated: notify;
I (26734) NimBLE: att_handle=3

I (26834) NimBLE: CSC simulated values: speed = 10 kph, cadence = 35

I (26834) NimBLE: GATT procedure initiated: notify;
I (26834) NimBLE: att_handle=3

```

## Note
* NVS support is there for bonding. So, bonding is persistent across reboot.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
