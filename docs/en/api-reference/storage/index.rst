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

.. note::

    It is suggested to use high-level APIs (``esp_partition`` or file system) instead of low-level driver APIs to access the SPI NOR flash.

    Due to the restriction of NOR flash and ESP hardware, accessing the main flash will affect the performance of the whole system. See :doc:`/api-reference/peripherals/spi_flash/index` to learn more about the limitations.

.. toctree::
    :maxdepth: 1

    fatfs
    mass_mfg.rst
    nvs_flash
    nvs_encryption
    nvs_partition_gen.rst
    nvs_partition_parse.rst
    sdmmc
    partition
    spiffs
    vfs
    wear-levelling

Code examples for this API section are provided in the :example:`storage` directory of ESP-IDF examples.
