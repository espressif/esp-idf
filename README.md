# Using Espressif IoT Development Framework with the ESP32

# Prerequisites

# Configuring your project

`make menuconfig`

# Compiling your project

`make app`

# Flashing the Bootloader

ESP32 has a bootloader in ROM which runs after reset, but ESP-IDF also uses a second stage software bootloader. The ROM bootloader loads the software bootloader, which then loads the firmware app of the ESP32. The software bootloader must be flashed to offset 0x5000 in the flash.

To build the software bootloader, navigate to your project's top-level directory and run:

``` shell
make bootloader
```

If you've configured the serial port details in `make menuconfig`, then

``` shell
make bootloader-flash
```

... will automatically run esptool.py to flash the image. Otherwise, you can customise the `esptool.py` command that is printed out as part of `make bootloader`.

You only need to flash the ESP32 bootloader once.

# The Partition Table

Once you've compiled your project, the "build" directory will contain a binary file with a name like "my_app.bin". This is an ESP32 image binary that can be loaded by the bootloader.

A single ESP32's flash can contain multiple apps, as well as many different kinds of data (calibration data, filesystems, parameter storage, etc). For this reason a partition table is flashed to offset 0x4000 in the flash.

Each entry in the partition table has a name (label), type (app, data, or something else), subtype and the offset in flash where the partition is loaded.

The simplest way to use the partition table is to `make menuconfig` and choose one of the simple predefined partition tables:

* "Single factory app, no OTA"
* "Factory app, two OTA definitions"

In both cases the factory app is flashed at offset 0x10000. If you `make partition_table` then it will print a summary of the partition table.

For more details about partition tables and how to create custom variations, view the `docs/partition_tables.rst` file.
