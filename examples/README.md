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
- `ieee802154` IEEE802.15.4 examples.
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

Before building an example, follow the [ESP-IDF Getting Started](https://idf.espressif.com/) to ensure you have the required development environment.

### Set Chip Target

First of all, your target must be supported by both:

- **By your ESP-IDF version**: For the full list of supported targets, run:
  ```
  idf.py --list-targets
  ```
- **By this example**: For the full list of supported targets,  refer to the supported targets table at the top of this README.

After you make sure that your target is supported, go to your example project directory and [set the chip target](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/tools/idf-py.html#select-the-target-chip-set-target):

```
idf.py set-target <target>
```

For example, to set esp32 as the chip target, run:

```
idf.py set-target esp32
```


### Configure the Project

For information about Kconfig options, see [Project Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/kconfig.html) > ESP Timer (High Resolution Timer).

To conveniently check or modify Kconfig options for this example in a project configuration menu, run:

```
idf.py menuconfig
```


### Build and Flash

Execute the following command to build the project, flash it to your development board, and run the monitor tool to view the serial output:

```
idf.py build flash monitor
```

This command can be reduced to `idf.py flash monitor`.

If the above command fails, check the log on the serial monitor which usually provides information on the possible cause of the issue.

To exit the serial monitor, use `Ctrl` + `]`.


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
