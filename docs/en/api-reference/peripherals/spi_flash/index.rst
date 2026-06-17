SPI Flash API
=============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The spi_flash component contains API functions related to reading, writing, erasing, and memory mapping for data in the external flash.

For higher-level API functions which work with partitions defined in the :doc:`partition table </api-guides/partition-tables>`, see :doc:`/api-reference/storage/partition`

.. note::

    ``esp_partition_*`` APIs are recommended to be used instead of the lower level ``esp_flash_*`` API functions when accessing the main SPI flash chip, since they conduct bounds checking and are guaranteed to calculate correct offsets in flash based on the information in the partition table. ``esp_flash_*`` functions can still be used directly when accessing an external (secondary) SPI flash chip.

Different from the API before ESP-IDF v4.0, the functionality of ``esp_flash_*`` APIs is not limited to the "main" SPI flash chip (the same SPI flash chip from which program runs). With different chip pointers, you can access external flash chips connected to not only SPI0/1 but also other SPI buses like SPI2.

.. note::

    Instead of going through the cache connected to the SPI0 peripheral, most ``esp_flash_*`` APIs go through other SPI peripherals like SPI1, SPI2, etc. This makes them able to access not only the main flash, but also external (secondary) flash.

    However, due to the limitations of the cache, operations through the cache are limited to the main flash. The address range limitation for these operations is also on the cache side. The cache is not able to access external flash chips or address range above its capabilities. These cache operations include: mmap, encrypted read/write, executing code or access to variables in the flash.

.. note::

    Flash APIs after ESP-IDF v4.0 are no longer **atomic**. If a write operation occurs during another on-going read operation, and the flash addresses of both operations overlap, the data returned from the read operation may contain both old data and new data (that was updated written by the write operation).

.. note::

    Encrypted flash operations are only supported with the main flash chip (and not with other flash chips, that is on SPI1 with different CS, or on other SPI buses). Reading through cache is only supported on the main flash, which is determined by the HW.

Support for Features of Flash Chips
-----------------------------------

Quad/Dual Mode Chips
^^^^^^^^^^^^^^^^^^^^

Features of different flashes are implemented in different ways and thus need special support. The fast/slow read and Dual mode (DOUT/DIO) of almost all flashes with 24-bit address are supported, because they do not need any vendor-specific commands.

Quad mode (QIO/QOUT) is supported on the following chip types:

1. ISSI
2. GD
3. MXIC
4. FM
5. Winbond
6. XMC
7. BOYA

.. note::

    Only when one flash series listed above is supported by {IDF_TARGET_NAME}, this flash series is supported by the chip driver by default. You can use ``Component config`` > ``SPI Flash driver`` > ``Auto-detect flash chips`` in menuconfig to enable/disable a flash series.

Optional Features
^^^^^^^^^^^^^^^^^

.. toctree::
    :hidden:

    spi_flash_optional_feature

There are some features that are not supported by all flash chips, or not supported by all Espressif chips. These features include:

.. only:: esp32s3

    -  OPI flash - means that flash supports octal mode.

-  32-bit address flash - usually means that the flash has higher capacity (equal to or larger than 16 MB) that needs longer addresses.

.. only:: esp32s3

    -  High performance mode (HPM) - means that flash works under high frequency which is higher than 80 MHz.

-  Flash unique ID - means that flash supports its unique 64-bit ID.

.. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND

    -  Suspend & Resume - means that flash can accept suspend/resume command during its writing/erasing. The {IDF_TARGET_NAME} may keep the cache on when the flash is being written/erased and suspend it to read its contents randomly.

If you want to use these features, please ensure both {IDF_TARGET_NAME} and ALL flash chips in your product support these features. For more details, refer to :doc:`spi_flash_optional_feature`.

You may also customise your own flash chip driver. See :doc:`spi_flash_override_driver` for more details.

.. toctree::
   :hidden:

   Custom Flash Driver <spi_flash_override_driver>

Initializing a Flash Device
---------------------------

To use the ``esp_flash_*`` APIs, you need to initialise a flash chip on a certain SPI bus, as shown below:

1. Call :cpp:func:`spi_bus_initialize` to properly initialize an SPI bus. This function initializes the resources (I/O, DMA, interrupts) shared among devices attached to this bus.

2. Call :cpp:func:`spi_bus_add_flash_device` to attach the flash device to the bus. This function allocates memory and fills the members for the ``esp_flash_t`` structure. The CS I/O is also initialized here.

