SPIFFS Filesystem
=================

Overview
--------

SPIFFS is a file system intended for SPI NOR flash devices on embedded targets.
It supports wear leveling, file system consistency checks and more.

Notes
-----

 - Presently, spiffs does not support directories. It produces a flat structure. If SPIFFS is mounted under ``/spiffs`` creating a file with path ``/spiffs/tmp/myfile.txt`` will create a file called ``/tmp/myfile.txt`` in SPIFFS, instead of ``myfile.txt`` under directory ``/spiffs/tmp``. 
 - It is not a realtime stack. One write operation might last much longer than another.
 - Presently, it does not detect or handle bad blocks.

Tools
-----

Host-Side tools for creating SPIFS partition images exist and one such tool is `mkspiffs <https://github.com/igrr/mkspiffs>`_.
You can use it to create image from a given folder and then flash that image with ``esptool.py``

To do that you need to obtain some parameters:

- Block Size: 4096 (standard for SPI Flash)
- Page Size: 256 (standard for SPI Flash)
- Image Size: Size of the partition in bytes (can be obtained from partition table)
- Partition Offset: Starting address of the partition (can be obtained from partition table)

To pack a folder into 1 Megabyte image::

    mkspiffs -c [src_folder] -b 4096 -p 256 -s 0x100000 spiffs.bin

To flash the image to ESP32 at offset 0x110000::

    python esptool.py --chip esp32 --port [port] --baud [baud] write_flash -z 0x110000 spiffs.bin

See also
--------

- :doc:`Partition Table documentation <../../api-guides/partition-tables>`

Application Example
-------------------

An example for using SPIFFS is provided in :example:`storage/spiffs` directory. This example initializes and mounts SPIFFS partition, and writes and reads data from it using POSIX and C library APIs. See README.md file in the example directory for more information.

High level API Reference
------------------------

* :component_file:`spiffs/include/esp_spiffs.h`

.. include:: /_build/inc/esp_spiffs.inc
