# Espressif IoT Development Framework

* [中文版](./README_CN.md)

[![Documentation Status](https://readthedocs.com/projects/espressif-esp-idf/badge/?version=latest)](https://docs.espressif.com/projects/esp-idf/en/latest/?badge=latest)

ESP-IDF is the official development framework for the [ESP32](https://espressif.com/en/products/hardware/esp32/overview) chip.

# Developing With ESP-IDF

## Setting Up ESP-IDF

See setup guides for detailed instructions to set up the ESP-IDF:

* [Getting Started Guide for the stable ESP-IDF version](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/)
* [Getting Started Guide for the latest (master branch) ESP-IDF version](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/)

### Non-GitHub forks

ESP-IDF uses relative locations as its submodules URLs ([.gitmodules](.gitmodules)). So they link to GitHub.
If ESP-IDF is forked to a Git repository which is not on GitHub, you will need to run the script
[tools/set-submodules-to-github.sh](tools/set-submodules-to-github.sh) after git clone.
The script sets absolute URLs for all submodules, allowing `git submodule update --init --recursive` to complete.
If cloning ESP-IDF from GitHub, this step is not needed.

## Finding a Project

As well as the [esp-idf-template](https://github.com/espressif/esp-idf-template) project mentioned in Getting Started, ESP-IDF comes with some example projects in the [examples](examples) directory.

Once you've found the project you want to work with, change to its directory and you can configure and build it.

To start your own project based on an example, copy the example project directory outside of the ESP-IDF directory.

# Quick Reference

See the Getting Started guide links above for a detailed setup guide. This is a quick reference for common commands when working with ESP-IDF projects:

## Setup Build Environment

(See Getting Started guide for a full list of required steps with details.)

* Install host build dependencies mentioned in Getting Started guide.
* Add `tools/` directory to the PATH
* Run `python -m pip install -r requirements.txt` to install Python dependencies

## Configuring the Project

`idf.py menuconfig`

* Opens a text-based configuration menu for the project.
* Use up & down arrow keys to navigate the menu.
* Use Enter key to go into a submenu, Escape key to go out or to exit.
* Type `?` to see a help screen. Enter key exits the help screen.
* Use Space key, or `Y` and `N` keys to enable (Yes) and disable (No) configuration items with checkboxes "`[*]`"
* Pressing `?` while highlighting a configuration item displays help about that item.
* Type `/` to search the configuration items.

Once done configuring, press Escape multiple times to exit and say "Yes" to save the new configuration when prompted.

## Compiling the Project

`idf.py build`

... will compile app, bootloader and generate a partition table based on the config.

## Flashing the Project

When the build finishes, it will print a command line to use esptool.py to flash the chip. However you can also do this automatically by running:

`idf.py -p PORT flash`

Replace PORT with the name of your serial port (like `COM3` on Windows, `/dev/ttyUSB0` on Linux, or `/dev/cu.usbserial-X` on MacOS. If the `-p` option is left out, `idf.py flash` will try to flash the first available serial port.

This will flash the entire project (app, bootloader and partition table) to a new chip. The settings for serial port flashing can be configured with `idf.py menuconfig`.

You don't need to run `idf.py build` before running `idf.py flash`, `idf.py flash` will automatically rebuild anything which needs it.

## Viewing Serial Output

The `idf.py monitor` target uses the [idf_monitor tool](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/idf-monitor.html) to display serial output from the ESP32. idf_monitor also has a range of features to decode crash output and interact with the device. [Check the documentation page for details](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/idf-monitor.html).

Exit the monitor by typing Ctrl-].

To build, flash and monitor output in one pass, you can run:

`idf.py flash monitor`

## Compiling & Flashing Only the App

After the initial flash, you may just want to build and flash just your app, not the bootloader and partition table:

* `idf.py app` - build just the app.
* `idf.py app-flash` - flash just the app.

`idf.py app-flash` will automatically rebuild the app if any source files have changed.

(In normal development there's no downside to reflashing the bootloader and partition table each time, if they haven't changed.)

## Erasing Flash

The `idf.py flash` target does not erase the entire flash contents. However it is sometimes useful to set the device back to a totally erased state, particularly when making partition table changes or OTA app updates. To erase the entire flash, run `idf.py erase_flash`.

This can be combined with other targets, ie `idf.py -p PORT erase_flash flash` will erase everything and then re-flash the new app, bootloader and partition table.

# Resources

* Documentation for the latest version: https://docs.espressif.com/projects/esp-idf/. This documentation is built from the [docs directory](docs) of this repository.

* The [esp32.com forum](https://esp32.com/) is a place to ask questions and find community resources.

* [Check the Issues section on github](https://github.com/espressif/esp-idf/issues) if you find a bug or have a feature request. Please check existing Issues before opening a new one.

* If you're interested in contributing to ESP-IDF, please check the [Contributions Guide](https://docs.espressif.com/projects/esp-idf/en/latest/contribute/index.html).