3. Call :cpp:func:`esp_flash_init` to actually communicate with the chip. This also detects the chip type, and influence the following operations.

.. note:: Multiple flash chips can be attached to the same bus now.

SPI Flash Access API
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

The SPI flash size is configured by writing a field in the ESP-IDF second stage bootloader image header, flashed at offset 0x1000.

By default, the SPI flash size is detected by ``esptool`` when this bootloader is written to flash, and the header is updated with the correct size. Alternatively, it is possible to generate a fixed flash size by setting :ref:`CONFIG_ESPTOOLPY_FLASHSIZE` in the project configuration.

If it is necessary to override the configured flash size at runtime, it is possible to set the ``chip_size`` member of the ``g_rom_flashchip`` structure. This size is used by ``esp_flash_*`` functions (in both software & ROM) to check the bounds.

Concurrency Constraints for Flash on SPI1
-----------------------------------------

.. toctree::
    :hidden:

    spi_flash_concurrency

.. attention::

   The SPI0/1 bus is shared between the instruction & data cache (for firmware execution) and the SPI1 peripheral (controlled by the drivers including this SPI flash driver). Hence, calling SPI Flash API on SPI1 bus (including the main flash) causes significant influence to the whole system. See :doc:`spi_flash_concurrency` for more details.


SPI Flash Encryption
--------------------

It is possible to encrypt the contents of SPI flash and have it transparently decrypted by hardware.

Refer to the :doc:`Flash Encryption documentation </security/flash-encryption>` for more details.

Memory Mapping API
------------------

{IDF_TARGET_CACHE_SIZE:default="64 KB", esp32c2=16~64 KB}

{IDF_TARGET_NAME} features memory hardware which allows regions of flash memory to be mapped into instruction and data address spaces. This mapping works only for read operations. It is not possible to modify contents of flash memory by writing to a mapped memory region.

Mapping happens in {IDF_TARGET_CACHE_SIZE} pages. Memory mapping hardware can map flash into the data address space and the instruction address space. See the technical reference manual for more details and limitations about memory mapping hardware.

Note that some pages are used to map the application itself into memory, so the actual number of available pages may be less than the capability of the hardware.

Reading data from flash using a memory mapped region is the only way to decrypt contents of flash when :doc:`flash encryption </security/flash-encryption>` is enabled. Decryption is performed at the hardware level.

Memory mapping API are declared in ``spi_flash_mmap.h`` and ``esp_partition.h``:

- :cpp:func:`spi_flash_mmap` maps a region of physical flash addresses into instruction space or data space of the CPU.
- :cpp:func:`spi_flash_munmap` unmaps previously mapped region.
- :cpp:func:`esp_partition_mmap` maps part of a partition into the instruction space or data space of the CPU.

Differences between :cpp:func:`spi_flash_mmap` and :cpp:func:`esp_partition_mmap` are as follows:

- :cpp:func:`spi_flash_mmap` must be given a {IDF_TARGET_CACHE_SIZE} aligned physical address.
- :cpp:func:`esp_partition_mmap` may be given any arbitrary offset within the partition. It adjusts the returned pointer to mapped memory as necessary.

Note that since memory mapping happens in pages, it may be possible to read data outside of the partition provided to ``esp_partition_mmap``, regardless of the partition boundary.

.. note::

    mmap is supported by cache, so it can only be used on main flash.

SPI Flash Implementation
------------------------

.. note::

    The header files in ``components/spi_flash/include/esp_flash_chips/`` directory are **semi-public** - they are intended for expert users who need to implement custom chip drivers for unsupported flash chips, but they are **not considered stable API** and may change without notice. For most use cases, you should use the public APIs in ``esp_flash.h`` instead.

The ``esp_flash_t`` structure holds chip data as well as three important parts of this API:

1. The host driver, which provides the hardware support to access the chip;
2. The chip driver, which provides compatibility service to different chips;
3. The OS functions, provide support of some OS functions (e.g., lock, delay) in different stages (1st/2nd boot, or the app).

Host Driver
^^^^^^^^^^^

The host driver relies on an interface (``spi_flash_host_driver_t``) defined in the ``spi_flash_types.h`` (in the ``esp_hal_mspi/include/hal`` folder). This interface provides some common functions to communicate with the chip.

