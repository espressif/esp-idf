Partition Tables
================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

A single {IDF_TARGET_NAME}'s flash can contain multiple apps, as well as many different kinds of data (calibration data, filesystems, parameter storage, etc). For this reason a partition table is flashed to (:ref:`default offset <CONFIG_PARTITION_TABLE_OFFSET>`) 0x8000 in the flash.

The partition table length is 0xC00 bytes, as we allow a maximum of 95 entries. An MD5 checksum, used for checking the integrity of the partition table at runtime, is appended after the table data. Thus, the partition table occupies an entire flash sector, which size is 0x1000 (4 KB). As a result, any partition following it must be at least located at (:ref:`default offset <CONFIG_PARTITION_TABLE_OFFSET>`) + 0x1000.

Each entry in the partition table has a name (label), type (app, data, or something else), subtype and the offset in flash where the partition is loaded.

The simplest way to use the partition table is to open the project configuration menu (``idf.py menuconfig``) and choose one of the simple predefined partition tables under :ref:`CONFIG_PARTITION_TABLE_TYPE`:

* "Single factory app, no OTA"
* "Factory app, two OTA definitions"

In both cases the factory app is flashed at offset 0x10000. If you execute ``idf.py partition-table`` then it will print a summary of the partition table.

Built-in Partition Tables
-------------------------

Here is the summary printed for the "Single factory app, no OTA" configuration:

.. code-block:: none

    # ESP-IDF Partition Table
    # Name,   Type, SubType, Offset,  Size, Flags
    nvs,      data, nvs,     0x9000,  0x6000,
    phy_init, data, phy,     0xf000,  0x1000,
    factory,  app,  factory, 0x10000, 1M,

* At a 0x10000 (64 KB) offset in the flash is the app labelled "factory". The bootloader runs this app by default.
* There are also two data regions defined in the partition table for storing NVS library partition and PHY init data.

Here is the summary printed for the "Factory app, two OTA definitions" configuration:

.. code-block:: none

    # ESP-IDF Partition Table
    # Name,   Type, SubType, Offset,  Size, Flags
    nvs,      data, nvs,     0x9000,  0x4000,
    otadata,  data, ota,     0xd000,  0x2000,
    phy_init, data, phy,     0xf000,  0x1000,
    factory,  app,  factory, 0x10000,  1M,
    ota_0,    app,  ota_0,   0x110000, 1M,
    ota_1,    app,  ota_1,   0x210000, 1M,

* There are now three app partition definitions. The type of the factory app (at 0x10000) and the next two "OTA" apps are all set to "app", but their subtypes are different.
* There is also a new "otadata" slot, which holds the data for OTA updates. The bootloader consults this data in order to know which app to execute. If "ota data" is empty, it will execute the factory app.

Creating Custom Tables
----------------------

If you choose "Custom partition table CSV" in ``menuconfig``, then you can also enter the name of a CSV file (in the project directory) to use for your partition table. The CSV file can describe any number of definitions for the table you need.

The CSV format is the same format as printed in the summaries shown above. However, not all fields are required in the CSV. For example, here is the "input" CSV for the OTA partition table:

.. code-block:: none

    # Name,   Type, SubType,  Offset,   Size,  Flags
    nvs,      data, nvs,      0x9000,  0x4000
    otadata,  data, ota,      0xd000,  0x2000
    phy_init, data, phy,      0xf000,  0x1000
    factory,  app,  factory,  0x10000,  1M
    ota_0,    app,  ota_0,    ,         1M
    ota_1,    app,  ota_1,    ,         1M
    nvs_key,  data, nvs_keys, ,        0x1000

* Whitespace between fields is ignored, and so is any line starting with ``#`` (comments).
* Each non-comment line in the CSV file is a partition definition.
* If you change the value of :ref:`CONFIG_PARTITION_TABLE_OFFSET`, you should update any fixed ``Offset`` in your CSV file to avoid overlaps with the new partition table location. Alternatively, leaving the ``Offset`` field blank allows the ``gen_esp32part.py`` tool to automatically calculate the correct offset based on the current partition table offset and alignment requirements.

