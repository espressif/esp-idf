File System Considerations
==========================

:link_to_translation:`zh_CN:[中文]`

This chapter is intended to help you decide which file system is most suitable for your application. It points out specific features and properties of the file systems supported by the ESP-IDF, which are important in typical use-cases rather than describing all the specifics or comparing implementation details. Technical details for each file system are available in their corresponding documentation.

Currently, the ESP-IDF framework supports three file systems. ESP-IDF provides convenient APIs to handle the mounting and dismounting of file systems in a unified way. File and directory access is implemented via C/POSIX standard file APIs, allowing all applications to use the same interface regardless of the underlying file system:

- :ref:`FatFS <fatfs-fs-section>`
- :ref:`SPIFFS <spiffs-fs-section>`
- :ref:`LittleFS <littlefs-fs-section>`

All of them are based on 3rd-party libraries connected to the ESP-IDF through various wrappers and modifications.

ESP-IDF also provides the NVS Library API for simple data storage use cases, using keys to access associated values. While it is not a full-featured file system, it is a good choice for storing configuration data, calibration data, and similar information. For more details, see the :ref:`NVS Library <nvs-fs-section>` section.

The most significant properties and features of above-mentioned file systems are summarized in the following table:

.. list-table::
    :widths: 20 40 40 40
    :header-rows: 1

    * -
      - FatFS
      - SPIFFS
      - LittleFS
    * - Features
      - * Implements MS FAT12, FAT16, FAT32 and optionally exFAT variants
        * General purpose filesystem, widely compatible across most HW platforms
        * Well documented
        * Thread safe
      - * Developed for NOR flash devices on embedded systems, low RAM usage
        * Implements static wear levelling
        * Limited documentation, no ongoing development
        * Thread safe
      - * Designed as fail-safe, with own wear levelling and with fixed amount of RAM usage independent on the file system size
        * Well documented
        * Thread safe
    * - Storage units and limits
      - * Clusters (1–128 sectors)
        * Supported sector sizes: 512 B, 4096 B
        * FAT12: cluster size 512 B – 8 kB, max 4085 clusters
        * FAT16: cluster size 512 B – 64 kB, max 65525 clusters
        * FAT32: cluster size 512 B – 32 kB, max 268435455 clusters
      - * Logical pages, logical blocks (consists of pages)
        * Typical setup: page = 256 B, block = 64 kB
      - * Blocks, metadata pairs
        * Typical block size: 4 kB
    * - Wear Levelling
      - Optional (for SPI Flash)
      - Integrated
      - Integrated
    * - Minimum partition size
      - * 8 sectors with wear levelling on (4 FATFS sectors + 4 WL sectors with WL sector size = 4096 B)
        * plus 4 sectors at least
        * real number given by WL configuration (Safe, Perf)
      - * 6 logical blocks
        * 8 pages per block
      - Not specified, theoretically 2 blocks
    * - Maximum partition size
      - * FAT12: approx. 32 MB with 8 kB clusters
        * FAT16: approx. 4 GB with 64 kB clusters (theoretical)
        * FAT32: approx. 8 TB with 32 kB clusters (theoretical)
      - Absolute maximum not specified, more than 1024 pages per block not recommended
      - Not specified, theoretically around 2 GB
    * - Directory Support
      - * Yes (max 65536 entries in a common FAT directory)
        * Limitations:
            * FAT12: max 224 files in the Root directory
            * FAT16: max 512 files in the Root directory
            * FAT32: the Root is just another directory
      - No
      - Yes
    * - Power failure protection
      - No
      - Partial (see :ref:`spiffs-fs-section`)
      - Yes (integrated)
    * - Encryption support
      - Yes
      - No
      - Yes
    * - Supported targets
      - * SPI Flash (NOR)
        * SD cards
      - SPI Flash (NOR)
      - * SPI Flash (NOR)
        * SD cards (IDF >= v5.0)

For file systems performance comparison using various configurations and parameters, see Storage performance benchmark example :example:`storage/perf_benchmark`.


.. _fatfs-fs-section:

FatFS
---------

The most supported file system, recommended for common applications - file/directory operations, data storage, logging, etc. It provides automatic resolution of specific FAT system type and is widely compatible with PC or other platforms. FatFS supports partition encryption, read-only mode, optional wear-levelling for SPI Flash (SD cards use own built-in WL), equipped with auxiliary host side tools (generators and parsers, Python scripts). It supports SDMMC access. The biggest weakness is its low resilience against sudden power-off events. To mitigate such a scenario impact, the ESP-IDF FatFS default setup deploys 2 FAT table copies. This option can be disabled by setting :cpp:member:`esp_vfs_fat_mount_config_t::use_one_fat` flag (the 2-FAT processing is fully handled by the FatFS library). See also related examples.

**Related documents:**

