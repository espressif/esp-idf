Concurrency Constraints for flash on SPI1
=========================================

The SPI0/1 bus is shared between the instruction & data cache (for firmware execution) and the SPI1 peripheral (controlled by the drivers including this SPI Flash driver). Hence, operations to SPI1 will cause significant influence to the whole system. This kind of operations include calling SPI Flash API or other drivers on SPI1 bus, any operations like read/write/erase or other user defined SPI operations, regardless to the main flash or other SPI slave devices.

.. only:: not esp32c3

   On {IDF_TARGET_NAME}, these caches must be disabled while reading/writing/erasing.

.. only:: esp32c3

    On {IDF_TARGET_NAME}, the config option :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND` (enabled by default) allows the cache to read flash & PSRAM concurrently with SPI1 operations. See :ref:`auto_suspend` for more details.

    If this option is disabled, the caches must be disabled while reading/writing/erasing operations. There are some constraints using driver on the SPI1 bus, see :ref:`impact_disabled_cache`. This constraints will cause more IRAM/DRAM usages.

.. _impact_disabled_cache:

When the caches are disabled
----------------------------

This means that all CPUs must be running code from IRAM and must only be reading data from DRAM while flash write operations occur. If you use the API functions documented here, then the caches will be disabled automatically and transparently. However, note that it will have some performance impact on other tasks in the system.

There are no such constraints and impacts for flash chips on other SPI buses than SPI0/1.

For differences between IRAM, DRAM, and flash cache, please refer to the :ref:`application memory layout <memory-layout>` documentation.

.. only:: not CONFIG_FREERTOS_UNICORE

    To avoid reading flash cache accidentally, when one CPU initiates a flash write or erase operation, the other CPU is put into a blocked state, and all non-IRAM-safe interrupts are disabled on all CPUs until the flash operation completes.

See also :ref:`esp_flash_os_func`, :ref:`spi_bus_lock`.

.. _iram-safe-interrupt-handlers:

IRAM-Safe Interrupt Handlers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you have an interrupt handler that you want to execute while a flash operation is in progress (for example, for low latency operations), set the ``ESP_INTR_FLAG_IRAM`` flag when the :doc:`interrupt handler is registered </api-reference/system/intr_alloc>`.

You must ensure that all data and functions accessed by these interrupt handlers, including the ones that handlers call, are located in IRAM or DRAM. See :ref:`how-to-place-code-in-iram`.

If a function or symbol is not correctly put into IRAM/DRAM, and the interrupt handler reads from the flash cache during a flash operation, it will cause a crash due to Illegal Instruction exception (for code which should be in IRAM) or garbage data to be read (for constant data which should be in DRAM).

.. note::

   When working with string in ISRs, it is not advised to use ``printf`` and other output functions. For debugging purposes, use :cpp:func:`ESP_DRAM_LOGE` and similar macros when logging from ISRs. Make sure that both ``TAG`` and format string are placed into ``DRAM`` in that case.

.. only:: esp32c3

   .. include:: auto_suspend.inc
