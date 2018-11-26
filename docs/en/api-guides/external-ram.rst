Support for external RAM
************************

.. toctree::
   :maxdepth: 1

Introduction
============

The ESP32 has a few hundred KiB of internal RAM, residing on the same die as the rest of the ESP32. For some purposes, this is insufficient,
and therefore the ESP32 incorporates the ability to also use up to 4MiB of external SPI RAM memory as memory. The external memory is incorporated
in the memory map and is, within certain restrictions, usable in the same way internal data RAM is.

Hardware
========

The ESP32 supports SPI (P)SRAM connected in parallel with the SPI flash chip. While the ESP32 is capable of supporting several types
of RAM chips, the ESP32 SDK at the moment only supports the ESP-PSRAM32 chip.

The ESP-PSRAM32 chip is an 1.8V device, and can only be used in parallel with an 1.8V flash part. Make sure to either set the MTDI
pin to a high signal level on bootup, or program the fuses in the ESP32 to always use a VDD_SIO level of 1.8V. Not doing this risks
damaging the PSRAM and/or flash chip.

To connect the ESP-PSRAM chip to the ESP32D0W*, connect the following signals:
 * PSRAM /CE (pin 1) - ESP32 GPIO 16
 * PSRAM SO (pin 2) - flash DO
 * PSRAM SIO[2] (pin 3) - flash WP
 * PSRAM SI (pin 5) - flash DI
 * PSRAM SCLK (pin 6) - ESP32 GPIO 17
 * PSRAM SIO[3] (pin 7) - flash HOLD
 * PSRAM Vcc (pin 8) - ESP32 VCC_SDIO

Connections for the ESP32D2W* chips are TBD.

.. NOTE::
   Espressif sells an ESP-WROVER module which contains an ESP32, 1.8V flash and the ESP-PSRAM32 integrated in a module, ready for inclusion
   on an end product PCB.

.. _external_ram_config:

Configuring External RAM
========================

ESP-IDF fully supports using external memory in applications. ESP-IDF can be configured to handle external RAM in several ways after it is initialized at startup:

 * :ref:`external_ram_config_memory_map`
 * :ref:`external_ram_config_capability_allocator`
 * :ref:`external_ram_config_malloc` (default)
 * :ref:`external_ram_config_bss`

.. _external_ram_config_memory_map:

Integrate RAM into ESP32 memory map
-----------------------------------

Select this option by choosing "Integrate RAM into ESP32 memory map" from :ref:`CONFIG_SPIRAM_USE`.

This is the most basic option for external SPIRAM integration. Most users will want one of the other, more advanced, options.

During ESP-IDF startup, external RAM is mapped into the data address space starting at at address 0x3F800000 (byte-accessible). The length of this region is the same as the SPIRAM size (up to the limit of 4MiB).

The application can manually place data in external memory by creating pointers to this region. The application is responsible for all management of the external SPIRAM: coordinating buffer usage, preventing corruption, etc.

.. _external_ram_config_capability_allocator:

Add external RAM to the capability allocator
--------------------------------------------

Select this option by choosing "Make RAM allocatable using heap_caps_malloc(..., MALLOC_CAP_SPIRAM)" from :ref:`CONFIG_SPIRAM_USE`.

When enabled, memory is mapped to address 0x3F800000 but also added to the :doc:`capabilities-based heap memory allocator </api-reference/system/mem_alloc>` using ``MALLOC_CAP_SPIRAM``.

To allocate memory from external RAM, a program should call ``heap_caps_malloc(size, MALLOC_CAP_SPIRAM)``. After use, this memory can be freed by calling the normal ``free()`` function.

.. _external_ram_config_malloc:

Provide external RAM via malloc()
---------------------------------

Select this option by choosing "Make RAM allocatable using malloc() as well" from :ref:`CONFIG_SPIRAM_USE`. This is the default selection.

Using this option, memory is added to the capability allocator as described for the previous option. However it is also added to the pool of RAM that can be returned by standard ``malloc()``.

This allows any application to use the external RAM without having to rewrite the code to use ``heap_caps_malloc(..., MALLOC_CAP_SPIRAM)``.

An additional configuration item, :ref:`CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL`, can be used to set the size threshold when a single allocation should prefer external memory:

- When allocating a size less than the threshold, the allocator will try internal memory first.
- When allocating a size equal to or larger than the threshold, the allocator will try external memory first.

