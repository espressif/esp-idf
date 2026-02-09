| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

# BLE Channel sounding example
(See the README.md file in the upper-level 'examples' directory for more information about examples.)
* This example demonstrates the capability of the CS procedure to be executed on the ESP host + external controller.
* It is important to note that the current example does not provide support for distance calculation, and it is currently under development.

## Example output

I (372) hal_uart: set baud_rate:115200.

I (382) NimBLE_RAS_INITIATOR: BLE Host Task Started
I (382) main_task: Returned from app_main()

I (432) NimBLE: GAP procedure initiated: extended discovery;

I (157332) NimBLE: GAP procedure initiated: extended connect;
I (157552) NimBLE: Connection established
I (157562) NimBLE: Connection secured
I (162222) NimBLE: encryption change event; status=0

I (162372) NimBLE: CS capabilities exchanged
I (162372) NimBLE: Set default CS settings
I (162392) NimBLE: create CS config
I (162392) NimBLE: Setup phase completed

## Note:
* This example currently requires an external Bluetooth controller supporting BLE Channel sounding functionality,as the ESP chips listed above do not have native controller support for BLE channel sounding feature and is under development phase

* To install the dependency packages needed, please refer to the top level [README file](../../../README.md#running-test-python-script-pytest).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C2/ESP32-C3/ESP32-S3/ESP32-C6 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Configure the Project

Open the project configuration menu:

```bash
idf.py menuconfig
```

In the `Example Configuration` menu:

* Change the `Peer Address` option if needed.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
