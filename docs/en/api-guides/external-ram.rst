Support for external RAM
************************

:link_to_translation:`zh_CN:[中文]`

.. toctree::
   :maxdepth: 1

Introduction
============

{IDF_TARGET_NAME} has a few hundred kilobytes of internal RAM, residing on the same die as the rest of the chip components. It can be insufficient for some purposes, so {IDF_TARGET_NAME} has the ability to also use up to 4 MB of external SPI RAM memory. The external memory is incorporated in the memory map and, with certain restrictions, is usable in the same way as internal data RAM.


Hardware
========

{IDF_TARGET_NAME} supports SPI PSRAM connected in parallel with the SPI flash chip. While {IDF_TARGET_NAME} is capable of supporting several types of RAM chips, ESP-IDF only supports the ESP-PSRAM32 chip at the moment.

The ESP-PSRAM32 chip is a 1.8 V device which can only be used in parallel with a 1.8 V flash component. Make sure to either set the MTDI pin to a high signal level on bootup, or program {IDF_TARGET_NAME} eFuses to always use the VDD_SIO level of 1.8 V. Not doing this can damage the PSRAM and/or flash chip.

.. only:: esp32

    To connect the ESP-PSRAM32 chip to ESP32D0W*, connect the following signals:

    * PSRAM /CE (pin 1) > ESP32 GPIO 16
    * PSRAM SO (pin 2) > flash DO
    * PSRAM SIO[2] (pin 3) > flash WP
    * PSRAM SI (pin 5) > flash DI
    * PSRAM SCLK (pin 6) > ESP32 GPIO 17
    * PSRAM SIO[3] (pin 7) > flash HOLD
    * PSRAM Vcc (pin 8) > ESP32 VCC_SDIO

    Connections for ESP32D2W* chips are TBD.

    .. note::

        Espressif produces the line of ESP32-WROVER modules which contain ESP32, 1.8 V flash, and ESP-PSRAM32. These modules are ready to be mounted on an end product PCB.

.. _external_ram_config:


Configuring External RAM
========================

ESP-IDF fully supports the use of external memory in applications. Once the external RAM is initialized at startup, ESP-IDF can be configured to handle it in several ways:

.. list::

    * :ref:`external_ram_config_memory_map`
    * :ref:`external_ram_config_capability_allocator`
    * :ref:`external_ram_config_malloc` (default)
    :esp32: * :ref:`external_ram_config_bss`

.. _external_ram_config_memory_map:


Integrate RAM into the {IDF_TARGET_NAME} memory map
---------------------------------------------------

Select this option by choosing "Integrate RAM into memory map" from :ref:`CONFIG_SPIRAM_USE`.

This is the most basic option for external SPI RAM integration. Most likely, you will need another, more advanced option.

During the ESP-IDF startup, external RAM is mapped into the data address space, starting at address 0x3F800000 (byte-accessible). The length of this region is the same as the SPI RAM size (up to the limit of 4 MB).

Applications can manually place data in external memory by creating pointers to this region. So if an application uses external memory, it is responsible for all management of the external SPI RAM: coordinating buffer usage, preventing corruption, etc.

.. _external_ram_config_capability_allocator:


Add external RAM to the capability allocator
--------------------------------------------

Select this option by choosing "Make RAM allocatable using heap_caps_malloc(..., MALLOC_CAP_SPIRAM)" from :ref:`CONFIG_SPIRAM_USE`.

When enabled, memory is mapped to address 0x3F800000 and also added to the :doc:`capabilities-based heap memory allocator </api-reference/system/mem_alloc>` using ``MALLOC_CAP_SPIRAM``.

To allocate memory from external RAM, a program should call ``heap_caps_malloc(size, MALLOC_CAP_SPIRAM)``. After use, this memory can be freed by calling the normal ``free()`` function.

.. _external_ram_config_malloc:


Provide external RAM via malloc()
---------------------------------

Select this option by choosing "Make RAM allocatable using malloc() as well" from :ref:`CONFIG_SPIRAM_USE`. This is the default option.

In this case, memory is added to the capability allocator as described for the previous option. However, it is also added to the pool of RAM that can be returned by the standard ``malloc()`` function.

This allows any application to use the external RAM without having to rewrite the code to use ``heap_caps_malloc(..., MALLOC_CAP_SPIRAM)``.

An additional configuration item, :ref:`CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL`, can be used to set the size threshold when a single allocation should prefer external memory:

- When allocating a size less than the threshold, the allocator will try internal memory first.
- When allocating a size equal to or larger than the threshold, the allocator will try external memory first.

If a suitable block of preferred internal/external memory is not available, the allocator will try the other type of memory.

Because some buffers can only be allocated in internal memory, a second configuration item :ref:`CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL` defines a pool of internal memory which is reserved for *only* explicitly internal allocations (such as memory for DMA use). Regular ``malloc()`` will not allocate from this pool. The :ref:`MALLOC_CAP_DMA <dma-capable-memory>` and ``MALLOC_CAP_INTERNAL`` flags can be used to allocate memory from this pool.

.. only:: esp32

    .. _external_ram_config_bss:

    Allow .bss segment placed in external memory
    --------------------------------------------

    Enable this option by checking :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY`. This configuration setting is independent of the other three.

    If enabled, a region of the address space starting from 0x3F800000 will be used to store zero-initialized data (BSS segment) from the lwIP, net80211, libpp, and bluedroid ESP-IDF libraries.

    Additional data can be moved from the internal BSS segment to external RAM by applying the macro ``EXT_RAM_ATTR`` to any static declaration (which is not initialized to a non-zero value).

    This option reduces the internal static memory used by the BSS segment.

    Remaining external RAM can also be added to the capability heap allocator using the method shown above.


Restrictions
============

External RAM use has the following restrictions:

 * When flash cache is disabled (for example, if the flash is being written to), the external RAM also becomes inaccessible; any reads from or writes to it will lead to an illegal cache access exception. This is also the reason why ESP-IDF does not by default allocate any task stacks in external RAM (see below).
 * External RAM cannot be used as a place to store DMA transaction descriptors or as a buffer for a DMA transfer to read from or write into. Any buffers that will be used in combination with DMA must be allocated using ``heap_caps_malloc(size, MALLOC_CAP_DMA)`` and can be freed using a standard ``free()`` call.
 * External RAM uses the same cache region as the external flash. This means that frequently accessed variables in external RAM can be read and modified almost as quickly as in internal ram. However, when accessing large chunks of data (>32 KB), the cache can be insufficient, and speeds will fall back to the access speed of the external RAM. Moreover, accessing large chunks of data can "push out" cached flash, possibly making the execution of code slower afterwards.
 * External RAM cannot be used as task stack memory. Due to this, :cpp:func:`xTaskCreate` and similar functions will always allocate internal memory for stack and task TCBs, and functions such as :cpp:func:`xTaskCreateStatic` will check if the buffers passed are internal.
 * By default, failure to initialize external RAM will cause the ESP-IDF startup to abort. This can be disabled by enabling the config item :ref:`CONFIG_SPIRAM_IGNORE_NOTFOUND`.

.. only:: esp32

    If :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY` is enabled, the option to ignore failure is not available as the linker will have assigned symbols to external memory addresses at link time.


.. only:: esp32

    .. include:: inc/external-ram-esp32-notes.rst

.. _ESP32 ECO: https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf
.. _ESP32 ECO V3 User Guide: https://www.espressif.com/sites/default/files/documentation/ESP32_ECO_V3_User_Guide__EN.pdf
