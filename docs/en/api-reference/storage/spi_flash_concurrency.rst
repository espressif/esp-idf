.. _concurrency-constraints-flash:

Concurrency Constraints for flash on SPI1
=========================================

The SPI0/1 bus is shared between the instruction & data cache (for firmware execution) and the SPI1 peripheral (controlled by the drivers including this SPI Flash driver). Hence, operations to SPI1 will cause significant influence to the whole system. This kind of operations include calling SPI Flash API or other drivers on SPI1 bus, any operations like read/write/erase or other user defined SPI operations, regardless to the main flash or other SPI slave devices.

.. only:: not esp32c3

   On {IDF_TARGET_NAME}, these caches must be disabled while reading/writing/erasing.

.. only:: esp32c3

    On {IDF_TARGET_NAME}, the config option :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND` (enabled by default) allows the cache to read flash concurrently with SPI1 operations. See :ref:`auto-suspend` for more details.

    If this option is disabled, the caches must be disabled while reading/writing/erasing operations. There are some constraints using driver on the SPI1 bus, see :ref:`impact_disabled_cache`. This constraints will cause more IRAM/DRAM usages.


.. _impact_disabled_cache:

When the caches are disabled
----------------------------

Under this condition, all CPUs should always execute code and access data from internal RAM. The APIs documented in this file will disable the caches automatically and transparently.

.. only:: esp32c3

    However, when :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND` is enabled, these APIs won't disable the caches. The hardware will handle the arbitration between them.

.. only:: not CONFIG_FREERTOS_UNICORE

    The way that these APIs disable the caches will suspend all the other tasks. Besides, all non-IRAM-safe interrupts will be disabled. The other core will be polling in a busy loop. These will be restored until the Flash operation completes.

.. only:: CONFIG_FREERTOS_UNICORE

    The way that these APIs disable the caches will also disable non-IRAM-safe interrupts. These will be restored until the Flash operation completes.

See also :ref:`esp_flash_os_func` and :ref:`spi_bus_lock`.

There are no such constraints and impacts for flash chips on other SPI buses than SPI0/1.

For differences between internal RAM (e.g. IRAM, DRAM) and flash cache, please refer to the :ref:`application memory layout <memory-layout>` documentation.


.. _iram-safe-interrupt-handlers:

IRAM-Safe Interrupt Handlers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For interrupt handlers which need to execute when the cache is disabled (e.g., for low latency operations), set the ``ESP_INTR_FLAG_IRAM`` flag when the :doc:`interrupt handler is registered </api-reference/system/intr_alloc>`.

You must ensure that all data and functions accessed by these interrupt handlers, including the ones that handlers call, are located in IRAM or DRAM. See :ref:`how-to-place-code-in-iram`.

If a function or symbol is not correctly put into IRAM/DRAM, and the interrupt handler reads from the flash cache during a flash operation, it will cause a crash due to Illegal Instruction exception (for code which should be in IRAM) or garbage data to be read (for constant data which should be in DRAM).

.. note::

    When working with strings in ISRs, it is not advised to use ``printf`` and other output functions. For debugging purposes, use :cpp:func:`ESP_DRAM_LOGE` and similar macros when logging from ISRs. Make sure that both ``TAG`` and format string are placed into ``DRAM`` in that case.

Non-IRAM-Safe Interrupt Handlers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If the ``ESP_INTR_FLAG_IRAM`` flag is not set when registering, the interrupt handler will not get executed when the caches are disabled. Once the caches are restored, the non-IRAM-safe interrupts will be re-enabled. After this moment, the interrupt handler will run normally again. This means that as long as caches are disabled, users won't see the corresponding hardware event happening.


.. only:: esp32c3

   .. include:: auto_suspend.inc
