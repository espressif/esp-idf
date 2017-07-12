Memory allocation
=================

Overview
--------

The ESP32 has multiple types of RAM. Internally, there's IRAM, DRAM as well as RAM that can be used as both. It's also
possible to connect external SPI flash to the ESP32; it's memory can be integrated into the ESP32s memory map using
the flash cache.

In order to make use of all this memory, esp-idf has a capabilities-based memory allocator. Basically, if you want to have
memory with certain properties (for example, DMA-capable, or capable of executing code), you
can create an OR-mask of the required capabilities and pass that to pvPortMallocCaps. For instance, the normal malloc
code internally allocates memory with ```heap_caps_malloc(size, MALLOC_CAP_8BIT)``` in order to get data memory that is
byte-addressable.

Because malloc uses this allocation system as well, memory allocated using ```heap_caps_malloc()``` can be freed by calling
the standard ```free()``` function.

The "soc" component contains a list of memory regions for the chip, along with the type of each memory (aka its tag) and the associated capabilities for that memory type. On startup, a separate heap is initialised for each contiguous memory region. The capabilities-based allocator chooses the best heap for each allocation, based on the requested capabilities.

Special Uses
------------

If a certain memory structure is only addressed in 32-bit units, for example an array of ints or pointers, it can be
useful to allocate it with the MALLOC_CAP_32BIT flag. This also allows the allocator to give out IRAM memory; something
which it can't do for a normal malloc() call. This can help to use all the available memory in the ESP32.

API Reference - Heap Allocation
-------------------------------

.. include:: /_build/inc/esp_heap_alloc_caps.inc

API Reference - Heap Regions
----------------------------

.. include:: /_build/inc/heap_regions.inc
