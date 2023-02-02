Storage API
***********

:link_to_translation:`zh_CN:[中文]`

This section contains reference of the high-level storage APIs. They are based on low-level drivers such as SPI Flash, SD/MMC.

- :doc:`Partitions API <partition>` allow block based access to SPI Flash according to the :doc:`Partition Table </api-guides/partition-tables>`.
- :doc:`Non-Volatile Storage library (NVS) <nvs_flash>` implements a fault-tolerant wear-levelled key-value storage in SPI NOR Flash.
- :doc:`Virtual File System (VFS) <vfs>` library provides an interface for registration of file system drivers. SPIFFS, FAT and various other file system libraries are based on the VFS.
- :doc:`SPIFFS <spiffs>` is a wear-levelled file system optimized for SPI NOR Flash, well suited for small partition sizes and low throughput
- :doc:`FAT <fatfs>` is a standard file system which can be used in SPI Flash or on SD/MMC cards
- :doc:`Wear Levelling <wear-levelling>` library implements a flash translation layer (FTL) suitable for SPI NOR Flash. It is used as a container for FAT partitions in Flash.

.. note::
    It's suggested to use high-level APIs (``esp_partition`` or file system) instead of low-level driver APIs to access the SPI NOR Flash.

    Due to the restriction of NOR Flash and ESP hardware, accessing the main flash will affect the performance of the whole system. See :doc:`SPI Flash Documents </api-reference/peripherals/spi_flash/index>` to learn more about the limitations.

.. toctree::
    :maxdepth: 1

    fatfs
    mass_mfg.rst
    nvs_flash
    nvs_partition_gen.rst
    nvs_partition_parse.rst
    sdmmc
    partition
    spiffs
    vfs
    wear-levelling

Code examples for this API section are provided in the :example:`storage` directory of ESP-IDF examples.
