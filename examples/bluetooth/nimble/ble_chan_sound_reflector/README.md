| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

# BLE Channel Sounding reflector Example

(See the README.md file in the upper-level 'examples' directory for more information about examples.)
* This example demonstrates the capability of the CS procedure to be executed on the ESP host + external controller.
* It is important to note that the current example does not provide support for distance calculation, and it is currently under development.
* Ble channel sounding reflector example uses the RAS service defined in [ble_svc_ras.c](../../../../components/bt/host/nimble/nimble/nimble/host/services/ras/src/ble_svc_ras.c)

I (372) hal_uart: set baud_rate:115200.

I (382) NimBLE_RAS_INITIATOR: BLE Host Task Started
I (382) main_task: Returned from app_main()

I (157562) NimBLE: GAP procedure initiated: extended advertise; instance=0

I (157562) NimBLE: Connection secured
I (162222) NimBLE: encryption change event; status=0

I (162372) NimBLE: CS capabilities exchanged
I (162372) NimBLE: Set default CS settings

I (162392) NimBLE: Setup phase completed

To test this demo, any BLE channel sounding initiator app can be used.

## Note

* This example currently requires an external Bluetooth controller supporting BLE Channel sounding functionality, as the ESP chips listed above do not have native controller support for BLE channel sounding feature and is under development phase

* To install the dependency packages needed, please refer to the top level [README file](../../../README.md#running-test-python-script-pytest).

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


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