Here is an example of a CSV partition table that includes bootloader and partition table partitions:

.. code-block:: none

    # ESP-IDF Partition Table
    # Name,           Type,            SubType,  Offset,  Size,     Flags
    bootloader,       bootloader,      primary,  N/A,     N/A,
    partition_table,  partition_table, primary,  N/A,     N/A,
    nvs,              data,            nvs,      ,        0x6000,
    phy_init,         data,            phy,      ,        0x1000,
    factory,          app,             factory,  ,        1M,
    recoveryBloader,  bootloader,      recovery, N/A,     N/A,

The ``gen_esp32part.py`` tool will replace each ``N/A`` with appropriate values based on the selected Kconfig options: {IDF_TARGET_CONFIG_BOOTLOADER_OFFSET_IN_FLASH} for the bootloader offset and :ref:`CONFIG_PARTITION_TABLE_OFFSET` for the partition table offset.

Name Field
~~~~~~~~~~

Name field can be any meaningful name. It is not significant to the {IDF_TARGET_NAME}. The maximum length of names is 16 bytes, including one null terminator. Names longer than the maximum length will be truncated.

Type Field
~~~~~~~~~~

Partition type field can be specified as a name or a number 0-254 (or as hex 0x00-0xFE). Types 0x00-0x3F are reserved for ESP-IDF core functions.

- ``app`` (0x00),
- ``data`` (0x01),
- ``bootloader`` (0x02). By default, this partition is not included in any CSV partition table files in ESP-IDF because it is not required and does not impact the system's functionality. It is only useful for the bootloader OTA updates and flash partitioning. Even if this partition is not present in the CSV file, it is still possible to perform the OTA.
- ``partition_table`` (0x03). By default, this partition also is not included in any CSV partition table files in ESP-IDF.
- 0x40-0xFE are reserved for **custom partition types**. If your app needs to store data in a format not already supported by ESP-IDF, then use a value from this range.

See :cpp:type:`esp_partition_type_t` for the enum definitions for ``app`` and ``data`` partitions.

If writing in C++ then specifying a application-defined partition type requires casting an integer to :cpp:type:`esp_partition_type_t` in order to use it with the :ref:`partition API<api-reference-partition-table>`. For example:

.. code-block:: none

    static const esp_partition_type_t APP_PARTITION_TYPE_A = (esp_partition_type_t)0x40;

The bootloader ignores any partition types other than ``app`` (0x00) and ``data`` (0x01).

SubType
~~~~~~~

{IDF_TARGET_ESP_PHY_REF:default = ":ref:`CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION`", esp32p4, esp32c5, esp32c61="NOT UPDATED YET"}

The 8-bit SubType field is specific to a given partition type. ESP-IDF currently only specifies the meaning of the subtype field for ``app`` and ``data`` partition types.

See enum :cpp:type:`esp_partition_subtype_t` for the full list of subtypes defined by ESP-IDF, including the following:

.. only:: not esp32c6

  * When type is ``app``, the SubType field can be specified as ``factory`` (0x00), ``ota_0`` (0x10) ... ``ota_15`` (0x1F) or ``test`` (0x20).