In other files of the SPI HAL, some of these functions are implemented with existing {IDF_TARGET_NAME} memory-spi functionalities. However, due to the speed limitations of {IDF_TARGET_NAME}, the HAL layer cannot provide high-speed implementations to some reading commands (so the support for it was dropped). The files (``memspi_host_driver.h`` and ``.c``) implement the high-speed version of these commands with the ``common_command`` function provided in the HAL, and wrap these functions as ``spi_flash_host_driver_t`` for upper layer to use.

You can also implement your own host driver, even with the GPIO. As long as all the functions in the ``spi_flash_host_driver_t`` are implemented, the esp_flash API can access the flash regardless of the low-level hardware.

Chip Driver
^^^^^^^^^^^

The chip driver, defined in ``esp_flash_chips/spi_flash_chip_driver.h``, wraps basic functions provided by the host driver for the API layer to use.

Some operations need some commands to be sent first, or read some status afterwards. Some chips need different commands or values, or need special communication ways.

There is a type of chip called ``generic chip`` which stands for common chips. Other special chip drivers can be developed on the base of the generic chip.

The chip driver relies on the host driver.

.. _esp_flash_os_func:

OS Functions
^^^^^^^^^^^^

.. toctree::
   :hidden:

   ../spi_features

Currently the OS function layer provides entries of a lock and delay.

The lock (see :ref:`spi_bus_lock`) is used to resolve the conflicts among the access of devices on the same SPI bus, and the SPI Flash chip access. E.g.

1. On SPI1 bus, the cache (used to fetch the data (code) in the Flash and PSRAM) should be disabled when the flash chip on the SPI0/1 is being accessed.

2. On the other buses, the flash driver needs to disable the ISR registered by SPI Master driver, to avoid conflicts.

3. Some devices of SPI Master driver may require to use the bus monopolized during a period (especially when the device does not have a CS wire, or the wire is controlled by software like SDSPI driver).

The delay is used by some long operations which requires the master to wait or polling periodically.

The top API wraps these the chip driver and OS functions into an entire component, and also provides some argument checking.

OS functions can also help to avoid a watchdog timeout when erasing large flash areas. During this time, the CPU is occupied with the flash erasing task. This stops other tasks from being executed. Among these tasks is the idle task to feed the watchdog timer (WDT). If the configuration option :ref:`CONFIG_ESP_TASK_WDT_PANIC` is selected and the flash operation time is longer than the watchdog timeout period, the system will reboot.

It is pretty hard to totally eliminate this risk, because the erasing time varies with different flash chips, making it hard to be compatible in flash drivers. Therefore, users need to pay attention to it. Please use the following guidelines:

1. It is recommended to enable the :ref:`CONFIG_SPI_FLASH_YIELD_DURING_ERASE` option to allow the scheduler to re-schedule during erasing flash memory. Besides, following parameters can also be used.

- Increase :ref:`CONFIG_SPI_FLASH_ERASE_YIELD_TICKS` or decrease :ref:`CONFIG_SPI_FLASH_ERASE_YIELD_DURATION_MS` in menuconfig.
- You can also increase :ref:`CONFIG_ESP_TASK_WDT_TIMEOUT_S` in menuconfig for a larger watchdog timeout period. However, with larger watchdog timeout period, previously detected timeouts may no longer be detected.

2. Please be aware of the consequences of enabling the :ref:`CONFIG_ESP_TASK_WDT_PANIC` option when doing long-running SPI flash operations which triggers the panic handler when it times out. However, this option can also help dealing with unexpected exceptions in your application. Please decide whether this is needed to be enabled according to actual condition.

3. During your development, please carefully review the actual flash operation according to the specific requirements and time limits on erasing flash memory of your projects. Always allow reasonable redundancy based on your specific product requirements when configuring the flash erasing timeout threshold, thus improving the reliability of your product.

.. _spi-flash-implementation-details:

Implementation Details
----------------------

In order to perform some flash operations, it is necessary to make sure that both CPUs are not running any code from flash for the duration of the flash operation:

- In a single-core setup, the SDK needs to disable interrupts or scheduler before performing the flash operation.
- In a dual-core setup, the SDK needs to make sure that both CPUs are not running any code from flash.

When SPI flash API is called on CPU A (can be PRO or APP), start the ``spi_flash_op_block_func`` function on CPU B using the ``esp_ipc_call`` API. This API wakes up a high priority task on CPU B and tells it to execute a given function, in this case, ``spi_flash_op_block_func``. This function disables cache on CPU B and signals that the cache is disabled by setting the ``s_flash_op_can_start`` flag. Then the task on CPU A disables cache as well and proceeds to execute flash operation.

