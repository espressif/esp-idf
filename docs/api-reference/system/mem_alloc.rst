Heap Memory Allocation
======================

Overview
--------

The ESP32 has multiple types of RAM. Internally, there's IRAM, DRAM as well as RAM that can be used as both. It's also
possible to connect external SPI RAM to the ESP32 - external RAM can be integrated into the ESP32's memory map using
the flash cache.

For most purposes, the standard libc ``malloc()`` and ``free()`` functions can be used for heap allocation without any
issues.

However, in order to fully make use of all of the memory types and their characteristics, esp-idf also has a
capabilities-based heap memory allocator. If you want to have memory with certain properties (for example, DMA-capable
memory, or executable memory), you can create an OR-mask of the required capabilities and pass that to
:cpp:func:`heap_caps_malloc`. For instance, the standard ``malloc()`` implementation internally allocates memory via
``heap_caps_malloc(size, MALLOC_CAP_8BIT)`` in order to get data memory that is byte-addressable.

Because malloc uses this allocation system as well, memory allocated using :cpp:func:`heap_caps_malloc` can be freed by calling
the standard ``free()`` function.

The "soc" component contains a list of memory regions for the chip, along with the type of each memory (aka its tag) and the associated capabilities for that memory type. On startup, a separate heap is initialised for each contiguous memory region. The capabilities-based allocator chooses the best heap for each allocation, based on the requested capabilities.

Special Uses
------------

DMA-Capable Memory
^^^^^^^^^^^^^^^^^^

Use the MALLOC_CAP_DMA flag to allocate memory which is suitable for use with hardware DMA engines (for example SPI and I2S). This capability flag excludes any external PSRAM.

32-Bit Accessible Memory
^^^^^^^^^^^^^^^^^^^^^^^^

If a certain memory structure is only addressed in 32-bit units, for example an array of ints or pointers, it can be
useful to allocate it with the MALLOC_CAP_32BIT flag. This also allows the allocator to give out IRAM memory; something
which it can't do for a normal malloc() call. This can help to use all the available memory in the ESP32.

Memory allocated with MALLOC_CAP_32BIT can *only* be accessed via 32-bit reads and writes, any other type of access will
generate a fatal LoadStoreError exception.

API Reference - Heap Allocation
-------------------------------

.. include:: /_build/inc/esp_heap_caps.inc

Heap Tracing & Debugging
------------------------

The following features are documented on the :doc:`Heap Memory Debugging </api-reference/system/heap_debug>` page:

- :ref:`Heap Information <heap-information>` (free space, etc.)
- :ref:`Heap Corruption Detection <heap-corruption>`
- :ref:`Heap Tracing <heap-tracing>` (memory leak detection, monitoring, etc.)

API Reference - Initialisation
------------------------------

.. include:: /_build/inc/esp_heap_caps_init.inc

Implementation Notes
--------------------

Knowledge about the regions of memory in the chip comes from the "soc" component, which contains memory layout information for the chip.

Each contiguous region of memory contains its own memory heap. The heaps are created using the `multi_heap <API Reference - Multi Heap API>`_ functionality. multi_heap allows any contiguous region of memory to be used as a heap.

The heap capabilities allocator uses knowledge of the memory regions to initialize each individual heap. When you call a function in the heap capabilities API, it will find the most appropriate heap for the allocation (based on desired capabilities, available space, and preferences for each region's use) and then call the multi_heap function to use the heap situation in that particular region.

API Reference - Multi Heap API
------------------------------

(Note: The multi heap API is used internally by the heap capabilities allocator. Most IDF programs will never need to call this API directly.)

.. include:: /_build/inc/multi_heap.inc