.. only:: esp32c6

  * When type is ``app``, the SubType field can be specified as ``factory`` (0x00), ``ota_0`` (0x10) through ``ota_15`` (0x1F), or ``test`` (0x20). Additionally, if :doc:`ESP-TEE <../security/tee/tee>` functionality is enabled, two TEE-specific subtypes become available: ``tee_0`` (0x30) and ``tee_1`` (0x31).

    - ``factory`` (0x00) is the default app partition. The bootloader will execute the factory app unless there it sees a partition of type data/ota, in which case it reads this partition to determine which OTA image to boot.

        - OTA never updates the factory partition.
        - If you want to conserve flash usage in an OTA project, you can remove the factory partition and use ``ota_0`` instead.

    - ``ota_0`` (0x10) ... ``ota_15`` (0x1F) are the OTA app slots. When :doc:`OTA <../api-reference/system/ota>` is in use, the OTA data partition configures which app slot the bootloader should boot. When using OTA, an application should have at least two OTA application slots (``ota_0`` & ``ota_1``). Refer to the :doc:`OTA documentation <../api-reference/system/ota>` for more details.
    - ``test`` (0x20) is a reserved subtype for factory test procedures. It will be used as the fallback boot partition if no other valid app partition is found. It is also possible to configure the bootloader to read a GPIO input during each boot, and boot this partition if the GPIO is held low, see :ref:`bootloader_boot_from_test_firmware`.

    .. only:: esp32c6

      - ``tee_0`` (0x30) and ``tee_1`` (0x31) are the TEE app slots. When :doc:`TEE OTA <../security/tee/tee-ota>` is in use, the TEE OTA data partition configures which TEE app slot the bootloader should boot. When using TEE OTA, the partition table should have these two TEE app slots. Refer to the :doc:`TEE OTA documentation <../security/tee/tee-ota>` for more details.

* When type is ``bootloader``, the SubType field can be specified as:

    - ``primary`` (0x00). This is the 2nd stage bootloader, located at the {IDF_TARGET_CONFIG_BOOTLOADER_OFFSET_IN_FLASH} address in flash memory. The tool automatically determines the appropriate size and offset for this subtype, so any size or offset specified for this subtype will be ignored. You can either leave these fields blank or use ``N/A`` as a placeholder.
    - ``ota`` (0x01). This is a temporary bootloader partition used by the bootloader OTA update functionality to download a new image. The tool ignores the size for this subtype, allowing you to leave it blank or use ``N/A``. You can only specify an offset, or leave it blank to have the tool calculate it based on the offsets of previously used partitions.
    - ``recovery`` (0x02). This is the recovery bootloader partition used for safely performing OTA updates to the bootloader. The ``gen_esp32part.py`` tool automatically determines the address and size for this partition, so you can leave these fields blank or use ``N/A`` as a placeholder. The address must match an eFuse field, which is defined through a Kconfig option. If the normal bootloader loading path fails, the first stage (ROM) bootloader will attempt to load the recovery partition at the address specified by the eFuse field.

    The size of the bootloader type is calculated by the ``gen_esp32part.py`` tool  based on the specified ``--offset`` (the partition table offset) and ``--primary-partition-offset`` arguments. Specifically, the bootloader size is defined as (:ref:`CONFIG_PARTITION_TABLE_OFFSET` - {IDF_TARGET_CONFIG_BOOTLOADER_OFFSET_IN_FLASH}). This calculated size applies to all subtypes of the bootloader.

* When type is ``partition_table``, the SubType field can be specified as:

    - ``primary`` (0x00). This is the primary partition table, located at the :ref:`CONFIG_PARTITION_TABLE_OFFSET` address in flash memory. The tool automatically determines the appropriate size and offset for this subtype, so any size or offset specified for this subtype will be ignored. You can either leave these fields blank or use ``N/A`` as a placeholder.
    - ``ota`` (0x01). It is a temporary partition table partition used by the partition table OTA update functionality for downloading a new image. The tool ignores the size for this subtype, allowing you to leave it blank or use ``N/A``. You can specify an offset, or leave it blank, in which case the tool will calculate it based on the offsets of previously allocated partitions.

    The size for the ``partition_table`` type is fixed at ``0x1000`` and applies uniformly across all subtypes of ``partition_table``.

