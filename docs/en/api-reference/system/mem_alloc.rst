Heap Memory Allocation
======================

:link_to_translation:`zh_CN:[中文]`

Stack and Heap
--------------

ESP-IDF applications use the common computer architecture patterns of **stack** (dynamic memory allocated by program control flow), **heap** (dynamic memory allocated by function calls), and **static memory** (memory allocated at compile time).

Because ESP-IDF is a multi-threaded RTOS environment, each RTOS task has its own stack. By default, each of these stacks is allocated from the heap when the task is created. See :cpp:func:`xTaskCreateStatic` for the alternative where stacks are statically allocated.

Because {IDF_TARGET_NAME} uses multiple types of RAM, it also contains multiple heaps with different capabilities. A capabilities-based memory allocator allows apps to make heap allocations for different purposes.

For most purposes, the C Standard Library's ``malloc()`` and ``free()`` functions can be used for heap allocation without any special consideration. However, in order to fully make use of all of the memory types and their characteristics, ESP-IDF also has a capabilities-based heap memory allocator. If you want to have a memory with certain properties (e.g., :ref:`dma-capable-memory` or executable-memory), you can create an OR-mask of the required capabilities and pass that to :cpp:func:`heap_caps_malloc`.

.. _memory_capabilities:

Memory Capabilities
-------------------

The {IDF_TARGET_NAME} contains multiple types of RAM:

- DRAM (Data RAM) is memory that is connected to CPU's data bus and is used to hold data. This is the most common kind of memory accessed as a heap.
- IRAM (Instruction RAM) is memory that is connected to the CPU's instruction bus and usually holds executable data only (i.e., instructions). If accessed as generic memory, all accesses must be aligned to :ref:`32-Bit Accessible Memory <32-Bit Accessible Memory>`.
- D/IRAM is RAM that is connected to CPU's data bus and instruction bus, thus can be used either Instruction or Data RAM.

For more details on these internal memory types, see :ref:`memory-layout`.

.. only:: SOC_SPIRAM_SUPPORTED

    It is also possible to connect external SPI RAM to the {IDF_TARGET_NAME}. The :doc:`external RAM </api-guides/external-ram>` is integrated into the {IDF_TARGET_NAME}'s memory map via the cache, and accessed similarly to DRAM.

All DRAM memory is single-byte accessible, thus all DRAM heaps possess the ``MALLOC_CAP_8BIT`` capability. Users can call ``heap_caps_get_free_size(MALLOC_CAP_8BIT)`` to get the free size of all DRAM heaps.

.. only:: esp32

    If ran out of ``MALLOC_CAP_8BIT``, the users can use ``MALLOC_CAP_IRAM_8BIT`` instead. In that case, IRAM can still be used as a "reserve" pool of internal memory if the users only access it in a 32-bit aligned manner, or if they enable ``CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY)``.


When calling ``malloc()``, the ESP-IDF ``malloc()`` internally calls ``heap_caps_malloc_default(size)``. This will allocate memory with the capability ``MALLOC_CAP_DEFAULT``, which is byte-addressable.

Because ``malloc()`` uses the capabilities-based allocation system, memory allocated using :cpp:func:`heap_caps_malloc` can be freed by calling the standard ``free()`` function.

Available Heap
--------------

.. _dram-definition:

DRAM
^^^^

At startup, the DRAM heap contains all data memory that is not statically allocated by the app. Reducing statically-allocated buffers increases the amount of available free heap.

To find the amount of statically allocated memory, use the :ref:`idf.py size <idf.py-size>` command.

.. only:: esp32

    .. note::

        See the :ref:`dram` section for more details about the DRAM usage limitations.

.. note::

    At runtime, the available heap DRAM may be less than calculated at compile time, because, at startup, some memory is allocated from the heap before the FreeRTOS scheduler is started (including memory for the stacks of initial FreeRTOS tasks).

IRAM
^^^^

At startup, the IRAM heap contains all instruction memory that is not used by the app executable code.

The :ref:`idf.py size <idf.py-size>` command can be used to find the amount of IRAM used by the app.

D/IRAM
^^^^^^

Some memory in the {IDF_TARGET_NAME} is available as either DRAM or IRAM. If memory is allocated from a D/IRAM region, the free heap size for both types of memory will decrease.

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

Finding Available Heap
^^^^^^^^^^^^^^^^^^^^^^

See :ref:`heap-information`.

Special Capabilities
--------------------

.. _dma-capable-memory:

DMA-Capable Memory
^^^^^^^^^^^^^^^^^^

Use the ``MALLOC_CAP_DMA`` flag to allocate memory which is suitable for use with hardware DMA engines (for example SPI and I2S). This capability flag excludes any external PSRAM.

.. only SOC_SPIRAM_SUPPORTED and not esp32::

    The EDMA hardware feature allows DMA buffers to be placed in external PSRAM, but there may be additional alignment constraints. Consult the {IDF_TARGET_NAME} Technical Reference Manual for details. To allocate a DMA-capable external memory buffer, use the ``MALLOC_CAP_SPIRAM`` capabilities flag together with :cpp:func:`heap_caps_aligned_alloc` with the necessary alignment specified.

.. _32-bit accessible memory:

32-Bit Accessible Memory
^^^^^^^^^^^^^^^^^^^^^^^^

If a certain memory structure is only addressed in 32-bit units, for example, an array of ints or pointers, it can be useful to allocate it with the ``MALLOC_CAP_32BIT`` flag. This also allows the allocator to give out IRAM memory, which is sometimes unavailable for a normal ``malloc()`` call. This can help to use all the available memory in the {IDF_TARGET_NAME}.

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA and SOC_CPU_HAS_FPU

    Please note that on {IDF_TARGET_NAME} series chips, ``MALLOC_CAP_32BIT`` cannot be used for storing floating-point variables. This is because ``MALLOC_CAP_32BIT`` may return instruction RAM and the floating-point assembly instructions on {IDF_TARGET_NAME} cannot access instruction RAM.

Memory allocated with ``MALLOC_CAP_32BIT`` can **only** be accessed via 32-bit reads and writes, any other type of access will generate a fatal LoadStoreError exception.

.. only:: SOC_SPIRAM_SUPPORTED

    External SPI Memory
    ^^^^^^^^^^^^^^^^^^^

    When :doc:`external RAM </api-guides/external-ram>` is enabled, external SPI RAM can be allocated using standard ``malloc`` calls, or via ``heap_caps_malloc(MALLOC_CAP_SPIRAM)``, depending on the configuration. See :ref:`external_ram_config` for more details.

    .. only:: esp32

        On ESP32 only external SPI RAM under 4 MiB in size can be allocated this way. To use the region above the 4 MiB limit, you can use the :doc:`himem API </api-reference/system/himem>`.

Thread Safety
-------------

Heap functions are thread-safe, meaning they can be called from different tasks simultaneously without any limitations.

It is technically possible to call ``malloc``, ``free``, and related functions from interrupt handler (ISR) context (see :ref:`calling-heap-related-functions-from-isr`). However, this is not recommended, as heap function calls may delay other interrupts. It is strongly recommended to refactor applications so that any buffers used by an ISR are pre-allocated outside of the ISR. Support for calling heap functions from ISRs may be removed in a future update.

.. _calling-heap-related-functions-from-isr:

Calling Heap-Related Functions from ISR
---------------------------------------

The following functions from the heap component can be called from the interrupt handler (ISR):

* :cpp:func:`heap_caps_malloc`
* :cpp:func:`heap_caps_malloc_default`
* :cpp:func:`heap_caps_realloc_default`
* :cpp:func:`heap_caps_malloc_prefer`
* :cpp:func:`heap_caps_realloc_prefer`
* :cpp:func:`heap_caps_calloc_prefer`
* :cpp:func:`heap_caps_free`
* :cpp:func:`heap_caps_realloc`
* :cpp:func:`heap_caps_calloc`
* :cpp:func:`heap_caps_aligned_alloc`
* :cpp:func:`heap_caps_aligned_free`

.. note::

    However, this practice is strongly discouraged.

Heap Tracing & Debugging
------------------------

The following features are documented on the :doc:`Heap Memory Debugging </api-reference/system/heap_debug>` page:

- :ref:`Heap Information <heap-information>` (free space, etc.)
- :ref:`Heap Allocation and Free Function Hooks <heap-allocation-free>`
- :ref:`Heap Corruption Detection <heap-corruption>`
- :ref:`Heap Tracing <heap-tracing>` (memory leak detection, monitoring, etc.)

Implementation Notes
--------------------

Knowledge about the regions of memory in the chip comes from the "SoC" component, which contains memory layout information for the chip, and the different capabilities of each region. Each region's capabilities are prioritized, so that (for example) dedicated DRAM and IRAM regions are used for allocations ahead of the more versatile D/IRAM regions.

Each contiguous region of memory contains its own memory heap. The heaps are created using the :ref:`multi_heap <multi-heap>` functionality. ``multi_heap`` allows any contiguous region of memory to be used as a heap.

The heap capabilities allocator uses knowledge of the memory regions to initialize each individual heap. Allocation functions in the heap capabilities API will find the most appropriate heap for the allocation based on desired capabilities, available space, and preferences for each region's use, and then calling :cpp:func:`multi_heap_malloc` for the heap situated in that particular region.

Calling ``free()`` involves finding the particular heap corresponding to the freed address, and then call :cpp:func:`multi_heap_free` on that particular ``multi_heap`` instance.


API Reference - Heap Allocation
-------------------------------

.. include-build-file:: inc/esp_heap_caps.inc


API Reference - Initialisation
------------------------------

.. include-build-file:: inc/esp_heap_caps_init.inc

.. _multi-heap:

API Reference - Multi-Heap API
------------------------------

(Note: The multi-heap API is used internally by the heap capabilities allocator. Most ESP-IDF programs never need to call this API directly.)

.. include-build-file:: inc/multi_heap.inc
