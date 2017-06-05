Support for external RAM
************************

.. toctree::
   :maxdepth: 1

Introduction
============

The ESP32 has a few hundred KiB of internal RAM. For some purposes, this is insufficient, and therefore the ESP32 incorporates the ability
to also use up to 4MiB of external SPI RAM memory as memory. The external memory is incorporated in the memory map and is, within certain 
restrictions, usable in the same way internal data RAM is.

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
   Espressif sells a ESP-WROVER module which contains an ESP32, 1.8V flash and the ESP-PSRAM32 integrated in a module, ready for inclusion
   on an end product PCB.

Software
========

Esp-idf fully supports integrating external memory use into your applications. Esp-idf can be configured to handle external RAM in several ways:
 * Only initialize RAM. This allows the application to manually place data here by dereferencing pointers pointed at the external RAM memory
   region (0x3F800000 and up).
 * Initialize RAM and add it to the capability allocator. This allows a program to specifically allocate a chunk of external RAM using
   ``pvPortMallocCaps(size, MALLOC_CAP_SPIRAM)``. This memory can be used and subsequently freed using a normal ``free()`` call.
 * Initialize RAM, add it to the capability allocator and add memory to the pool of RAM that can be returned by ``malloc()``. This allows
   any application to use the external RAM without having to rewrite the code to use ``pvPortMallocCaps``.

All these options can be selected from the menuconfig menu.

Restrictions
------------

The use of external RAM has a few restrictions:
 * When disabling flash cache (for example, because the flash is being written to), the external RAM also becomes inaccessible; any reads from or
   writes to it will lead to an illegal cache access exception. This is also the reason that esp-idf will never allocate a tasks stack in external
   RAM.
 * External RAM cannot be used as a place to store DMA transaction descriptors or as a buffer for a DMA transfer to read from or write into. Any
   buffers that will be used in combination with DMA must be allocated using ``pvPortMallocCaps(size, MALLOC_CAP_DMA)`` (and can be freed using a
   standard ``free()`` call.)
 * External RAM uses the same cache region as the external flash. This means that often accessed variables in external RAM can be read and 
   modified almost as quickly as in internal ram. However, when accessing large chunks of data (>32K), the cache can be insufficient and speeds 
   will fall back to the access speed of the external RAM. Moreover, accessing large chunks of data can 'push out' cached flash, possibly making 
   execution of code afterwards slower.


Chip revisions
==============

There are some issues with certain revisions of the ESP32 that have repercussions for use with external RAM. These are documented in the ESP32 
ECO_ document. In particular, esp-idf handles the bugs mentioned in the following ways:

ESP32 rev v0
------------
Esp-idf has no workaround for the bugs in this revision of silicon, and it cannot be used to map external PSRAM into the ESP32s main memory map.

ESP32 rev v1
------------
The bugs in this silicon revision introduce a hazard when certain sequences of machine instructions operate on external memory locations. To work around
this, the gcc compiler to compile esp-idf has been expanded with an flag: ``-mfix-esp32-psram-cache-issue``. With this flag passed to gcc on the command
line, the compiler work around these sequences and only output code that can safely be executed 

In esp-idf, this flag is enabled when you select 'Enable workaround for bug in SPI RAM cache for Rev1 ESP32s'. Esp-idf also takes other measures to make
sure no combination of PSRAM access plus the offending instruction sets are used: it links to a version of Newlib recompiled with the gcc flag, doesn't use
some ROM functions and allocates static memory for the WiFi stack.

.. _ECO: https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf






