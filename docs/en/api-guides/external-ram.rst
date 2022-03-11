Support for external RAM
************************

:link_to_translation:`zh_CN:[中文]`

.. toctree::
   :maxdepth: 1

Introduction
============
{IDF_TARGET_PSRAM_SIZE:default="Value not updated", esp32="4 MB", esp32s2="10.5 MB", esp32s3="16 MB"}

{IDF_TARGET_NAME} has a few hundred kilobytes of internal RAM, residing on the same die as the rest of the chip components. It can be insufficient for some purposes, so {IDF_TARGET_NAME} has the ability to also use up to {IDF_TARGET_PSRAM_SIZE} of external SPI RAM memory. The external memory is incorporated in the memory map and, with certain restrictions, is usable in the same way as internal data RAM.


Hardware
========

{IDF_TARGET_NAME} supports SPI PSRAM connected in parallel with the SPI flash chip. While {IDF_TARGET_NAME} is capable of supporting several types of RAM chips, ESP-IDF currently only supports Espressif branded PSRAM chips (ESP-PSRAM32, ESP-PSRAM64, etc).

.. note:: Some PSRAM chips are 1.8 V devices and some are 3.3 V. The working voltage of the PSRAM chip must match the working voltage of the flash component. Consult the datasheet for your PSRAM chip and {IDF_TARGET_NAME} device to find out the working voltages. For a 1.8 V PSRAM chip, make sure to either set the MTDI pin to a high signal level on bootup, or program {IDF_TARGET_NAME} eFuses to always use the VDD_SIO level of 1.8 V. Not doing this can damage the PSRAM and/or flash chip.

.. note:: Espressif produces both modules and system-in-package chips that integrate compatible PSRAM and flash and are ready to mount on a product PCB. Consult the Espressif website for more information.

For specific details about connecting the SoC or module pins to an external PSRAM chip, consult the SoC or module datasheet.

.. _external_ram_config:


Configuring External RAM
========================

ESP-IDF fully supports the use of external memory in applications. Once the external RAM is initialized at startup, ESP-IDF can be configured to handle it in several ways:

.. list::

    * :ref:`external_ram_config_memory_map`
    * :ref:`external_ram_config_capability_allocator`
    * :ref:`external_ram_config_malloc` (default)
    :esp32 or esp32s2: * :ref:`external_ram_config_bss`
    :esp32: * :ref:`external_ram_config_noinit`

.. _external_ram_config_memory_map:

Integrate RAM into the {IDF_TARGET_NAME} memory map
---------------------------------------------------
{IDF_TARGET_PSRAM_ADDR_START:default="Value not updated", esp32="0x3F800000", esp32s2="0x3F500000", esp32s3="0x3D000000"}

Select this option by choosing "Integrate RAM into memory map" from :ref:`CONFIG_SPIRAM_USE`.

This is the most basic option for external SPI RAM integration. Most likely, you will need another, more advanced option.

During the ESP-IDF startup, external RAM is mapped into the data address space, starting at address {IDF_TARGET_PSRAM_ADDR_START} (byte-accessible). The length of this region is the same as the SPI RAM size (up to the limit of {IDF_TARGET_PSRAM_SIZE}).

Applications can manually place data in external memory by creating pointers to this region. So if an application uses external memory, it is responsible for all management of the external SPI RAM: coordinating buffer usage, preventing corruption, etc.

.. _external_ram_config_capability_allocator:


Add external RAM to the capability allocator
--------------------------------------------

Select this option by choosing "Make RAM allocatable using heap_caps_malloc(..., MALLOC_CAP_SPIRAM)" from :ref:`CONFIG_SPIRAM_USE`.

When enabled, memory is mapped to address {IDF_TARGET_PSRAM_ADDR_START} and also added to the :doc:`capabilities-based heap memory allocator </api-reference/system/mem_alloc>` using ``MALLOC_CAP_SPIRAM``.

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