While a flash operation is running, interrupts can still run on CPUs A and B. It is assumed that all interrupt code is placed into RAM. Once the interrupt allocation API is added, a flag should be added to request the interrupt to be disabled for the duration of a flash operations.

Once the flash operation is complete, the function on CPU A sets another flag, ``s_flash_op_complete``, to let the task on CPU B know that it can re-enable cache and release the CPU. Then the function on CPU A re-enables the cache on CPU A as well and returns control to the calling code.

Additionally, all API functions are protected with a mutex (``s_flash_op_mutex``).

In a single core environment (:ref:`CONFIG_FREERTOS_UNICORE` enabled), you need to disable both caches, so that no inter-CPU communication can take place.

.. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND

    .. _internal_memory_saving_for_flash_driver:

    Internal Memory Saving For Flash Driver
    ---------------------------------------

    The ESP-IDF provides options to optimize the usage of IRAM by selectively placing certain functions into flash memory via turning off :ref:`CONFIG_SPI_FLASH_PLACE_FUNCTIONS_IN_IRAM`. It allows SPI flash operation functions to be executed from flash memory instead of IRAM. Thus it saves IRAM memory for other significant time-critical functions or tasks.

    However, this has some implications for flash itself. Functions placed into flash memory may have slightly increased execution times compared to those placed in IRAM. Applications with strict timing requirements or those heavily reliant on SPI flash operations may need to evaluate the trade-offs before enabling this option.

    .. note::

        :ref:`CONFIG_SPI_FLASH_PLACE_FUNCTIONS_IN_IRAM` should not be turned off when :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND` is not enabled, otherwise it will cause critical crash. As for flash suspend feature, please refer to :ref:`auto-suspend` for more information.

    Resource Consumption
    ^^^^^^^^^^^^^^^^^^^^

    Use the :doc:`/api-guides/tools/idf-size` tool to check the code and data consumption of the SPI flash driver. The following are the test results under 2 different conditions (using ESP32-C2 as an example):

    **Note that the following data are not exact values and are for reference only; they may differ on different chip models.**

    Resource consumption when :ref:`CONFIG_SPI_FLASH_PLACE_FUNCTIONS_IN_IRAM` is enabled:

    .. list-table:: Resource Consumption
       :widths: 20 10 10 10 10 10 10 10 10 10
       :header-rows: 1

       * - Component Layer
         - Total Size
         - DIRAM
         - .bss
         - .data
         - .text
         - Flash Code
         - .text
         - Flash Data
         - .rodata
       * - hal
         - 4624
         - 4038
         - 0
         - 0
         - 4038
         - 586
         - 586
         - 0
         - 0
       * - spi_flash
         - 14074
         - 11597
         - 82
         - 1589
         - 9926
         - 2230
         - 2230
         - 247
         - 247

    Resource consumption when :ref:`CONFIG_SPI_FLASH_PLACE_FUNCTIONS_IN_IRAM` is disabled:

    .. list-table:: Resource Consumption
       :widths: 20 10 10 10 10 10 10 10 10 10
       :header-rows: 1

       * - Component Layer
         - Total Size
         - DIRAM
         - .bss
         - .data
         - .text
         - Flash Code
         - .text
         - Flash Data
         - .rodata
       * - hal
         - 4632
         - 0
         - 0
         - 0
         - 0
         - 4632
         - 4632
         - 0
         - 0
       * - spi_flash
         - 14569
         - 1399
         - 22
         - 429
         - 948
         - 11648
         - 11648
         - 1522
         - 1522

Related Documents
------------------

.. list::

    - :doc:`spi_flash_optional_feature`
    - :doc:`spi_flash_concurrency`
    :CONFIG_ESP_ROM_HAS_SPI_FLASH: - :doc:`spi_flash_idf_vs_rom`

.. toctree::
    :hidden:

    spi_flash_idf_vs_rom


API Reference - SPI Flash
-------------------------

.. include-build-file:: inc/esp_flash_spi_init.inc
.. include-build-file:: inc/esp_flash.inc
.. include-build-file:: inc/spi_flash_mmap.inc
.. include-build-file:: inc/spi_flash_types.inc
.. include-build-file:: inc/esp_flash_err.inc
.. include-build-file:: inc/esp_spi_flash_counters.inc

API Reference - Flash Encrypt
-----------------------------

.. include-build-file:: inc/esp_flash_encrypt.inc
