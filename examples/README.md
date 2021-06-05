# Examples

This directory contains a range of example ESP-IDF projects. These examples are intended to demonstrate parts of the ESP-IDF functionality, and to provide code that you can copy and adapt into your own projects.

# Example Layout

The examples are grouped into subdirectories by category. Each category directory contains one or more example projects:

* `bluetooth/bluedroid` Classic BT, BLE and coex examples using default Bluedroid host stack.
* `bluetooth/nimble` BLE examples using NimBLE host stack.
* `bluetooth/esp_ble_mesh` ESP BLE Mesh examples.
* `bluetooth/hci` HCI transport (VHCI and HCI UART) examples.
* `build_system` Examples of build system features.
* `cxx` C++ language utilization examples and experimental components.
* `ethernet` Ethernet network examples.
* `get-started` Simple examples with minimal functionality. Good start point for beginners.
* `mesh` Wi-Fi Mesh examples.
* `openthread` OpenThread examples.
* `peripherals` Examples showing driver functionality for the various onboard ESP32 peripherals.
* `protocols` Examples showing network protocol interactions.
* `provisioning` Wi-Fi provisioning examples.
* `security` Examples about security features.
* `storage` Examples showing data storage methods using SPI flash, external storage like the SD/MMC interface and flash partitioning.
* `system` Demonstrates some internal chip features, or debugging & development tools.
* `wifi` Advanced Wi-Fi features (For network protocol examples, see `protocols` instead.)

In addition to these examples, `commmon_components` directory contains code shared by several examples.

# Using Examples

Before building an example, be sure to follow the Getting Started guide to ensure you have the required development environment.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-S2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-C3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html)


Building an example is the same as building any other project:

* Change into the directory of the new example you'd like to build.
* Run `idf.py set-target TARGET` to select the correct chip target to build before opening the project configuration menu. By default the target is `esp32` and the options are `esp32`, `esp32s2` and `esp32c3`.
* Run `idf.py menuconfig` to open the project configuration menu. Most examples have a project-specific "Example Configuration" section here (for example, to set the WiFi SSID & password to use).
* `idf.py build` to build the example.
* Follow the printed instructions to flash, or run `idf.py -p PORT flash`.

# Copying Examples

Each example is a standalone project. The examples *do not have to be inside the esp-idf directory*. You can copy an example directory to anywhere on your computer in order to make a copy that you can modify and work with.

The `IDF_PATH` environment variable is the only thing that connects the example to the rest of ESP-IDF.

If you're looking for a more bare-bones project to start from, try [esp-idf-template](https://github.com/espressif/esp-idf-template).

# Contributing Examples

If you have a new example you think we'd like, please consider sending it to us as a Pull Request.

In the ESP-IDF documentation, you can find a "Creating Examples" page which lays out the steps to creating a top quality example.