If a suitable block of preferred internal/external memory is not available, allocation will try the other type of memory.

Because some buffers can only be allocated in internal memory, a second configuration item :ref:`CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL` defines a pool of internal memory which is reserved for *only* explicitly internal allocations (such as memory for DMA use). Regular ``malloc()`` will not allocate from this pool. The :ref:`MALLOC_CAP_DMA <dma-capable-memory>` and ``MALLOC_CAP_INTERNAL`` flags can be used to allocate memory from this pool.

.. _external_ram_config_bss:

Allow .bss segment placed in external memory
--------------------------------------------

Enable this option by setting :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY`. This configuration setting is independent of the other three.

If enabled, a region of the address space starting from 0x3F800000 will be to used to store zero initialized data (BSS segment) from the lwip, net80211, libpp and bluedroid ESP-IDF libraries.

Additional data can be moved from the internal BSS segment to external RAM by applying the ``EXT_RAM_ATTR`` macro to any static declaration (which is not initialized to a non-zero value).

This option reduces the internal static memory used by the BSS segment.

Remaining external RAM can also be added to the capability heap allocator, by the method shown above.

Restrictions
============

External RAM use has the following restrictions:

 * When flash cache is disabled (for example, because the flash is being written to), the external RAM also becomes inaccessible; any reads from or
   writes to it will lead to an illegal cache access exception. This is also the reason that ESP-IDF does not by default allocate any task stacks in external RAM (see below).
 * External RAM cannot be used as a place to store DMA transaction descriptors or as a buffer for a DMA transfer to read from or write into. Any
   buffers that will be used in combination with DMA must be allocated using ``heap_caps_malloc(size, MALLOC_CAP_DMA)`` (and can be freed using a
   standard ``free()`` call.)
 * External RAM uses the same cache region as the external flash. This means that often accessed variables in external RAM can be read and
   modified almost as quickly as in internal ram. However, when accessing large chunks of data (>32K), the cache can be insufficient and speeds
   will fall back to the access speed of the external RAM. Moreover, accessing large chunks of data can 'push out' cached flash, possibly making
   execution of code afterwards slower.
 * External RAM cannot be used as task stack memory. Because of this, :cpp:func:`xTaskCreate` and similar functions will always allocate internal memory
   for stack and task TCBs and functions like :cpp:func:`xTaskCreateStatic` will check if the buffers passed are internal. However, for tasks not calling
   on code in ROM in any way, directly or indirectly, the menuconfig option :ref:`CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY` will eliminate
   the check in xTaskCreateStatic, allowing a task's stack to be in external RAM. Using this is not advised, however.
 * By default, failure to initialize external RAM will cause ESP-IDF startup to abort. This can be disabled by enabling config item :ref:`CONFIG_SPIRAM_IGNORE_NOTFOUND`. If :ref:`CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY` is enabled, the option to ignore failure is not available as the linker will have assigned symbols to external memory addresses at link time.
 * When used at 80MHz clock speed, external RAM must also occupy either the HSPI or VSPI bus. Select which SPI host will be used by :ref:`CONFIG_SPIRAM_OCCUPY_SPI_HOST`.

Chip revisions
==============

There are some issues with certain revisions of the ESP32 that have repercussions for use with external RAM. These are documented in the ESP32
ECO_ document. In particular, ESP-IDF handles the bugs mentioned in the following ways:

ESP32 rev v0
------------
ESP-IDF has no workaround for the bugs in this revision of silicon, and it cannot be used to map external PSRAM into the ESP32s main memory map.

ESP32 rev v1
------------
The bugs in this silicon revision introduce a hazard when certain sequences of machine instructions operate on external memory locations (ESP32 ECO 3.2).
To work around this, the gcc compiler to compile ESP-IDF has been expanded with a flag: ``-mfix-esp32-psram-cache-issue``. With this flag passed to gcc
on the command line, the compiler works around these sequences and only outputs code that can safely be executed.

In ESP-IDF, this flag is enabled when you select :ref:`CONFIG_SPIRAM_CACHE_WORKAROUND`. ESP-IDF also takes other measures to make
sure no combination of PSRAM access plus the offending instruction sets are used: it links to a version of Newlib recompiled with the gcc flag, doesn't use
some ROM functions and allocates static memory for the WiFi stack.

.. _ECO: https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf






