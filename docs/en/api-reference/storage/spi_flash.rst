SPI Flash API
=============

:link_to_translation:`zh_CN:[中文]`

Overview
--------
The spi_flash component contains API functions related to reading, writing, erasing, memory mapping for data in the external flash. The spi_flash component also has higher-level API functions which work with partitions defined in the :doc:`partition table </api-guides/partition-tables>`.

Different from the API before IDF v4.0, the functionality of `esp_flash_*` APIs is not limited to the "main" SPI flash chip (the same SPI flash chip from which program runs). With different chip pointers, you can access to external flash chips connected to not only SPI0/1 but also other SPI buses like SPI2.

.. note::

    Instead of through the cache connected to the SPI0 peripheral, most `esp_flash_*` APIs go through other SPI peripherals like SPI1, SPI2, etc.. This makes them able to access to not only the main flash, but also external flash.

    However due to limitations of the cache, operations through the cache are limited to the main flash. The address range limitation for these operations are also on the cache side. The cache is not able to access external flash chips or address range above its capabilities. These cache operations include: mmap, encrypted read/write, executing code or access to variables in the flash.

.. note::

    Flash APIs after IDF v4.0 are no longer *atomic*. A writing operation during another on-going read operation, on the overlapped flash address, may cause the return data from the read operation to be partly same as before, and partly updated as new written.


Kconfig option :ref:`CONFIG_SPI_FLASH_USE_LEGACY_IMPL` can be used to switch ``spi_flash_*`` functions back to the implementation before ESP-IDF v4.0. However, the code size may get bigger if you use the new API and the old API at the same time.

Encrypted reads and writes use the old implementation, even if :ref:`CONFIG_SPI_FLASH_USE_LEGACY_IMPL` is not enabled. As such, encrypted flash operations are only supported with the main flash chip (and not with other flash chips, that is on SPI1 with different CS, or on other SPI buses). Reading through cache is only supported on the main flash, which is determined by the HW.

Support for features of flash chips
-----------------------------------

Flash features of different vendors are operated in different ways and need special support. The fast/slow read and Dual mode (DOUT/DIO) of almost all 24-bits address flash chips are supported, because they don't need any vendor-specific commands.

Quad mode (QIO/QOUT) is supported on following chip types:

1. ISSI
2. GD
3. MXIC
4. FM
5. Winbond
6. XMC
7. BOYA

.. only:: esp32s3

    Octal mode (OPI) are supported on following chip types:

    1. MXIC

    To know how to configure menuconfig for a board with different Flash and PSRAM, please refer to the :ref:`SPI Flash and External SPI RAM Configuration <flash-psram-configuration>`

The 32-bit address range of following chip type is supported:

1. W25Q256
2. GD25Q256

Users can also customize their own flash chip driver, see :doc:`spi_flash_override_driver` for more details.

.. toctree::
   :hidden:

   Custom Flash Driver <spi_flash_override_driver>

Initializing a flash device
---------------------------

To use ``esp_flash_*`` APIs, you need to have a chip initialized on a certain SPI bus.

1. Call :cpp:func:`spi_bus_initialize` to properly initialize an SPI bus. This functions initialize the resources (I/O, DMA, interrupts) shared among devices attached to this bus.

2. Call :cpp:func:`spi_bus_add_flash_device` to attach the flash device onto the bus. This allocates memory, and fill the members for the ``esp_flash_t`` structure. The CS I/O is also initialized here.

3. Call :cpp:func:`esp_flash_init` to actually communicate with the chip. This will also detect the chip type, and influence the following operations.

.. note:: Multiple flash chips can be attached to the same bus now. However, using ``esp_flash_*`` devices and ``spi_device_*`` devices on the same SPI bus is not supported yet.

SPI flash access API
--------------------

This is the set of API functions for working with data in flash:

- :cpp:func:`esp_flash_read` reads data from flash to RAM
- :cpp:func:`esp_flash_write` writes data from RAM to flash
- :cpp:func:`esp_flash_erase_region` erases specific region of flash
- :cpp:func:`esp_flash_erase_chip` erases the whole flash
- :cpp:func:`esp_flash_get_chip_size` returns flash chip size, in bytes, as configured in menuconfig

Generally, try to avoid using the raw SPI flash functions to the "main" SPI flash chip in favour of :ref:`partition-specific functions <flash-partition-apis>`.

SPI Flash Size
--------------

The SPI flash size is configured by writing a field in the software bootloader image header, flashed at offset 0x1000.

By default, the SPI flash size is detected by esptool.py when this bootloader is written to flash, and the header is updated with the correct size. Alternatively, it is possible to generate a fixed flash size by setting :envvar:`CONFIG_ESPTOOLPY_FLASHSIZE` in project configuration.

If it is necessary to override the configured flash size at runtime, it is possible to set the ``chip_size`` member of the ``g_rom_flashchip`` structure. This size is used by ``esp_flash_*`` functions (in both software & ROM) to check the bounds.

Concurrency Constraints for flash on SPI1
-----------------------------------------

.. toctree::
    :hidden:

    spi_flash_concurrency

.. attention::

   The SPI0/1 bus is shared between the instruction & data cache (for firmware execution) and the SPI1 peripheral (controlled by the drivers including this SPI flash driver). Hence, calling SPI Flash API on SPI1 bus (including the main flash) will cause significant influence to the whole system. See :doc:`spi_flash_concurrency` for more details.

.. _flash-partition-apis:

Partition table API
-------------------

ESP-IDF projects use a partition table to maintain information about various regions of SPI flash memory (bootloader, various application binaries, data, filesystems). More information on partition tables can be found :doc:`here </api-guides/partition-tables>`.

This component provides API functions to enumerate partitions found in the partition table and perform operations on them. These functions are declared in ``esp_partition.h``:

- :cpp:func:`esp_partition_find` checks a partition table for entries with specific type, returns an opaque iterator.
- :cpp:func:`esp_partition_get` returns a structure describing the partition for a given iterator.
- :cpp:func:`esp_partition_next` shifts the iterator to the next found partition.
- :cpp:func:`esp_partition_iterator_release` releases iterator returned by ``esp_partition_find``.
- :cpp:func:`esp_partition_find_first` - a convenience function which returns the structure describing the first partition found by ``esp_partition_find``.
- :cpp:func:`esp_partition_read`, :cpp:func:`esp_partition_write`, :cpp:func:`esp_partition_erase_range` are equivalent to :cpp:func:`spi_flash_read`, :cpp:func:`spi_flash_write`, :cpp:func:`spi_flash_erase_range`, but operate within partition boundaries.

.. note::
    Application code should mostly use these ``esp_partition_*`` API functions instead of lower level ``esp_flash_*`` API functions. Partition table API functions do bounds checking and calculate correct offsets in flash, based on data stored in a partition table.


SPI Flash Encryption
--------------------

It is possible to encrypt the contents of SPI flash and have it transparently decrypted by hardware.

Refer to the :doc:`Flash Encryption documentation </security/flash-encryption>` for more details.

Memory mapping API
------------------

{IDF_TARGET_CACHE_SIZE:default="64 KB"}

{IDF_TARGET_NAME} features memory hardware which allows regions of flash memory to be mapped into instruction and data address spaces. This mapping works only for read operations. It is not possible to modify contents of flash memory by writing to a mapped memory region.

Mapping happens in {IDF_TARGET_CACHE_SIZE} pages. Memory mapping hardware can map flash into the data address space and the instruction address space. See the technical reference manual for more details and limitations about memory mapping hardware.

Note that some pages are used to map the application itself into memory, so the actual number of available pages may be less than the capability of the hardware.

Reading data from flash using a memory mapped region is the only way to decrypt contents of flash when :doc:`flash encryption </security/flash-encryption>` is enabled. Decryption is performed at the hardware level.

Memory mapping API are declared in ``esp_spi_flash.h`` and ``esp_partition.h``:

- :cpp:func:`spi_flash_mmap` maps a region of physical flash addresses into instruction space or data space of the CPU.
- :cpp:func:`spi_flash_munmap` unmaps previously mapped region.
- :cpp:func:`esp_partition_mmap` maps part of a partition into the instruction space or data space of the CPU.

Differences between :cpp:func:`spi_flash_mmap` and :cpp:func:`esp_partition_mmap` are as follows:

- :cpp:func:`spi_flash_mmap` must be given a {IDF_TARGET_CACHE_SIZE} aligned physical address.
- :cpp:func:`esp_partition_mmap` may be given any arbitrary offset within the partition, it will adjust the returned pointer to mapped memory as necessary.

Note that since memory mapping happens in pages, it may be possible to read data outside of the partition provided to ``esp_partition_mmap``, regardless of the partition boundary.

.. note::
    mmap is supported by cache, so it can only be used on main flash.

SPI Flash Implementation
------------------------

The ``esp_flash_t`` structure holds chip data as well as three important parts of this API:

1. The host driver, which provides the hardware support to access the chip;
2. The chip driver, which provides compatibility service to different chips;
3. The OS functions, provides support of some OS functions (e.g. lock, delay) in different stages (1st/2st boot, or the app).

Host driver
^^^^^^^^^^^

The host driver relies on an interface (``spi_flash_host_driver_t``) defined in the ``spi_flash_types.h`` (in the ``hal/include/hal`` folder). This interface provides some common functions to communicate with the chip.

In other files of the SPI HAL, some of these functions are implemented with existing {IDF_TARGET_NAME} memory-spi functionalities. However due to the speed limitations of {IDF_TARGET_NAME}, the HAL layer can't provide high-speed implementations to some reading commands (So we didn't do it at all). The files (``memspi_host_driver.h`` and ``.c``) implement the high-speed version of these commands with the ``common_command`` function provided in the HAL, and wrap these functions as ``spi_flash_host_driver_t`` for upper layer to use.

You can also implement your own host driver, even with the GPIO. As long as all the functions in the ``spi_flash_host_driver_t`` are implemented, the esp_flash API can access to the flash regardless of the low-level hardware.

Chip driver
^^^^^^^^^^^

The chip driver, defined in ``spi_flash_chip_driver.h``, wraps basic functions provided by the host driver for the API layer to use.

Some operations need some commands to be sent first, or read some status after. Some chips need different command or value, or need special communication ways.

There is a type of chip called ``generic chip`` which stands for common chips. Other special chip drivers can be developed on the base of the generic chip.

The chip driver relies on the host driver.

.. _esp_flash_os_func:

OS functions
^^^^^^^^^^^^

Currently the OS function layer provides entries of a lock and delay.

The lock (see :ref:`spi_bus_lock`) is used to resolve the conflicts among the access of devices on the same SPI bus, and the SPI Flash chip access. E.g.

1. On SPI1 bus, the cache (used to fetch the data (code) in the Flash and PSRAM) should be disabled when the flash chip on the SPI0/1 is being accessed.

2. On the other buses, the flash driver needs to disable the ISR registered by SPI Master driver, to avoid conflicts.

3. Some devices of SPI Master driver may requires to use the bus monopolized during a period. (especially when the device doesn't have CS wire, or the wire is controlled by the software like SDSPI driver).

The delay is used by some long operations which requires the master to wait or polling periodically.

The top API wraps these the chip driver and OS functions into an entire component, and also provides some argument checking.


See also
--------

- :doc:`Partition Table documentation <../../api-guides/partition-tables>`
- :doc:`Over The Air Update (OTA) API <../system/ota>` provides high-level API for updating app firmware stored in flash.
- :doc:`Non-Volatile Storage (NVS) API <nvs_flash>` provides a structured API for storing small pieces of data in SPI flash.


.. _spi-flash-implementation-details:

Implementation details
----------------------

In order to perform some flash operations, it is necessary to make sure that both CPUs are not running any code from flash for the duration of the flash operation:
- In a single-core setup, the SDK does it by disabling interrupts/scheduler before performing the flash operation.
- In a dual-core setup, this is slightly more complicated as the SDK needs to make sure that the other CPU is not running any code from flash.

When SPI flash API is called on CPU A (can be PRO or APP), start the ``spi_flash_op_block_func`` function on CPU B using the ``esp_ipc_call`` API. This API wakes up a high priority task on CPU B and tells it to execute a given function, in this case, ``spi_flash_op_block_func``. This function disables cache on CPU B and signals that the cache is disabled by setting the ``s_flash_op_can_start`` flag. Then the task on CPU A disables cache as well and proceeds to execute flash operation.

While a flash operation is running, interrupts can still run on CPUs A and B. It is assumed that all interrupt code is placed into RAM. Once the interrupt allocation API is added, a flag should be added to request the interrupt to be disabled for the duration of a flash operations.

Once the flash operation is complete, the function on CPU A sets another flag, ``s_flash_op_complete``, to let the task on CPU B know that it can re-enable cache and release the CPU. Then the function on CPU A re-enables the cache on CPU A as well and returns control to the calling code.

Additionally, all API functions are protected with a mutex (``s_flash_op_mutex``).

In a single core environment (:ref:`CONFIG_FREERTOS_UNICORE` enabled), you need to disable both caches, so that no inter-CPU communication can take place.

API Reference - SPI Flash
-------------------------

.. include-build-file:: inc/esp_flash_spi_init.inc
.. include-build-file:: inc/esp_flash.inc
.. include-build-file:: inc/spi_flash_types.inc

.. _api-reference-partition-table:

API Reference - Partition Table
-------------------------------

.. include-build-file:: inc/esp_partition.inc

API Reference - Flash Encrypt
-----------------------------

.. include-build-file:: inc/esp_flash_encrypt.inc