- `FatFS source site <http://elm-chan.org/fsw/ff/>`_
- More about `FAT table size limits <https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system#Size_limits>`_
- :ref:`Using FatFS with VFS <using-fatfs-with-vfs>`
- :ref:`Using FatFS with VFS and SD cards <using-fatfs-with-vfs-and-sdcards>`
- ESP-IDF FatFS tools: :ref:`Partition generator <fatfs-partition-generator>` and :ref:`Partition analyzer <fatfs-partition-analyzer>`

**Examples:**

* :example:`storage/sd_card` demonstrates how to access the SD card that uses the FAT file system.
* :example:`storage/fatfs/ext_flash` demonstrates how to access the external flash that uses the FAT file system.


.. _spiffs-fs-section:

SPIFFS
----------------------

SPIFFS is a file system providing certain level of power-off safety (see repair-after-restart function :cpp:func:`esp_spiffs_check`) and built-in wear levelling. It tends to slow down when exceeding around 70% of the dedicated partition size due to its garbage collector implementation, and also doesn't support directories. It is useful for applications depending only on few files (possibly large) and requiring high level of consistency. Generally, the SPIFFS needs less RAM resources than FatFS and supports flash chips up to 128 MB in size. Please keep in mind the SPIFFS is not being developed and maintained anymore, so consider precisely whether its advantages for your project really prevail over the other file systems.

**Related documents:**

* :doc:`SPIFFS Filesystem <../api-reference/storage/spiffs>`
* :ref:`Tools For Generating SPIFFS Images <spiffs-generator>`

**Examples:**

* :example:`storage/spiffs` demonstrates how to use SPIFFS on {IDF_TARGET_NAME} chip.


.. _littlefs-fs-section:

LittleFS
----------------------

LittleFS is a block based file system designed for microcontrollers and embedded devices. It provides a good level of power failure resilience, implements dynamic wear levelling, and has very low RAM requirements. The system also has configurable limits and integrated SD/MMC card support. It is a recommended choice for general type of application. The only disadvantage is the file system not being natively compatible with other platforms (unlike FatFS).

LittleFS is available as external component in the `ESP Component Registry <https://components.espressif.com/>`_. See `LittleFS component page <https://components.espressif.com/components/joltwallet/littlefs>`_ for the details on including the file system into your project.

**Related documents:**

* `LittleFS project home (sources, documentation) <https://github.com/littlefs-project/littlefs>`_
* `LittleFS auxiliary tools and related projects <https://github.com/littlefs-project/littlefs?tab=readme-ov-file#related-projects>`_
* `LittleFS port for ESP-IDF <https://github.com/joltwallet/esp_littlefs>`_
* `ESP-IDF LittleFS component <https://components.espressif.com/components/joltwallet/littlefs>`_

**Examples:**

* :example:`storage/littlefs` demonstrates how to use LittleFS on {IDF_TARGET_NAME} chip.

.. _nvs-fs-section:

NVS Library
---------------

Non-volatile Storage (NVS) is useful for applications depending on handling numerous key-value pairs, for instance application system configuration. For convenience, the key space is divided into namespaces, each namespace is a separate storage area. Besides the basic data types up to the size of 64-bit integers, the NVS also supports zero terminated strings and blobs - binary data of arbitrary length.

Features include:

* Flash wear leveling by design.
* Sudden power-loss protection (data is stored in a way that ensures atomic updates).
* Encryption support (AES-XTS).
* Tooling is provided for both data preparation during manufacturing and offline analysis.

Points to keep in mind when developing NVS related code:

* The recommended use case is storing configuration data that does not change frequently.
* NVS is not suitable for logging or other use cases with frequent, large data updates. NVS works best with small updates and low-frequency writes. Another limitation is the maximum number of flash page erase cycles, which is typically around 100,000 for NOR flash devices.
* If the application needs to store groups of data with significantly different update rates, it is recommended to use separate NVS flash partitions for each group. This makes wear leveling easier to manage and reduces the risk of data corruption.
* The default NVS partition (the one labeled "nvs") is used by other ESP-IDF components such as WiFi, Bluetooth, etc. It is recommended to use a separate partition for application data to avoid conflicts with other components.
* The allocation unit for NVS storage in flash memory is one page—4,096 bytes. At least three pages are needed for each NVS partition to function properly. One page is always reserved and never used for data storage.
* Before writing or updating existing data, there must be enough free space in the NVS partition to store both the old and new data. The NVS library doesn't support partial updates. This can be especially challenging with large BLOBs spanning flash page boundaries, resulting in longer write times and increased overhead space consumption.
* The NVS library cannot ensure data consistency in out-of-spec power environments, such as systems powered by batteries or solar panels. Misinterpretation of flash data in such situations can lead to corruption of the NVS flash partition. Developers should include data recovery code, e.g., based on a read-only data partition with factory settings.
* An initialized NVS library leaves a RAM footprint, which scales linearly with the overall size of the flash partitions and the number of cached keys.

**Read-only NVS partitions:**