* When type is ``data``, the subtype field can be specified as ``ota`` (0x00), ``phy`` (0x01), ``nvs`` (0x02), nvs_keys (0x04), or a range of other component-specific subtypes (see :cpp:type:`subtype enum <esp_partition_subtype_t>`).

    - ``ota`` (0) is the :ref:`OTA data partition <ota_data_partition>` which stores information about the currently selected OTA app slot. This partition should be 0x2000 bytes in size. Refer to the :ref:`OTA documentation <ota_data_partition>` for more details.
    - ``phy`` (1) is for storing PHY initialisation data. This allows PHY to be configured per-device, instead of in firmware.

        - In the default configuration, the phy partition is not used and PHY initialisation data is compiled into the app itself. As such, this partition can be removed from the partition table to save space.
        - To load PHY data from this partition, open the project configuration menu (``idf.py menuconfig``) and enable {IDF_TARGET_ESP_PHY_REF} option. You will also need to flash your devices with phy init data as the esp-idf build system does not do this automatically.
    - ``nvs`` (2) is for the :doc:`Non-Volatile Storage (NVS) API <../api-reference/storage/nvs_flash>`.

        - NVS is used to store per-device PHY calibration data (different to initialisation data).

        .. only:: SOC_WIFI_SUPPORTED

            - NVS is used to store Wi-Fi data if the :doc:`esp_wifi_set_storage(WIFI_STORAGE_FLASH) <../api-reference/network/esp_wifi>` initialization function is used.

        - The NVS API can also be used for other application data.
        - It is strongly recommended that you include an NVS partition of at least 0x3000 bytes in your project.
        - If using NVS API to store a lot of data, increase the NVS partition size from the default 0x6000 bytes.
        - When NVS is used to store factory settings, it is recommended to keep these settings in a separate read-only NVS partition. The minimal size of a read-only NVS partition is 0x1000 bytes. See :ref:`read-only-nvs` for more details. ESP-IDF provides :doc:`NVS Partition Generator Utility </api-reference/storage/nvs_partition_gen>` to generate NVS partitions with factory settings and to flash them along with the application.
    - ``nvs_keys`` (4) is for the NVS key partition. See :doc:`Non-Volatile Storage (NVS) API <../api-reference/storage/nvs_flash>` for more details.

        - It is used to store NVS encryption keys when `NVS Encryption` feature is enabled.
        - The size of this partition should be 4096 bytes (minimum partition size).

    .. only:: esp32c6

            - ``tee_ota`` (0x90) is the :ref:`TEE OTA data partition <tee-ota-data-partition>` which stores information about the currently selected TEE OTA app slot. This partition should be 0x2000 bytes in size. Refer to the :doc:`TEE OTA documentation <../security/tee/tee-ota>` for more details.

    - There are other predefined data subtypes for data storage supported by ESP-IDF. These include:

        - ``coredump`` (0x03) is for storing core dumps while using a custom partition table CSV file. See :doc:`/api-guides/core_dump` for more details.
        - ``efuse`` (0x05) is for emulating eFuse bits using :ref:`virtual-efuses`.
        - ``undefined`` (0x06) is implicitly used for data partitions with unspecified (empty) subtype, but it is possible to explicitly mark them as undefined as well.
        - ``fat`` (0x81) is for :doc:`/api-reference/storage/fatfs`.
        - ``spiffs`` (0x82) is for :doc:`/api-reference/storage/spiffs`.
        - ``littlefs`` (0x83) is for `LittleFS filesystem <https://github.com/littlefs-project/littlefs>`_. See :example:`storage/littlefs` example for more details.

.. Comment: ``esphttpd`` (0x80) was not added to the list because there is no docs section for it and it is not clear whether user should use it explicitly.

    Other subtypes of ``data`` type are reserved for future ESP-IDF uses.

* If the partition type is any application-defined value (range 0x40-0xFE), then ``subtype`` field can be any value chosen by the application (range 0x00-0xFE).

    Note that when writing in C++, an application-defined subtype value requires casting to type :cpp:type:`esp_partition_subtype_t` in order to use it with the :ref:`partition API <api-reference-partition-table>`.

Extra Partition SubTypes
~~~~~~~~~~~~~~~~~~~~~~~~

