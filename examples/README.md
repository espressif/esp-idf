# Examples

This directory contains a range of example ESP-IDF projects. These are intended to demonstrate parts of ESP-IDF functionality, and to provide code that you can copy and adapt into your own projects.

# Example Layout

The examples are grouped into subdirectories by category. Each category directory contains one or more example projects:

* `bluetooth` contains Bluetooth (BLE & BT Classic) examples using default Bluedroid host stack.
* `bleutooth/nimble` contains BLE examples using NimBLE host stack
* `ethernet` contains Ethernet examples.
* `get-started` contains some very simple examples with minimal functionality.
* `mesh` contains Wi-Fi Mesh examples.
* `peripherals` contains examples showing driver functionality for the various onboard ESP32 peripherals.
* `protocols` contains examples showing network protocol interactions.
* `storage` contains examples showing data storage methods using SPI flash or external storage like the SD/MMC interface.
* `system` contains examples which demonstrate some internal chip features, or debugging & development tools.
* `wifi` contains examples of advanced Wi-Fi features. (For network protocol examples, see `protocols` instead.)
* `build_system` contains examples of build system features

# Using Examples

Building an example is the same as building any other project:

* Follow the Getting Started instructions which include building the "Hello World" example.
* Change into the directory of the new example you'd like to build.
* `make menuconfig` to configure the example. Most examples have a project-specific "Example Configuration" section here (for example, to set the WiFi SSID & password to use).
* `make` to build the example.
* Follow the printed instructions to flash, or run `make flash`.

# Copying Examples

Each example is a standalone project. The examples *do not have to be inside the esp-idf directory*. You can copy an example directory to anywhere on your computer in order to make a copy that you can modify and work with.

The `IDF_PATH` environment variable is the only thing that connects the example to the rest of ESP-IDF.

If you're looking for a more bare-bones project to start from, try [esp-idf-template](https://github.com/espressif/esp-idf-template).

# Contributing Examples

If you have a new example you think we'd like, please consider sending it to us as a Pull Request.

In the ESP-IDF documentation, you can find a "Creating Examples" page which lays out the steps to creating a top quality example.
