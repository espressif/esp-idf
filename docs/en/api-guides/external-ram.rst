Support for External RAM
************************

:link_to_translation:`zh_CN:[中文]`

.. toctree::
    :maxdepth: 1

Introduction
============

{IDF_TARGET_PSRAM_VADDR_SIZE:default="Value not updated", esp32="4 MB", esp32s2="10.5 MB", esp32s3="32 MB", esp32p4="64 MB"}

{IDF_TARGET_NAME} has a few hundred kilobytes of internal RAM, residing on the same die as the rest of the chip components. It can be insufficient for some purposes, so {IDF_TARGET_NAME} has the ability to use up to {IDF_TARGET_PSRAM_VADDR_SIZE} of virtual addresses for external PSRAM (Psuedostatic RAM) memory. The external memory is incorporated in the memory map and, with certain restrictions, is usable in the same way as internal data RAM.

.. only:: esp32s3

    The {IDF_TARGET_PSRAM_VADDR_SIZE} virtual addresses are shared with flash instructions and rodata.

Hardware
========

{IDF_TARGET_NAME} supports PSRAM connected in parallel with the SPI flash chip. While {IDF_TARGET_NAME} is capable of supporting several types of RAM chips, ESP-IDF currently only supports Espressif branded PSRAM chips (e.g., ESP-PSRAM32, ESP-PSRAM64, etc).

.. note::

    .. only:: esp32 or esp32s2 or esp32s3

        Some PSRAM chips are 1.8 V devices and some are 3.3 V. The working voltage of the PSRAM chip must match the working voltage of the flash component. Consult the datasheet for your PSRAM chip and {IDF_TARGET_NAME} device to find out the working voltages. For a 1.8 V PSRAM chip, make sure to either set the MTDI pin to a high signal level on boot-up, or program {IDF_TARGET_NAME} eFuses to always use the VDD_SIO level of 1.8 V. Not doing this can damage the PSRAM and/or flash chip.

    .. only:: esp32p4

        Some PSRAM chips are 1.8 V devices and some are 3.3 V. Consult the datasheet for your PSRAM chip and {IDF_TARGET_NAME} device to find out the working voltages.

        By default, the PSRAM is powered up by the on-chip LDO2. You can use :ref:`CONFIG_ESP_LDO_CHAN_PSRAM_DOMAIN` to switch the LDO channel accordingly. Set this value to -1 to use an external power supply, which means the on-chip LDO will not be used. By default, the PSRAM connected to LDO is set to the correct voltage based on the Espressif module used. You can still use :ref:`CONFIG_ESP_LDO_VOLTAGE_PSRAM_DOMAIN` to select the LDO output voltage if you are not using an Espressif module. When using an external power supply, this option does not exist.

.. note::

    Espressif produces both modules and system-in-package chips that integrate compatible PSRAM and flash and are ready to mount on a product PCB. Consult the Espressif website for more information. If you are using a custom PSRAM chip, ESP-IDF SDK might not be compatible with it.

For specific details about connecting the SoC or module pins to an external PSRAM chip, consult the SoC or module datasheet.

.. _external_ram_config:


Configuring External RAM
========================

ESP-IDF fully supports the use of external RAM in applications. Once the external RAM is initialized at startup, ESP-IDF can be configured to integrate the external RAM in several ways:

.. list::

    * :ref:`external_ram_config_memory_map`
    * :ref:`external_ram_config_capability_allocator`
    * :ref:`external_ram_config_malloc` (default)
    * :ref:`external_ram_config_bss`
    :esp32: * :ref:`external_ram_config_noinit`
    :SOC_SPIRAM_XIP_SUPPORTED: * :ref:`external_ram_config_xip`

.. _external_ram_config_memory_map:


Integrate RAM into the {IDF_TARGET_NAME} Memory Map
---------------------------------------------------

Select this option by choosing ``Integrate RAM into memory map`` from :ref:`CONFIG_SPIRAM_USE`.

This is the most basic option for external RAM integration. Most likely, you will need another, more advanced option.

During the ESP-IDF startup, external RAM is mapped into the data virtual address space. The address space is dynamically allocated. The length will be the minimum length between the PSRAM size and the available data virtual address space size.

Applications can manually place data in external memory by creating pointers to this region. So if an application uses external memory, it is responsible for all management of the external RAM: coordinating buffer usage, preventing corruption, etc.

It is recommended to access the PSRAM by ESP-IDF heap memory allocator (see next chapter).

.. _external_ram_config_capability_allocator:


Add External RAM to the Capability Allocator
--------------------------------------------

Select this option by choosing ``Make RAM allocatable using heap_caps_malloc(..., MALLOC_CAP_SPIRAM)`` from :ref:`CONFIG_SPIRAM_USE`.