A component can define a new partition subtype by setting the ``EXTRA_PARTITION_SUBTYPES`` property. This property is a CMake list, each entry of which is a comma separated string with ``<type>, <subtype>, <value>`` format. The build system uses this property to add extra subtypes and creates fields named ``ESP_PARTITION_SUBTYPE_<type>_<subtype>`` in :cpp:type:`esp_partition_subtype_t`. The project can use this subtype to define partitions in the partitions table CSV file and use the new fields in :cpp:type:`esp_partition_subtype_t`.

.. _partition-offset-and-size:

Offset & Size
~~~~~~~~~~~~~

.. list::

    - The offset represents the partition address in the SPI flash, which sector size is 0x1000 (4 KB). Thus, the offset must be a multiple of 4 KB.
    - Partitions with blank offsets in the CSV file will start after the previous partition, or after the partition table in the case of the first partition.
    - Partitions of type ``app`` have to be placed at offsets aligned to 0x10000 (64 KB). If you leave the offset field blank, ``gen_esp32part.py`` will automatically align the partition. If you specify an unaligned offset for an ``app`` partition, the tool will return an error.
    - Partitions of type ``app`` should have the size aligned to the flash sector size (4 KB). If you specify an unaligned size for an ``app`` partition, the tool will return an error.
    :SOC_SECURE_BOOT_V1: - If Secure Boot V1 is enabled, then the partition of type ``app`` needs to have size aligned to 0x10000 (64 KB) boundary.
    :SOC_SECURE_BOOT_V1: - The ``bootloader`` offset and size are not affected by the Secure Boot V1 option. Whether Secure Boot V1 is enabled or not, the bootloader remains the same size and does not include the secure digest, which is flashed at offset 0x0 in the flash and occupies one sector (4096 bytes).
    - Sizes and offsets can be specified as decimal numbers, hex numbers with the prefix 0x, or size multipliers K or M (1024 and 1024*1024 bytes).
    - For ``bootloader`` and ``partition_table`` types, specifying ``N/A`` for size and offset in the CSV file means that these values are automatically determined by the tool and cannot be manually defined. This requires setting the ``--offset`` and ``--primary-partition-offset`` arguments of ``gen_esp32part.py``.

.. note::

    If you want the partitions in the partition table to work relative to any placement (:ref:`CONFIG_PARTITION_TABLE_OFFSET`) of the table itself, leave the offset field (in CSV file) for all partitions blank. Similarly, if changing the partition table offset, then be aware that all blank partition offsets may change to match, and that any fixed offsets may now collide with the partition table (causing an error).

Flags
~~~~~

Two flags are currently supported, ``encrypted`` and ``readonly``:

    - If ``encrypted`` flag is set, the partition will be encrypted if :doc:`/security/flash-encryption` is enabled.

    .. note::

        The following type partitions will always be encrypted, regardless of whether this flag is set or not:

        .. list::

            - ``app``,
            - ``bootloader``,
            - ``partition_table``,
            - type ``data`` and subtype ``ota``,
            - type ``data`` and subtype ``nvs_keys``.

    - If ``readonly`` flag is set, the partition will be read-only. This flag is only supported for ``data`` type partitions except ``ota`` and ``coredump`` subtypes. This flag can help to protect against accidental writes to a partition that contains critical device-specific configuration data, e.g., factory data partition.

    .. note::

        Using C file I/O API to open a file (``fopen``) in any write mode (``w``, ``w+``, ``a``, ``a+``, ``r+``) will fail and return ``NULL``. Using ``open`` with any other flag than ``O_RDONLY`` will fail and return ``-1`` while ``errno`` global variable will be set to ``EROFS``. This is also true for any other POSIX syscall function performing write or erase operations. Opening a handle in read-write mode for NVS on a read-only partition will fail and return :c:macro:`ESP_ERR_NOT_ALLOWED` error code. Using a lower level API like ``esp_partition``, ``spi_flash``, etc. to write to a read-only partition will result in :c:macro:`ESP_ERR_NOT_ALLOWED` error code.

