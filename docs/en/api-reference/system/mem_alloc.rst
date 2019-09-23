Heap Memory Allocation
======================

Stack and Heap
--------------

ESP-IDF applications use the common computer architecture patterns of *stack* (dynamic memory allocated by program control flow) and *heap* (dynamic memory allocated by function calls), as well as statically allocated memory (allocated at compile time).

Because ESP-IDF is a multi-threaded RTOS environment, each RTOS task has its own stack. By default, each of these stacks is allocated from the heap when the task is created. (See :cpp:func:`xTaskCreateStatic` for the alternative where stacks are statically allocated.)

Because ESP32 uses multiple types of RAM, it also contains multiple heaps with different capabilities. A capabilities-based memory allocator allows apps to make heap allocations for different purposes.

For most purposes, the standard libc ``malloc()`` and ``free()`` functions can be used for heap allocation without any special consideration.

However, in order to fully make use of all of the memory types and their characteristics, ESP-IDF also has a
capabilities-based heap memory allocator. If you want to have memory with certain properties (for example, :ref:`dma-capable-memory` or executable-memory), you can create an OR-mask of the required capabilities and pass that to :cpp:func:`heap_caps_malloc`.

Memory Capabilities
-------------------

The ESP32 contains multiple types of RAM:

- DRAM (Data RAM) is memory used to hold data. This is the most common kind of memory accessed as heap.
- IRAM (Instruction RAM) usually holds executable data only. If accessed as generic memory, all accesses must be :ref:`32-bit aligned<32-Bit Accessible Memory>`.
- D/IRAM is RAM which can be used as either Instruction or Data RAM.

For more details on these internal memory types, see :ref:`memory-layout`.

It's also possible to connect external SPI RAM to the ESP32 - :doc:`external RAM </api-guides/external-ram>` can be integrated into the ESP32's memory map using the flash cache, and accessed similarly to DRAM.

DRAM uses capability ``MALLOC_CAP_8BIT`` (accessible in single byte reads and writes). When calling ``malloc()``, the ESP-IDF ``malloc()`` implementation internally calls ``heap_caps_malloc(size, MALLOC_CAP_8BIT)`` in order to allocate DRAM that is byte-addressable. To test the free DRAM heap size at runtime, call cpp:func:`heap_caps_get_free_size(MALLOC_CAP_8BIT)`.

Because malloc uses the capabilities-based allocation system, memory allocated using :cpp:func:`heap_caps_malloc` can be freed by calling
the standard ``free()`` function.

Available Heap
--------------

DRAM
^^^^

At startup, the DRAM heap contains all data memory which is not statically allocated by the app. Reducing statically allocated buffers will increase the amount of available free heap.

To find the amount of statically allocated memory, use the :ref:`idf.py size <idf.py-size>` command.

.. note:: Due to a technical limitation, the maximum statically allocated DRAM usage is 160KB. The remaining 160KB (for a total of 320KB of DRAM) can only be allocated at runtime as heap.

.. note:: At runtime, the available heap DRAM may be less than calculated at compile time, because at startup some memory is allocated from the heap before the FreeRTOS scheduler is started (including memory for the stacks of initial FreeRTOS tasks).

IRAM
^^^^

At startup, the IRAM heap contains all instruction memory which is not used by the app executable code.

The :ref:`idf.py size <idf.py-size>` command can be used to find the amount of IRAM used by the app.

D/IRAM
^^^^^^

Some memory in the ESP32 is available as either DRAM or IRAM. If memory is allocated from a D/IRAM region, the free heap size for both types of memory will decrease.

Heap Sizes
^^^^^^^^^^

At startup, all ESP-IDF apps log a summary of all heap addresses (and sizes) at level Info:

.. code-block:: none

    I (252) heap_init: Initializing. RAM available for dynamic allocation:
    I (259) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
    I (265) heap_init: At 3FFB2EC8 len 0002D138 (180 KiB): DRAM
    I (272) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
    I (278) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
    I (284) heap_init: At 4008944C len 00016BB4 (90 KiB): IRAM

Finding available heap
^^^^^^^^^^^^^^^^^^^^^^

See :ref:`heap-information`.

Special Capabilities
--------------------

.. _dma-capable-memory:

DMA-Capable Memory
^^^^^^^^^^^^^^^^^^

Use the ``MALLOC_CAP_DMA`` flag to allocate memory which is suitable for use with hardware DMA engines (for example SPI and I2S). This capability flag excludes any external PSRAM.

.. _32-bit accessible memory:

32-Bit Accessible Memory
^^^^^^^^^^^^^^^^^^^^^^^^

If a certain memory structure is only addressed in 32-bit units, for example an array of ints or pointers, it can be
useful to allocate it with the ``MALLOC_CAP_32BIT`` flag. This also allows the allocator to give out IRAM memory; something
which it can't do for a normal malloc() call. This can help to use all the available memory in the ESP32.

Memory allocated with ``MALLOC_CAP_32BIT`` can *only* be accessed via 32-bit reads and writes, any other type of access will
generate a fatal LoadStoreError exception.

External SPI Memory
^^^^^^^^^^^^^^^^^^^

When :doc:`external RAM </api-guides/external-ram>` is enabled, external SPI RAM under 4MiB in size can be allocated using standard ``malloc`` calls, or via ``heap_caps_malloc(MALLOC_CAP_SPIRAM)``, depending on configuration. See :ref:`external_ram_config` for more details.

To use the region above the 4MiB limit, you can use the :doc:`himem API</api-reference/system/himem>`.


API Reference - Heap Allocation
-------------------------------

.. include:: /_build/inc/esp_heap_caps.inc

Thread Safety
^^^^^^^^^^^^^

Heap functions are thread safe, meaning they can be called from different tasks simultaneously without any limitations.

It is technically possible to call ``malloc``, ``free``, and related functions from interrupt handler (ISR) context. However this is not recommended, as heap function calls may delay other interrupts. It is strongly recommended to refactor applications so that any buffers used by an ISR are pre-allocated outside of the ISR. Support for calling heap functions from ISRs may be removed in a future update.

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

Knowledge about the regions of memory in the chip comes from the "soc" component, which contains memory layout information for the chip, and the different capabilities of each region. Each region's capabilities are prioritised, so that (for example) dedicated DRAM and IRAM regions will be used for allocations ahead of the more versatile D/IRAM regions.

Each contiguous region of memory contains its own memory heap. The heaps are created using the `multi_heap <API Reference - Multi Heap API>`_ functionality. multi_heap allows any contiguous region of memory to be used as a heap.

The heap capabilities allocator uses knowledge of the memory regions to initialize each individual heap. Allocation functions in the heap capabilities API will find the most appropriate heap for the allocation (based on desired capabilities, available space, and preferences for each region's use) and then calling :cpp:func:`multi_heap_malloc` or :cpp:func:`multi_heap_calloc` for the heap situated in that particular region.

Calling ``free()`` involves finding the particular heap corresponding to the freed address, and then calling :cpp:func:`multi_heap_free` on that particular multi_heap instance.

API Reference - Multi Heap API
------------------------------

(Note: The multi heap API is used internally by the heap capabilities allocator. Most IDF programs will never need to call this API directly.)

.. include:: /_build/inc/multi_heap.inc