.. only:: esp32 or esp32s2

    .. _external_ram_config_bss:

    Allow .bss segment placed in external memory
    --------------------------------------------

    Enable this option by checking :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY`. This configuration setting is independent of the other three.

    If enabled, a region of the address space starting from {IDF_TARGET_PSRAM_ADDR_START} will be used to store zero-initialized data (BSS segment) from the lwIP, net80211, libpp, and bluedroid ESP-IDF libraries.

    Additional data can be moved from the internal BSS segment to external RAM by applying the macro ``EXT_RAM_ATTR`` to any static declaration (which is not initialized to a non-zero value).

    It is also possible to place the BSS section of a component or a library to external RAM using linker fragment scheme ``extram_bss``.

    This option reduces the internal static memory used by the BSS segment.

    Remaining external RAM can also be added to the capability heap allocator using the method shown above.


.. only:: esp32

    .. _external_ram_config_noinit:

    Allow .noinit segment placed in external memory
    -----------------------------------------------

    Enable this option by checking :ref:`CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY`. If enabled, a region of the address space provided in external RAM will be used to store non-initialized data. The values placed in this segment will not be initialized or modified even during startup or restart.

    By applying the macro ``EXT_RAM_NOINIT_ATTR``, data could be moved from the internal NOINIT segment to external RAM. Remaining external RAM can still be added to the capability heap allocator using the method shown above, :ref:`external_ram_config_capability_allocator`.

Restrictions
============

External RAM use has the following restrictions:

 * When flash cache is disabled (for example, if the flash is being written to), the external RAM also becomes inaccessible; any reads from or writes to it will lead to an illegal cache access exception. This is also the reason why ESP-IDF does not by default allocate any task stacks in external RAM (see below).

  * External RAM cannot be used as a place to store DMA transaction descriptors or as a buffer for a DMA transfer to read from or write into. Therefore when External RAM is enabled, any buffers that will be used in combination with DMA must be allocated using ``heap_caps_malloc(size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL)`` and can be freed using a standard ``free()`` call. 
  
  .. only:: SOC_PSRAM_DMA_CAPABLE

    Note, although {IDF_TARGET_NAME} has hardware support for DMA to/from external RAM, this is not yet supported in ESP-IDF.

 * External RAM uses the same cache region as the external flash. This means that frequently accessed variables in external RAM can be read and modified almost as quickly as in internal ram. However, when accessing large chunks of data (>32 KB), the cache can be insufficient, and speeds will fall back to the access speed of the external RAM. Moreover, accessing large chunks of data can "push out" cached flash, possibly making the execution of code slower afterwards.

 * In general, external RAM cannot be used as task stack memory. Due to this, :cpp:func:`xTaskCreate` and similar functions will always allocate internal memory for stack and task TCBs, and functions such as :cpp:func:`xTaskCreateStatic` will check if the buffers passed are internal.

.. only:: esp32

    The option :ref:`CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY` can be used to place task stacks into external memory. In these cases :cpp:func:`xTaskCreateStatic` must be used to specify a task stack buffer allocated from external memory, otherwise task stacks will still be allocated from internal memory.

Failure to initialize
=====================

 By default, failure to initialize external RAM will cause the ESP-IDF startup to abort. This can be disabled by enabling the config item :ref:`CONFIG_SPIRAM_IGNORE_NOTFOUND`.

 .. only:: esp32 or esp32s2

    If :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY` is enabled, the option to ignore failure is not available as the linker will have assigned symbols to external memory addresses at link time.


.. only:: not esp32

    Encryption
    ==========

    It is possible to enable automatic encryption for data stored in external RAM. When this is enabled any data read and written through the cache will automatically be encrypted/decrypted by the external memory encryption hardware.

    This feature is enabled whenever flash encryption is enabled. For more information on how to enable and how it works see :doc:`Flash Encryption </security/flash-encryption>`.


.. only:: esp32

    .. include:: inc/external-ram-esp32-notes.rst

.. _ESP32 ECO: https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf
.. _ESP32 ECO V3 User Guide: https://www.espressif.com/sites/default/files/documentation/ESP32_ECO_V3_User_Guide__EN.pdf