When enabled, memory is mapped to data virtual address space and also added to the :doc:`capabilities-based heap memory allocator </api-reference/system/mem_alloc>` using ``MALLOC_CAP_SPIRAM``.

To allocate memory from external RAM, a program should call ``heap_caps_malloc(size, MALLOC_CAP_SPIRAM)``. After use, this memory can be freed by calling the normal ``free()`` function.

.. _external_ram_config_malloc:


Provide External RAM via malloc()
---------------------------------

Select this option by choosing ``Make RAM allocatable using malloc() as well`` from :ref:`CONFIG_SPIRAM_USE`. This is the default option.

In this case, memory is added to the capability allocator as described for the previous option. However, it is also added to the pool of RAM that can be returned by the standard ``malloc()`` function.

This allows any application to use the external RAM without having to rewrite the code to use ``heap_caps_malloc(..., MALLOC_CAP_SPIRAM)``.

An additional configuration item, :ref:`CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL`, can be used to set the size threshold when a single allocation should prefer external memory:

- When allocating a size less than or equal to the threshold, the allocator will try internal memory first.
- When allocating a size larger than the threshold, the allocator will try external memory first.

If a suitable block of preferred internal/external memory is not available, the allocator will try the other type of memory.

Because some buffers can only be allocated in internal memory, a second configuration item :ref:`CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL` defines a pool of internal memory which is reserved for *only* explicitly internal allocations (such as memory for DMA use). Regular ``malloc()`` will not allocate from this pool. The :ref:`MALLOC_CAP_DMA <dma-capable-memory>` and ``MALLOC_CAP_INTERNAL`` flags can be used to allocate memory from this pool.

.. _external_ram_config_bss:

Allow .bss Segment to Be Placed in External Memory
--------------------------------------------------

