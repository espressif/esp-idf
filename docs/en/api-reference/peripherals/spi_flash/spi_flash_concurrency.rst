.. _concurrency-constraints-flash:

Concurrency Constraints for Flash on SPI0/1
===========================================

:link_to_translation:`zh_CN:[中文]`

The SPI0/1 bus is shared between the cache and the SPI1 peripheral (controlled by the drivers including this SPI Flash driver). Operations to SPI1 may cause significant influence to the cache and hence the whole system. There are no such constraints and impacts for flash chips connected to other SPI buses, which are not covered in this document.

There are three kinds of activities that can happen on SPI0/1 bus:

- Flash writing operations (via SPI1). For example, erasing, page programming, or status register writing commands (e.g., ``SE``, ``PP``, and ``WRSR``). During these commands, the flash is in a unreadable state. The CPU and the cache have to wait until the writing command is completed. APIs below can trigger writing commands:

    - Calling non_encrypted SPI flash write API (:cpp:func:`esp_flash_write`, :cpp:func:`esp_flash_erase_region`, etc.)
    - Calling :cpp:func:`esp_flash_write_encrypted`

- Short operations (via SPI1, includes non-writing flash commands). APIs below can trigger short operations:

    .. list::

        - Calling non_encrypted SPI flash read API (:cpp:func:`esp_flash_read`, etc.)
        :esp32: - Or other drivers on SPI1 bus for user defined SPI operations (enable experimental feature :ref:`CONFIG_SPI_FLASH_SHARE_SPI1_BUS`)

- Cache read (via SPI0). Following API and operations can trigger cache read:

    - Code execution from SPI Flash or PSRAM
    - Fetch static data of .data/.rodata/.bss segment from SPI Flash or PSRAM
    - All other read/write operation to the PSRAM via the heap or `esp_himem`
    - Read from area mapped to SPI Flash, includes:

        - mmap-like functions: :cpp:func:`spi_flash_mmap`, :cpp:func:`spi_flash_mmap_pages`, :cpp:func:`esp_mmu_map`, :cpp:func:`bootloader_mmap`, and :cpp:func:`esp_partition_mmap`.
        - Functions relying on :cpp:func:`spi_flash_mmap`: :cpp:func:`esp_partition_find`, :cpp:func:`esp_partition_register_external`.
        - Encrypted flash read/write APIs :cpp:func:`esp_flash_read_encrypted` and :cpp:func:`esp_flash_write_encrypted` (on esp32, or for data validation).

.. only:: esp32

    Caches are disabled during all SPI1 operations. Most tasks will be disabled, and access to Flash/PSRAM is forbidden. See :ref:`cache_disabled` for more details.

.. only:: not esp32

    All SPI flash APIs are exclusive to each other by some internal mutex provided by the driver.

    For all SPI1 operations (read/write), caches are disabled during these operations by default. Most tasks will be disabled, and access to Flash/PSRAM is forbidden. See :ref:`cache_disabled` for more details.


.. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND or SOC_SPIRAM_XIP_SUPPORTED

    Some options help reduce the impact of cache disabling. The impact of write operations differs between modes.

    .. only:: SOC_SPIRAM_XIP_SUPPORTED

        - **XIP from PSRAM**: In this mode, all segments that were previously executed from Flash are loaded and executed from PSRAM instead. As a result, the cache can remain enabled while the flash is being erased or written, and code execution is not affected by write operations in most cases. See :ref:`xip_from_psram` for more details.

    .. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND

        - **Auto Suspend**: In this mode, when cache access to flash misses during flash erase/write operations, it is allowed to suspend the flash writing to read from it transparently with some latency. As a result, caches are kept enabled and code execution won't be affected so much during writing operations.

            This is an optional feature that depends on special SPI Flash models, hence disabled by default. See :doc:`spi_flash_optional_feature` and :ref:`auto-suspend` for more details.


See :ref:`esp_flash_os_func` and :ref:`spi_bus_lock` for the detailed information of software implementation.


.. _cache_disabled:

Cache Disabled (Default)
------------------------

.. only:: esp32

    Caches are disabled during SPI1 operations. All SPI1 operations will automatically and transparently disable the caches.

.. only:: not esp32

    By default, caches are disabled during SPI1 operations (read/write). All SPI1 operations will automatically and transparently disable the caches.

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    When the caches are disabled, all non-IRAM-safe interrupts will be disabled, and all other tasks are suspended. The other core will be polling in a busy loop. Only IRAM-safe interrupt handlers will be executed. These will be restored when the Flash operation completes.

.. only:: not SOC_HP_CPU_HAS_MULTIPLE_CORES

    When the caches are disabled, all non-IRAM-safe interrupts will be disabled, and all other tasks are suspended. Only IRAM-safe interrupt handlers will be executed. These will be restored when the Flash operation completes.

See :ref:`iram-safe-interrupt-handlers` for information on how to prevent an interrupt handler from being disabled when the cache is disabled.

When the cache is disabled, all CPUs should execute code and access data only from internal RAM. For differences between internal RAM (e.g., IRAM, DRAM) and flash cache, please refer to the :ref:`application memory layout <memory-layout>` documentation.


.. _iram-safe-interrupt-handlers:

IRAM-Safe Interrupt Handlers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For interrupt handlers which need to execute when the cache is disabled (e.g., for low latency operations), set the ``ESP_INTR_FLAG_IRAM`` flag when the :doc:`interrupt handler is registered </api-reference/system/intr_alloc>`.

You must ensure that all data and functions accessed by these interrupt handlers, including the ones that handlers call, are located in IRAM or DRAM. See :ref:`how-to-place-code-in-iram`.

If a function or symbol is not correctly put into IRAM/DRAM, and the interrupt handler reads from the flash cache during a flash operation, it will cause a crash. This may be due to an Illegal Instruction exception (for code which should be in IRAM) or garbage data being read (for constant data which should be in DRAM).

.. note::

    When working with strings in ISRs, it is not advised to use ``printf`` and other output functions. For debugging purposes, use :cpp:func:`ESP_DRAM_LOGE` and similar macros when logging from ISRs. Make sure that both ``TAG`` and format string are placed into ``DRAM`` in that case.

Non-IRAM-Safe Interrupt Handlers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If the ``ESP_INTR_FLAG_IRAM`` flag is not set when registering, the interrupt handler will not be executed when the caches are disabled. Once the caches are restored, the non-IRAM-safe interrupts will be re-enabled. After this moment, the interrupt handler will run normally again. This means that as long as caches are disabled, the corresponding hardware events will not occur.

.. only:: SOC_DMA_CAN_ACCESS_FLASH

    When DMA Read Data from Flash
    -----------------------------

    The Flash device doesn't allow reading while it is being erased/programmed, even when the data is not in the region being erased/programmed.

    When the flash is being erased/programmed, the Flash data read by DMA is unpredictable. It is recommended to stop DMA access to Flash before erasing or writing to it. If DMA cannot be stopped (for example, the LCD needs to continuously refresh image data stored in Flash), it is advisable to copy such data to PSRAM or internal SRAM.

.. only:: SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND

   .. include:: auto_suspend.inc

.. only:: SOC_SPIRAM_XIP_SUPPORTED

   .. include:: xip_from_psram.inc

