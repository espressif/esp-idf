Storage API
***********

:link_to_translation:`zh_CN:[中文]`

This section contains reference of the high-level storage APIs. They are based on low-level drivers such as SPI flash, SD/MMC.

- :doc:`Partitions API <partition>` allow block based access to SPI flash according to the :doc:`/api-guides/partition-tables`.
- :doc:`Non-Volatile Storage library (NVS) <nvs_flash>` implements a fault-tolerant wear-levelled key-value storage in SPI NOR flash.
- :doc:`Virtual File System (VFS) <vfs>` library provides an interface for registration of file system drivers. SPIFFS, FAT and various other file system libraries are based on the VFS.
- :doc:`SPIFFS <spiffs>` is a wear-levelled file system optimized for SPI NOR flash, well suited for small partition sizes and low throughput
- :doc:`FAT <fatfs>` is a standard file system which can be used in SPI flash or on SD/MMC cards
- :doc:`Wear Levelling <wear-levelling>` library implements a flash translation layer (FTL) suitable for SPI NOR flash. It is used as a container for FAT partitions in flash.

For information about storage security, please refer to :doc:`Storage Security <storage-security>`.

.. note::

    It is suggested to use high-level APIs (``esp_partition`` or file system) instead of low-level driver APIs to access the SPI NOR flash.

    Due to the restriction of NOR flash and ESP hardware, accessing the main flash will affect the performance of the whole system. See :doc:`/api-reference/peripherals/spi_flash/index` to learn more about the limitations.

.. toctree::
    :maxdepth: 1

    fatfs
    fatfsgen
    mass_mfg.rst
    nvs_flash
    nvs_bootloader
    nvs_encryption
    nvs_partition_gen.rst
    nvs_partition_parse.rst
    sdmmc
    partition
    spiffs
    vfs
    wear-levelling
    storage-security.rst

Examples
--------

.. list-table:: NVS API examples
    :widths: 25 75
    :header-rows: 0

    * - **Code Example**
      - **Description**
    * - :example:`nvs_rw_blob <storage/nvs_rw_blob>`
      - Shows the use of the C-style API to read and write blob data types in NVS flash.
    * - :example:`nvs_rw_value <storage/nvs_rw_value>`
      - Shows the use of the C-style API to read and write integer data types in NVS flash.
    * - :example:`nvs_rw_value_cxx <storage/nvs_rw_value_cxx>`
      - Shows the use of the C++-style API to read and write integer data types in NVS flash.
    * - :example:`nvs_bootloader <storage/nvs_bootloader>`
      - Shows the use of the API available to the bootloader code to read NVS data.
    * - :example:`nvsgen <storage/nvsgen>`
      - Demonstrates how to use the Python-based NVS image generation tool to create an NVS partition image from the contents of a CSV file.

.. list-table:: Common Filesystem API
    :widths: 25 75
    :header-rows: 0

    * - **Code Example**
      - **Description**
    * - :example:`fatfs/getting_started <storage/fatfs/getting_started>`
      - Demonstrates basic common file API (stdio.h) usage over internal flash using FATFS.
    * - :example:`fatfs/fs_operations <storage/fatfs/fs_operations>`
      - Demonstrates POSIX API for filesystem manipulation, such as moving, removing and renaming files.

.. list-table:: FATFS API examples
    :widths: 25 75
    :header-rows: 0

    * - **Code Example**
      - **Description**
    * - :example:`fatfsgen <storage/fatfs/fatfsgen>`
      - Demonstrates the capabilities of Python-based tooling for FATFS images available on host computers.
    * - :example:`ext_flash_fatfs <storage/fatfs/ext_flash>`
      - Demonstrates using FATFS over wear leveling on external flash.
    * - :example:`wear_leveling <storage/wear_levelling>`
      - Demonstrates using FATFS over wear leveling on internal flash.

.. list-table:: SPIFFS API examples
    :widths: 25 75
    :header-rows: 0

    * - **Code Example**
      - **Description**
    * - :example:`spiffs <storage/spiffs>`
      - Shows the use of the SPIFFS API to initialize the filesystem and work with files using POSIX functions.
    * - :example:`spiffsgen <storage/spiffsgen>`
      - Demonstrates the capabilities of Python-based tooling for SPIFFS images available on host computers.

.. list-table:: Partition API examples
    :widths: 25 75
    :header-rows: 0

    * - **Code Example**
      - **Description**
    * - :example:`partition_api <storage/partition_api>`
      - Provides an overview of API functions to look up particular partitions, perform basic I/O operations, and use partitions via CPU memory mapping.
    * - :example:`parttool <storage/parttool>`
      - Demonstrates the capabilities of Python-based tooling for partition images available on host computers.

.. list-table:: VFS related examples
    :widths: 25 75
    :header-rows: 0

    * - **Code Example**
      - **Description**
    * - :example:`littlefs <storage/littlefs>`
      - Shows the use of the LittleFS component to initialize the filesystem and work with a file using POSIX functions.
    * - :example:`semihost_vfs <storage/semihost_vfs>`
      - Demonstrates the use of the VFS API to let an ESP-based device access a file on a JTAG-connected host using POSIX functions.