Enable this option by checking :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY`.

If enabled, the region of the data virtual address space where the PSRAM is mapped to will be used to store zero-initialized data (BSS segment) from the lwIP, net80211, libpp, wpa_supplicant and bluedroid ESP-IDF libraries.

Additional data can be moved from the internal BSS segment to external RAM by applying the macro ``EXT_RAM_BSS_ATTR`` to any static declaration (which is not initialized to a non-zero value).

It is also possible to place the BSS section of a component or a library to external RAM using linker fragment scheme ``extram_bss``.

This option reduces the internal static memory used by the BSS segment.

Remaining external RAM can also be added to the capability heap allocator using the method shown above.

.. _external_ram_config_noinit:

Allow .noinit Segment to Be Placed in External Memory
--------------------------------------------------------------

Enable this option by checking :ref:`CONFIG_SPIRAM_ALLOW_NOINIT_SEG_EXTERNAL_MEMORY`. If enabled, the region of the data virtual address space where the PSRAM is mapped to will be used to store non-initialized data. The values placed in this segment will not be initialized or modified even during startup or restart.

By applying the macro ``EXT_RAM_NOINIT_ATTR``, data could be moved from the internal NOINIT segment to external RAM. Remaining external RAM can still be added to the capability heap allocator using the method shown above, :ref:`external_ram_config_capability_allocator`.

.. only:: SOC_SPIRAM_XIP_SUPPORTED

    .. only:: esp32s2 or esp32s3

        Move Instructions in Flash to PSRAM
        -----------------------------------

        The :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS` option allows the flash ``.text`` sections (for instructions) to be placed in PSRAM.

        By enabling the :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS` option,

        - Instructions from the ``.text`` sections of flash are moved into PSRAM on system startup.

        - The corresponding virtual memory range of those instructions will also be re-mapped to PSRAM.

        Move Read-Only Data in Flash to PSRAM
        ---------------------------------------

        The :ref:`CONFIG_SPIRAM_RODATA` option allows the flash ``.rodata`` sections (for read only data) to be placed in PSRAM.

        By enabling the :ref:`CONFIG_SPIRAM_RODATA` option,

        - Instructions from the ``.rodata`` sections of flash are moved into PSRAM on system startup.

        - The corresponding virtual memory range of those rodata will also be re-mapped to PSRAM.

        .. _external_ram_config_xip:

        Execute In Place (XiP) from PSRAM
        ------------------------------------

        The :ref:`CONFIG_SPIRAM_XIP_FROM_PSRAM` is a helper option for you to select both the :ref:`CONFIG_SPIRAM_FETCH_INSTRUCTIONS` and :ref:`CONFIG_SPIRAM_RODATA`.

        The benefits of XiP from PSRAM is:

        - PSRAM access speed is faster than Flash access. So the performance is better.

        - The cache will not be disabled during an SPI1 flash operation, thus optimizing the code execution performance during SPI1 flash operations. For ISRs, ISR callbacks and data which might be accessed during this period, you do not need to place them in internal RAM, thus internal RAM usage can be optimized. This feature is useful for high throughput peripheral involved applications to improve the performance during SPI1 flash operations.

        :example:`system/xip_from_psram` demonstrates the usage of XiP from PSRAM, optimizing internal RAM usage and avoiding cache disabling during flash operations from user call (e.g., flash erase/read/write operations).

    .. only:: esp32p4

        .. _external_ram_config_xip:

        Execute In Place (XiP) from PSRAM
        ------------------------------------

        The :ref:`CONFIG_SPIRAM_XIP_FROM_PSRAM` option enables the executable in place (XiP) from PSRAM feature. With this option sections that are normally placed in flash, ``.text`` (for instructions) and ``.rodata`` (for read only data), will be loaded in PSRAM.

        With this option enabled, the cache will not be disabled during an SPI1 flash operation, so code that requires executing during an SPI1 flash operation does not have to be placed in internal RAM. Because P4 flash and PSRAM are using two separate SPI buses, moving flash content to PSRAM will actually increase the load of the PSRAM MSPI bus, so the exact impact on performance will be dependent on your app usage of PSRAM. For example, as the PSRAM bus speed could be much faster than flash bus speed, if the instructions and data that are used to be in flash are not accessed very frequently, you might get better performance with this option enabled. We suggest doing performance profiling to determine if enabling this option.

Restrictions
============

External RAM use has the following restrictions:

.. list::

    - When flash cache is disabled (for example, if the flash is being written to), the external RAM also becomes inaccessible. Any read operations from or write operations to it will lead to an illegal cache access exception. This is also the reason why ESP-IDF does not by default allocate any task stacks in external RAM (see below).

    :esp32s2: - External RAM cannot be used as a place to store DMA transaction descriptors or as a buffer for a DMA transfer to read from or write into. Therefore when External RAM is enabled, any buffer that will be used in combination with DMA must be allocated using ``heap_caps_malloc(size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL)`` and can be freed using a standard ``free()`` call. Note that although {IDF_TARGET_NAME} has hardware support for DMA to or from external RAM, this is not yet supported in ESP-IDF.

    :esp32s3: - Although {IDF_TARGET_NAME} has hardware support for DMA to or from external RAM, there are still limitations:

        :esp32s3: - DMA transaction descriptors cannot be placed in PSRAM.
        :esp32s3: - The bandwidth that DMA accesses external RAM is very limited, especially when the core is trying to access the external RAM at the same time.
        :esp32s3: - You can configure :ref:`CONFIG_SPIRAM_SPEED` as 120 MHz for an octal PSRAM. The bandwidth will be improved. However there are still restrictions for this option. See :ref:`All Supported PSRAM Modes and Speeds <flash-psram-combination>` for more details.

    - External RAM uses the same cache region as the external flash. This means that frequently accessed variables in external RAM can be read and modified almost as quickly as in internal RAM. However, when accessing large chunks of data (> 32 KB), the cache can be insufficient, and speeds will fall back to the access speed of the external RAM. Moreover, accessing large chunks of data can "push out" cached flash, possibly making the execution of code slower afterwards.

    - In general, external RAM will not be used as task stack memory. :cpp:func:`xTaskCreate` and similar functions will always allocate internal memory for stack and task TCBs.

The option :ref:`CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY` can be used to allow placing task stacks into external memory. In these cases :cpp:func:`xTaskCreateStatic` must be used to specify a task stack buffer allocated from external memory, otherwise task stacks will still be allocated from internal memory.


Failure to Initialize
=====================

By default, failure to initialize external RAM will cause the ESP-IDF startup to abort. This can be disabled by enabling the config item :ref:`CONFIG_SPIRAM_IGNORE_NOTFOUND`.

.. only:: esp32 or esp32s2

    If :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY` is enabled, the option to ignore failure is not available as the linker will have assigned symbols to external memory addresses at link time.


.. only:: not esp32

    Encryption
    ==========

    It is possible to enable automatic encryption for data stored in external RAM. When this is enabled any data read and written through the cache will automatically be encrypted or decrypted by the external memory encryption hardware.

    This feature is enabled whenever flash encryption is enabled. For more information on how to enable and how it works see :doc:`Flash Encryption </security/flash-encryption>`.


.. only:: esp32

    .. include:: inc/external-ram-esp32-notes.rst

.. _ESP32 Series SoC Errata: https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf
.. _ESP32 Chip Revision v3.0 User Guide: https://www.espressif.com/sites/default/files/documentation/ESP32_ECO_V3_User_Guide__EN.pdf