* Read-only partitions can be used to store data that should not be modified at runtime. This is useful for storing firmware or configuration data that should not be changed by the application.
* NVS partitions can be flagged as ``readonly`` in the partition table CSV file. Size of read-only NVS partition can be as small as one page (4 KiB/``0x1000``), which is not possible for standard read-write NVS partitions.
* Partitions of sizes ``0x1000`` and ``0x2000`` are always read-only and partitions of size ``0x3000`` and above are always read-write capable (still can be opened in read-only mode in the code).

**Related documents:**

- To learn more about the API and NVS library details, see the :doc:`NVS documentation page <../api-reference/storage/nvs_flash>`.
- For mass production, you can use the :doc:`NVS Partition Generator Utility <../api-reference/storage/nvs_partition_gen>`.
- For offline NVS partition analysis, you can use the :doc:`NVS Partition Parser Utility <../api-reference/storage/nvs_partition_parse>`.
- For more information about read-only NVS partitions, see the :ref:`Read-only NVS <read-only-nvs>`.

**Examples:**

- :example:`storage/nvs/nvs_rw_value` demonstrates how to use NVS to write and read a single integer value.
- :example:`storage/nvs/nvs_rw_blob` demonstrates how to use NVS to write and read a blob.
- :example:`security/nvs_encryption_hmac` demonstrates NVS encryption using the HMAC peripheral, where the encryption keys are derived from the HMAC key burnt in eFuse.
- :example:`security/flash_encryption` demonstrates the flash encryption workflow including NVS partition creation and usage.


File handling design considerations
-----------------------------------

Here are several recommendation for building reliable storage features into your application:

* Use C Standard Library file APIs (ISO or POSIX) wherever possible. This high-level interface guarantees you will not need to change much, if it comes for instance to switching to a different file system. All the ESP-IDF supported file systems work as underlying layer for C STDLIB calls, so the specific file system details are nearly transparent to the application code. The only parts unique to each single system are formatting, mounting and diagnostic/repair functions
* Keep the file system dependent code separated, use wrappers to allow minimum change updates
* Design reasonable structure of your application file storage:
    * Distribute the load evenly, if possible. Use meaningful number of directories/subdirectories (for instance FAT12 can keep only 224 records in its root directory).
    * Avoid using too many files or too large files (though the latter usually causes less troubles than the former). Each file equals to a record in the system's internal "database", which can easily end up in the necessary overhead consuming more space than the data stored. Even worse case is exhausting the filesystem's resources and subsequent failure of the application - which can happen really quickly in embedded systems' environment.
    * Be cautious about number of write or erase operations performed in SPI Flash memory (for example, each write in the FatFS involves full erase of the area to be written). NOR Flash devices typically survive 100,000+ erase cycles per sector, and their lifetime is extended by the Wear-Levelling mechanism (implemented as a standalone component in corresponding driver stack, transparent from the application's perspective). The Wear-Levelling algorithm rotates the Flash memory sectors all around given partition space, so it requires some disk space available for the virtual sector shuffle. If you create "well-tailored" partition with the minimum space needed and manage to fill it with your application data, the Wear Levelling becomes ineffective and your device would degrade quickly. Projects with Flash write frequency around 500ms are fully capable to destroy average ESP32 flash in few days time (real world example).
    * With the previous point given, consider using reasonably large partitions to ensure safe margins for your data. It is usually cheaper to invest into extra Flash space than to forcibly resolve troubles unexpectedly happening in the field.
    * Think twice before deciding for specific file system - they are not 100% equal and each application has own strategy and requirements. For instance, the NVS is not suitable for storing a production data, as its design doesn't deal well with too many items being stored (recommended maximum for NVS partition size would be around 128 kB).


Encrypting partitions
---------------------

{IDF_TARGET_NAME} based chips provide several features to encrypt the contents of various partitions within chip's main SPI flash memory. All the necessary information can be found in chapters :doc:`Flash Encryption <../security/flash-encryption>` and :doc:`NVS Encryption <../api-reference/storage/nvs_encryption>`. Both variants use the AES family of algorithms, the Flash Encryption provides hardware-driven encryption scheme and is transparent from the software's perspective, whilst the NVS Encryption is a software feature implemented using mbedTLS component (though the mbedTLS can internally use the AES hardware accelerator, if available on given chip model). The latter requires the Flash Encryption enabled as the NVS Encryption needs a proprietary encrypted partition to hold its keys, and the NVS internal structure is not compatible with the Flash Encryption design. Therefore, both features come separate.

Considering the storage security scheme and the design of {IDF_TARGET_NAME} chips, there are several implications that may not be fully obvious in the main documents:

* The Flash encryption applies only to the main SPI Flash memory, due to its cache module design (all the "transparent" encryption APIs run over this cache). This implies that external flash partitions cannot be encrypted using the native Flash Encryption means.
* External partition encryption can be deployed by implementing custom encrypt/decrypt code in appropriate driver APIs - either by implementing own SPI flash driver (see :example:`storage/custom_flash_driver`) or by customizing higher levels in the driver stack, for instance by providing own :ref:`FatFS disk IO layer <fatfs-diskio-layer>`.
