# Espressif IoT Development Framework

[![Documentation Status](https://readthedocs.com/projects/espressif-esp-idf/badge/?version=latest)](https://docs.espressif.com/projects/esp-idf/en/latest/?badge=latest)

ESP-IDF is the official development framework for the [ESP32](https://espressif.com/en/products/hardware/esp32/overview) chip.

# Developing With ESP-IDF

## Setting Up ESP-IDF

See setup guides for detailed instructions to set up the ESP-IDF:

* [Getting Started Guide for the stable ESP-IDF version](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/)
* [Getting Started Guide for the latest (master branch) ESP-IDF version](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/)

## Finding a Project

As well as the [esp-idf-template](https://github.com/espressif/esp-idf-template) project mentioned in Getting Started, ESP-IDF comes with some example projects in the [examples](examples) directory.

Once you've found the project you want to work with, change to its directory and you can configure and build it.

To start your own project based on an example, copy the example project directory outside of the ESP-IDF directory.

# Quick Reference

See the Getting Started guide links above for a detailed setup guide. This is a quick reference for common commands when working with ESP-IDF projects:

## Configuring the Project

`make menuconfig`

* Opens a text-based configuration menu for the project.
* Use up & down arrow keys to navigate the menu.
* Use Enter key to go into a submenu, Escape key to go out or to exit.
* Type `?` to see a help screen. Enter key exits the help screen.
* Use Space key, or `Y` and `N` keys to enable (Yes) and disable (No) configuration items with checkboxes "`[*]`"
* Pressing `?` while highlighting a configuration item displays help about that item.
* Type `/` to search the configuration items.

Once done configuring, press Escape multiple times to exit and say "Yes" to save the new configuration when prompted.

## Compiling the Project

`make -j4 all`

... will compile app, bootloader and generate a partition table based on the config.

NOTE: The `-j4` option causes `make` to run 4 parallel jobs. This is much faster than the default single job. The recommended number to pass to this option is `-j(number of CPUs + 1)`.

## Flashing the Project

When the build finishes, it will print a command line to use esptool.py to flash the chip. However you can also do this automatically by running:

`make -j4 flash`

This will flash the entire project (app, bootloader and partition table) to a new chip. The settings for serial port flashing can be configured with `make menuconfig`.

You don't need to run `make all` before running `make flash`, `make flash` will automatically rebuild anything which needs it.

## Viewing Serial Output

The `make monitor` target uses the [idf_monitor tool](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/idf-monitor.html) to display serial output from the ESP32. idf_monitor also has a range of features to decode crash output and interact with the device. [Check the documentation page for details](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/idf-monitor.html).

Exit the monitor by typing Ctrl-].

To build, flash and monitor output in one pass, you can run:

`make -j4 flash monitor`

## Compiling & Flashing Only the App

After the initial flash, you may just want to build and flash just your app, not the bootloader and partition table:

* `make app` - build just the app.
* `make app-flash` - flash just the app.

`make app-flash` will automatically rebuild the app if any source files have changed.

(In normal development there's no downside to reflashing the bootloader and partition table each time, if they haven't changed.)

## Parallel Builds

ESP-IDF supports compiling multiple files in parallel, so all of the above commands can be run as `make -jN` where `N` is the number of parallel make processes to run (generally N should be equal to the number of CPU cores in your system, plus one.)

Multiple make functions can be combined into one. For example: to build the app & bootloader using 5 jobs in parallel, then flash everything, and then display serial output from the ESP32 run:

```
make -j5 flash monitor
```


## The Partition Table

Once you've compiled your project, the "build" directory will contain a binary file with a name like "my_app.bin". This is an ESP32 image binary that can be loaded by the bootloader.

A single ESP32's flash can contain multiple apps, as well as many different kinds of data (calibration data, filesystems, parameter storage, etc). For this reason a partition table is flashed to offset 0x8000 in the flash.

Each entry in the partition table has a name (label), type (app, data, or something else), subtype and the offset in flash where the partition is loaded.

The simplest way to use the partition table is to `make menuconfig` and choose one of the simple predefined partition tables:

* "Single factory app, no OTA"
* "Factory app, two OTA definitions"

In both cases the factory app is flashed at offset 0x10000. If you `make partition_table` then it will print a summary of the partition table.

For more details about partition tables and how to create custom variations, view the [`docs/en/api-guides/partition-tables.rst`](docs/en/api-guides/partition-tables.rst) file.

## Erasing Flash

The `make flash` target does not erase the entire flash contents. However it is sometimes useful to set the device back to a totally erased state, particularly when making partition table changes or OTA app updates. To erase the entire flash, run `make erase_flash`.

This can be combined with other targets, ie `make erase_flash flash` will erase everything and then re-flash the new app, bootloader and partition table.

# Resources

* Documentation for the latest version: https://docs.espressif.com/projects/esp-idf/. This documentation is built from the [docs directory](docs) of this repository.

* The [esp32.com forum](https://esp32.com/) is a place to ask questions and find community resources.

* [Check the Issues section on github](https://github.com/espressif/esp-idf/issues) if you find a bug or have a feature request. Please check existing Issues before opening a new one.

* If you're interested in contributing to ESP-IDF, please check the [Contributions Guide](https://docs.espressif.com/projects/esp-idf/en/latest/contribute/index.html).


