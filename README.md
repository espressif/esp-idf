# Using Espressif IoT Development Framework with the ESP32

[![alt text](https://readthedocs.org/projects/docs/badge/?version=latest "Documentation Status")](http://esp-idf.readthedocs.io/en/latest/?badge=latest)

# Setting Up ESP-IDF

In the [docs](docs) directory you will find per-platform setup guides:

* [Windows Setup Guide](docs/windows-setup.rst)
* [Mac OS Setup Guide](docs/macos-setup.rst)
* [Linux Setup Guide](docs/linux-setup.rst)

# Finding A Project

As well as the [esp-idf-template](https://github.com/espressif/esp-idf-template) project mentioned in the setup guide, esp-idf comes with some example projects in the [examples](examples) directory.

Once you've found the project you want to work with, change to its directory and you can configure and build it:

# Configuring your project

`make menuconfig`

# Compiling your project

`make all`

... will compile app, bootloader and generate a partition table based on the config.

# Flashing your project

When `make all` finishes, it will print a command line to use esptool.py to flash the chip. However you can also do this from make by running:

`make flash`

This will flash the entire project (app, bootloader and partition table) to a new chip. The settings for serial port flashing can be configured with `make menuconfig`.

You don't need to run `make all` before running `make flash`, `make flash` will automatically rebuild anything which needs it.

# Viewing Serial Output

The `make monitor` target will use the already-installed [miniterm](http://pyserial.readthedocs.io/en/latest/tools.html#module-serial.tools.miniterm) (a part of pyserial) to display serial output from the ESP32 on the terminal console.

Exit miniterm by typing Ctrl-].

To flash and monitor output in one pass, you can run:

`make flash monitor`

# Compiling & Flashing Just the App

After the initial flash, you may just want to build and flash just your app, not the bootloader and partition table:

* `make app` - build just the app.
* `make app-flash` - flash just the app.

`make app-flash` will automatically rebuild the app if it needs it.

(There's no downside to reflashing the bootloader and partition table each time, if they haven't changed.)

# Parallel Builds

esp-idf supports compiling multiple files in parallel, so all of the above commands can be run as `make -jN` where `N` is the number of parallel make processes to run (generally N should be equal to or one more than the number of CPU cores in your system.)

Multiple make functions can be combined into one. For example: to build the app & bootloader using 5 jobs in parallel, then flash everything, and then display serial output from the ESP32 run:

```
make -j5 flash monitor
```

# The Partition Table

Once you've compiled your project, the "build" directory will contain a binary file with a name like "my_app.bin". This is an ESP32 image binary that can be loaded by the bootloader.

A single ESP32's flash can contain multiple apps, as well as many different kinds of data (calibration data, filesystems, parameter storage, etc). For this reason a partition table is flashed to offset 0x4000 in the flash.

Each entry in the partition table has a name (label), type (app, data, or something else), subtype and the offset in flash where the partition is loaded.

The simplest way to use the partition table is to `make menuconfig` and choose one of the simple predefined partition tables:

* "Single factory app, no OTA"
* "Factory app, two OTA definitions"

In both cases the factory app is flashed at offset 0x10000. If you `make partition_table` then it will print a summary of the partition table.

For more details about partition tables and how to create custom variations, view the `docs/partition-tables.rst` file.

# Erasing Flash

The `make flash` target does not erase the entire flash contents. However it is sometimes useful to set the device back to a totally erased state, particularly when making partition table changes or OTA app updates. To erase the entire flash, run `make erase_flash`.

This can be combined with other targets, ie `make erase_flash flash` will erase everything and then re-flash the new app, bootloader and partition table.

# Resources

* The [docs directory of the esp-idf repository](docs) contains source of [esp-idf](http://esp-idf.readthedocs.io/) documentation.

* The [esp32.com forum](http://esp32.com/) is a place to ask questions and find community resources.

* [Check the Issues section on github](https://github.com/espressif/esp-idf/issues) if you find a bug or have a feature request. Please check existing Issues before opening a new one.

* If you're interested in contributing to esp-idf, please check the [Contributions Guide](http://esp-idf.readthedocs.io/en/latest/contributing.html>).

