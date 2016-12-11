Partition Tables
================

Overview
--------

A single ESP32's flash can contain multiple apps, as well as many different kinds of data (calibration data, filesystems, parameter storage, etc). For this reason a partition table is flashed to offset 0x8000 in the flash.

Partition table length is 0xC00 bytes (maximum 95 partition table entries). If the partition table is signed due to `secure boot`, the signature is appended after the table data.

Each entry in the partition table has a name (label), type (app, data, or something else), subtype and the offset in flash where the partition is loaded.

The simplest way to use the partition table is to `make menuconfig` and choose one of the simple predefined partition tables:

* "Single factory app, no OTA"
* "Factory app, two OTA definitions"

In both cases the factory app is flashed at offset 0x10000. If you `make partition_table` then it will print a summary of the partition table.

Built-in Partition Tables
-------------------------

Here is the summary printed for the "Single factory app, no OTA" configuration::

  # Espressif ESP32 Partition Table
  # Name,   Type, SubType, Offset,  Size
  nvs,      data, nvs,     0x9000,  0x6000
  phy_init, data, phy,     0xf000,  0x1000
  factory,  app,  factory, 0x10000, 1M

* At a 0x10000 (64KB) offset in the flash is the app labelled "factory". The bootloader will run this app by default.
* There are also two data regions defined in the partition table for storing NVS library partition and PHY init data.

Here is the summary printed for the "Factory app, two OTA definitions" configuration::

  # Espressif ESP32 Partition Table
  # Name,   Type, SubType, Offset,  Size
  nvs,      data, nvs,     0x9000,  0x4000
  otadata,  data, ota,     0xd000,  0x2000
  phy_init, data, phy,     0xf000,  0x1000
  factory,  0,    0,       0x10000, 1M
  ota_0,    0,    ota_0,   ,        1M
  ota_1,    0,    ota_1,   ,        1M

* There are now three app partition definitions.
* The type of all three are set as "app", but the subtype varies between the factory app at 0x10000 and the next two "OTA" apps.
* There is also a new "ota data" slot, which holds the data for OTA updates. The bootloader consults this data in order to know which app to execute. If "ota data" is empty, it will execute the factory app.


Creating Custom Tables
----------------------

If you choose "Custom partition table CSV" in menuconfig then you can also enter the name of a CSV file (in the project directory) to use for your partition table. The CSV file can describe any number of definitions for the table you need.

The CSV format is the same format as printed in the summaries shown above. However, not all fields are required in the CSV. For example, here is the "input" CSV for the OTA partition table::

  # Name,   Type, SubType, Offset,   Size
  nvs,      data, nvs,     0x9000,  0x4000
  otadata,  data, ota,     0xd000,  0x2000
  phy_init, data, phy,     0xf000,  0x1000
  factory,  app,  factory, 0x10000,  1M
  ota_0,    app,  ota_0,   ,         1M
  ota_1,    app,  ota_1,   ,         1M
  
* Whitespace between fields is ignored, and so is any line starting with # (comments).
* Each non-comment line in the CSV file is a partition definition.
* Only the offset for the first partition is supplied. The gen_esp32part.py tool fills in each remaining offset to start after the preceding partition.

Name field
~~~~~~~~~~

Name field can be any meaningful name. It is not significant to the ESP32. Names longer than 16 characters will be truncated.

Type field
~~~~~~~~~~

Type field can be specified as app (0) or data (1). Or it can be a number 0-254 (or as hex 0x00-0xFE). Types 0x00-0x3F are reserved for Espressif. If your application needs to store data, please add a custom partition type in the range 0x40-0xFE.

The bootloader ignores any types other than 0 & 1.

Subtype
~~~~~~~

When type is "app", the subtype field can be specified as factory (0), ota_0 (0x10) ... ota_15 (0x1F) and test (0x20). Or it can be any number 0-255 (0x00-0xFF). The bootloader will execute the factory app unless there it sees a partition of type data/ota, in which case it reads this partition to determine which OTA image to boot

When type is "data", the subtype field can be specified as ota (0), phy (1), nvs (2). Or it can be a number 0x00-0xFF. The bootloader ignores all data subtypes except for ota. Subtypes 0-0x7f are reserved for Espressif use. To create custom data partition subtypes use "data" type, and choose any unused subtype in 0x80-0xFF range. If you are porting a filesystem to the ESP-IDF, consider opening a PR to add the new subtype to esp_partition.h file.

Offset & Size
~~~~~~~~~~~~~

Only the first offset field is required (we recommend using 0x10000). Partitions with blank offsets will start after the previous partition.

App partitions have to be at offsets aligned to 0x10000 (64K). If you leave the offset field blank, the tool will automatically align the partition. If you specify an unaligned offset for an app partition, the tool will return an error.

Sizes and offsets can be specified as decimal numbers, hex numbers with the prefix 0x, or size multipliers M or K (1024 and 1024*1024 bytes).

NVS data partition has to be at least 0x3000 bytes long, and OTA data parition has to be 0x2000 bytes long. If you are using NVS in your application to store a lot of data, consider using a custom partition table with larger NVS partition.

Generating Binary Partition Table
---------------------------------

The partition table which is flashed to the ESP32 is in a binary format, not CSV. The tool bin/gen_esp32part.py is used to convert between CSV and binary formats.

If you configure the partition table CSV name in ``make menuconfig`` and then ``make partition_table``, this conversion is done for you.

To convert CSV to Binary manually::

  python bin/gen_esp32part.py --verify input_partitions.csv binary_partitions.bin

To convert binary format back to CSV::

  python bin/gen_esp32part.py --verify binary_partitions.bin input_partitions.csv

To display the contents of a binary partition table on stdout (this is how the summaries displayed when running `make partition_table` are generated::

  python bin/gen_esp32part.py binary_partitions.bin

``gen_esp32part.py`` takes one optional argument, ``--verify``, which will also verify the partition table during conversion (checking for overlapping partitions, unaligned partitions, etc.)

Flashing the partition table
----------------------------

* ``make partition_table-flash``: will flash the partition table with esptool.py.
* ``make flash``: Will flash everything including the partition table.

A manual flashing command is also printed as part of ``make partition_table``.


.. _secure boot: security/secure-boot.rst
