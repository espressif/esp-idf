# Examples

This directory contains a range of example ESP-IDF projects. These examples are intended to demonstrate parts of the ESP-IDF functionality, and to provide code that you can copy and adapt into your own projects.

## Example Layout

The examples are grouped into subdirectories by category. Each category directory contains one or more example projects:

- `bluetooth/bluedroid` Classic BT, BLE and coex examples using default Bluedroid host stack.
- `bluetooth/nimble` BLE examples using NimBLE host stack.
- `bluetooth/esp_ble_mesh` ESP BLE Mesh examples.
- `bluetooth/hci` HCI transport (VHCI and HCI UART) examples.
- `build_system` Examples of build system features.
- `cxx` C++ language utilization examples and experimental components.
- `ethernet` Ethernet network examples.
- `get-started` Simple examples with minimal functionality. Good start point for beginners.
- `mesh` Wi-Fi Mesh examples.
- `network` Examples related to general network environment, test & analysis.
- `openthread` OpenThread examples.
- `peripherals` Examples showing driver functionality for the various onboard ESP32 peripherals.
- `protocols` Examples showing network protocol interactions.
- `provisioning` Wi-Fi provisioning examples.
- `security` Examples about security features.
- `storage` Examples showing data storage methods using SPI flash, external storage like the SD/MMC interface and flash partitioning.
- `system` Demonstrates some internal chip features, or debugging & development tools.
- `wifi` Advanced Wi-Fi features (For network protocol examples, see `protocols` instead.)
- `Zigbee` Zigbee network and device examples.

In addition to these examples, `commmon_components` directory contains code shared by several examples.

## Using Examples

Before building an example, be sure to follow the [ESP-IDF Getting Started Guide](https://idf.espressif.com/) to ensure you have the required development environment.

Building an example is the same as building any other project:

- Change into the directory of the new example you'd like to build.
- Run `idf.py set-target TARGET` to select the correct chip target to build before opening the project configuration menu. By default the target is `esp32`. For all options see `idf.py set-target --help`
- Run `idf.py menuconfig` to open the project configuration menu. Most examples have a project-specific "Example Configuration" section here (for example, to set the WiFi SSID & password to use).
- `idf.py build` to build the example.
- Follow the printed instructions to flash, or run `idf.py -p PORT flash`.

## Running Test Python Script (ttfw)

Some of the examples have `..._test.py` scripts that are used to test that the example works as expected. These scripts run automatically in the internal test queue. They are not intended to be run by ESP-IDF users but sometimes you may want to run them locally.

Install Python dependencies and export the Python path where the IDF CI Python modules are found with the following commands:

```bash
bash install.sh --enable-ttfw
source export.sh
export PYTHONPATH=$IDF_PATH/tools/ci/python_packages:$IDF_PATH/tools:$PYTHONPATH
```

## Running Test Python Script (pytest)

Some of the examples have `pytest_....py` scripts that are using the `pytest` as the test framework. For detailed information, please refer to the "Run the Tests Locally" Section under [ESP-IDF tests in Pytest documentation](../docs/en/contribute/esp-idf-tests-with-pytest.rst)

Using `pytest` is the recommended way to write new tests. We will migrate all the example test scripts to this new framework soon.

## Copying Examples

Each example is a standalone project. The examples _do not have to be inside the esp-idf directory_. You can copy an example directory to anywhere on your computer in order to make a copy that you can modify and work with.

The `IDF_PATH` environment variable is the only thing that connects the example to the rest of ESP-IDF.

If you're looking for a more bare-bones project to start from, try [esp-idf-template](https://github.com/espressif/esp-idf-template).

## Contributing Examples

If you have a new example you think we'd like, please consider sending it to us as a Pull Request.

In the ESP-IDF documentation, you can find a ["Creating Examples" page](../docs/en/contribute/creating-examples.rst) which lays out the steps to creating a top quality example.