You can specify multiple flags by separating them with a colon. For example, ``encrypted:readonly``.

Generating Binary Partition Table
---------------------------------

The partition table which is flashed to the {IDF_TARGET_NAME} is in a binary format, not CSV. The tool :component_file:`partition_table/gen_esp32part.py` is used to convert between CSV and binary formats.

If you configure the partition table CSV name in the project configuration (``idf.py menuconfig``) and then build the project or run ``idf.py partition-table``, this conversion is done as part of the build process.

To convert CSV to Binary manually:

.. code-block:: none

    python gen_esp32part.py input_partitions.csv binary_partitions.bin

To convert binary format back to CSV manually:

.. code-block:: none

    python gen_esp32part.py binary_partitions.bin input_partitions.csv

To display the contents of a binary partition table on stdout (this is how the summaries displayed when running ``idf.py partition-table`` are generated:

.. code-block:: none

    python gen_esp32part.py binary_partitions.bin

Partition Size Checks
---------------------

The ESP-IDF build system will automatically check if generated binaries fit in the available partition space, and will fail with an error if a binary is too large.

Currently these checks are performed for the following binaries:

* Bootloader binary must fit in space before partition table (see :ref:`bootloader-size`).
* App binary should fit in at least one partition of type "app". If the app binary does not fit in any app partition, the build will fail. If it only fits in some of the app partitions, a warning is printed about this.

.. note::

    Although the build process will fail if the size check returns an error, the binary files are still generated and can be flashed (although they may not work if they are too large for the available space.)

MD5 Checksum
~~~~~~~~~~~~

The binary format of the partition table contains an MD5 checksum computed based on the partition table. This checksum is used for checking the integrity of the partition table during the boot.

.. only:: esp32

    The MD5 checksum generation can be disabled by the ``--disable-md5sum`` option of ``gen_esp32part.py`` or by the :ref:`CONFIG_PARTITION_TABLE_MD5` option. This is useful for example when one :ref:`uses a bootloader from ESP-IDF before v3.1 <CONFIG_APP_COMPATIBLE_PRE_V3_1_BOOTLOADERS>` which cannot process MD5 checksums and the boot fails with the error message ``invalid magic number 0xebeb``.

.. only:: not esp32

    The MD5 checksum generation can be disabled by the ``--disable-md5sum`` option of ``gen_esp32part.py`` or by the :ref:`CONFIG_PARTITION_TABLE_MD5` option.


Flashing the Partition Table
----------------------------

* ``idf.py partition-table-flash``: will flash the partition table with esptool.py.
* ``idf.py flash``: Will flash everything including the partition table.

A manual flashing command is also printed as part of ``idf.py partition-table`` output.

.. note::

    Note that updating the partition table does not erase data that may have been stored according to the old partition table. You can use ``idf.py erase-flash`` (or ``esptool.py erase_flash``) to erase the entire flash contents.


Partition Tool (``parttool.py``)
--------------------------------

The component `partition_table` provides a tool :component_file:`parttool.py <partition_table/parttool.py>` for performing partition-related operations on a target device. The following operations can be performed using the tool:

    - reading a partition and saving the contents to a file (read_partition)
    - writing the contents of a file to a partition (write_partition)
    - erasing a partition (erase_partition)
    - retrieving info such as name, offset, size and flag ("encrypted") of a given partition (get_partition_info)

The tool can either be imported and used from another Python script or invoked from shell script for users wanting to perform operation programmatically. This is facilitated by the tool's Python API and command-line interface, respectively.

Python API
~~~~~~~~~~~

Before anything else, make sure that the `parttool` module is imported.

.. code-block:: python

    import sys
    import os

    idf_path = os.environ["IDF_PATH"]  # get value of IDF_PATH from environment
    parttool_dir = os.path.join(idf_path, "components", "partition_table")  # parttool.py lives in $IDF_PATH/components/partition_table

    sys.path.append(parttool_dir)  # this enables Python to find parttool module
    from parttool import *  # import all names inside parttool module

The starting point for using the tool's Python API to do is create a `ParttoolTarget` object:

.. code-block:: python

    # Create a parttool.py target device connected on serial port /dev/ttyUSB1
    target = ParttoolTarget("/dev/ttyUSB1")

The created object can now be used to perform operations on the target device:

.. code-block:: python

    # Erase partition with name 'storage'
  target.erase_partition(PartitionName("storage"))

    # Read partition with type 'data' and subtype 'spiffs' and save to file 'spiffs.bin'
    target.read_partition(PartitionType("data", "spiffs"), "spiffs.bin")

    # Write to partition 'factory' the contents of a file named 'factory.bin'
    target.write_partition(PartitionName("factory"), "factory.bin")

    # Print the size of default boot partition
    storage = target.get_partition_info(PARTITION_BOOT_DEFAULT)
    print(storage.size)

The partition to operate on is specified using `PartitionName` or `PartitionType` or PARTITION_BOOT_DEFAULT. As the name implies, these can be used to refer to partitions of a particular name, type-subtype combination, or the default boot partition.

More information on the Python API is available in the docstrings for the tool.

Command-line Interface
~~~~~~~~~~~~~~~~~~~~~~

The command-line interface of `parttool.py` has the following structure:

.. code-block:: bash

    parttool.py [command-args] [subcommand] [subcommand-args]

    - command-args - These are arguments that are needed for executing the main command (parttool.py), mostly pertaining to the target device
    - subcommand - This is the operation to be performed
    - subcommand-args - These are arguments that are specific to the chosen operation

.. code-block:: bash

    # Erase partition with name 'storage'
    parttool.py --port "/dev/ttyUSB1" erase_partition --partition-name=storage

    # Read partition with type 'data' and subtype 'spiffs' and save to file 'spiffs.bin'
    parttool.py --port "/dev/ttyUSB1" read_partition --partition-type=data --partition-subtype=spiffs --output "spiffs.bin"

    # Write to partition 'factory' the contents of a file named 'factory.bin'
    parttool.py --port "/dev/ttyUSB1" write_partition --partition-name=factory --input "factory.bin"

    # Print the size of default boot partition
    parttool.py --port "/dev/ttyUSB1" get_partition_info --partition-boot-default --info size

.. note::

    If the device has already enabled ``Flash Encryption`` or ``Secure Boot``, attempting to use commands that modify the flash content, such as ``erase_partition`` or ``write_partition``, will result in an error. This error is generated by the erase command of ``esptool.py``, which is called first before writing. This error is done as a safety measure to prevent bricking your device.

    .. code-block:: none

        A fatal error occurred: Active security features detected, erasing flash is disabled as a safety measure. Use --force to override, please use with caution, otherwise it may brick your device!

    To work around this, you need use the ``--force`` flag with ``esptool.py``. Specifically, the ``parttool.py`` provides the ``--esptool-erase-args`` argument that help to pass this flag to ``esptool.py``.

    .. code-block:: bash

        # Erase partition with name 'storage'
        # If Flash Encryption or Secure Boot are enabled then add "--esptool-erase-args=force"
        parttool.py --port "/dev/ttyUSB1" --esptool-erase-args=force erase_partition --partition-name=storage

        # Write to partition 'factory' the contents of a file named 'factory.bin'
        # If Flash Encryption or Secure Boot are enabled then add "--esptool-erase-args=force"
        parttool.py --port "/dev/ttyUSB1" --esptool-erase-args=force write_partition --partition-name=factory --input "factory.bin"

More information can be obtained by specifying `--help` as argument:

.. code-block:: bash

    # Display possible subcommands and show main command argument descriptions
    parttool.py --help

    # Show descriptions for specific subcommand arguments
    parttool.py [subcommand] --help

.. _secure boot: security/secure-boot-v1.rst
