堆内存分配
======================

:link_to_translation:`en:[English]`

栈 (stack) 和堆 (heap) 的区别
----------------------------------------

ESP-IDF 应用程序使用常见的计算机架构模式：由程序控制流动态分配的内存（即 **栈**）、由函数调用动态分配的内存（即 **堆**）以及在编译时分配的 **静态内存**。

由于 ESP-IDF 是一个多线程 RTOS 环境，因此每个 RTOS 任务都有自己的栈，这些栈默认在创建任务时从堆中分配。有关栈静态分配的方法，请参阅 :cpp:func:`xTaskCreateStatic`。

{IDF_TARGET_NAME} 使用多种类型的 RAM，因此具备不同属性的堆，即基于属性的内存分配器允许应用程序按不同目的进行堆分配。

多数情况下，可直接使用 C 标准函数库中的 ``malloc()`` 和 ``free()`` 函数实现堆分配。为充分利用各种内存类型及其特性，ESP-IDF 还具有基于内存属性的堆内存分配器。要配备具有特定属性的内存，如 :ref:`dma-capable-memory` 或可执行内存，可以创建具备所需属性的 OR 掩码，将其传递给 :cpp:func:`heap_caps_malloc`。

.. _memory_capabilities:

内存属性
-------------------

{IDF_TARGET_NAME} 包含多种类型的 RAM：

- DRAM（数据 RAM）是连接到 CPU 数据总线上的内存，用于存储数据。这是作为堆访问最常见的一种内存。
- IRAM（指令 RAM）是连接到 CPU 指令总线上的内存，通常仅用于存储可执行数据（即指令）。如果作为通用内存访问，则所有访问必须为 :ref:`32 位可访问内存 <32-Bit Accessible Memory>`。
- D/IRAM 是连接到 CPU 数据总线和指令总线的 RAM，因此可用作指令 RAM 或数据 RAM。

有关内存类型的详细信息，请参阅 :ref:`memory-layout`。

.. only:: SOC_SPIRAM_SUPPORTED

    也可将外部 SPI RAM 连接到 {IDF_TARGET_NAME}。通过缓存将 :doc:`片外 RAM </api-guides/external-ram>` 集成到 {IDF_TARGET_NAME} 的内存映射中，访问方式与 DRAM 类似。

所有的 DRAM 内存都可以单字节访问，因此所有的 DRAM 堆都具有 ``MALLOC_CAP_8BIT`` 属性。要获取所有 DRAM 堆的剩余空间大小，请调用 ``heap_caps_get_free_size(MALLOC_CAP_8BIT)``。

.. only:: esp32

    如果占用了所有的 ``MALLOC_CAP_8BIT`` 堆空间，则可以用 ``MALLOC_CAP_IRAM_8BIT`` 代替。此时，若只以 32 位对齐的方式访问 IRAM 内存，或者启用了 ``CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY``，则仍然可以将 IRAM 用作内部内存的“储备池”。


调用 ``malloc()`` 时，ESP-IDF ``malloc()`` 内部调用 ``heap_caps_malloc_default(size)``，使用属性 ``MALLOC_CAP_DEFAULT`` 分配内存。该属性可实现字节寻址功能，即存储空间的最小编址单位为字节。

``malloc()`` 使用基于属性的分配系统，所以使用 :cpp:func:`heap_caps_malloc` 分配的内存可以通过调用标准的 ``free()`` 函数释放。

可用堆空间
-------------------

.. _dram-definition:

DRAM
^^^^

启动时，DRAM 堆包含应用程序未静态分配的所有数据内存，减少静态分配的缓冲区将增加可用的空闲堆空间。

调用命令 :ref:`idf.py size <idf.py-size>` 可查找静态分配内存大小。

.. only:: esp32

    .. note::

        有关 DRAM 使用限制的详细信息，请参阅 :ref:`dram` 。

.. note::

    运行时可用的 DRAM 堆空间可能少于编译时计算的大小，因为启动时会在运行 FreeRTOS 调度程序之前从堆中分配部分内存，包括初始 FreeRTOS 任务的栈内存。

IRAM
^^^^

启动时，IRAM 堆包含所有应用程序可执行代码未使用的指令内存。

调用命令 :ref:`idf.py size <idf.py-size>` 查找应用程序使用的 IRAM 量。

D/IRAM
^^^^^^

一些内存在 {IDF_TARGET_NAME} 中可用作 DRAM 或 IRAM。如果从 D/IRAM 区域分配内存，则两种类型的内存的可用堆空间都会减少。

堆空间大小
^^^^^^^^^^^^^^

启动时，所有 ESP-IDF 应用程序都会记录全部堆地址（和空间大小）的摘要，级别为 Info：

.. code-block:: none

    I (252) heap_init: Initializing. RAM available for dynamic allocation:
    I (259) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
    I (265) heap_init: At 3FFB2EC8 len 0002D138 (180 KiB): DRAM
    I (272) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
    I (278) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
    I (284) heap_init: At 4008944C len 00016BB4 (90 KiB): IRAM

查找可用堆
^^^^^^^^^^^^^^^^^^^^^^

请参阅 :ref:`heap-information`。

特殊用途
--------------------

.. _dma-capable-memory:

DMA 存储器
^^^^^^^^^^^^^^^^^^

使用 ``MALLOC_CAP_DMA`` 标志分配适合与硬件 DMA 引擎（如 SPI 和 I2S）配合使用的内存，此属性标志不包括外部 PSRAM。

.. only SOC_SPIRAM_SUPPORTED and not esp32::

    EDMA 硬件功能允许将 DMA 缓冲区放置在外部 PSRAM，但可能存在其他对齐限制，详情请参阅 {IDF_TARGET_NAME} 技术参考手册。要分配一个可用 DMA 的外部内存缓冲区，请使用 ``MALLOC_CAP_SPIRAM`` 属性标志 和 :cpp:func:`heap_caps_aligned_alloc`，并指定必要的对齐方式。

.. _32-bit accessible memory:

32 位可访问内存
^^^^^^^^^^^^^^^^^^^^^^^^

如果某个内存结构体仅以 32 位为单位寻址，例如一个整数或指针数组，则可以使用 ``MALLOC_CAP_32BIT`` 标志分配。通过这一方式，分配器能够在无法调用 ``malloc()`` 的情况下提供 IRAM 内存，从而充分利用 {IDF_TARGET_NAME} 中的所有可用内存。

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA and SOC_CPU_HAS_FPU

    请注意，在 {IDF_TARGET_NAME} 系列芯片上，不可使用 ``MALLOC_CAP_32BIT`` 存储浮点变量。因为 ``MALLOC_CAP_32BIT`` 可能返回指令 RAM，而 {IDF_TARGET_NAME} 上的浮点汇编指令无法访问指令 RAM。

请注意，使用 ``MALLOC_CAP_32BIT`` 分配的内存 *只能* 通过 32 位读写访问，其他类型的访问将导致 LoadStoreError 异常。

.. only:: SOC_SPIRAM_SUPPORTED

    外部 SPI 内存
    ^^^^^^^^^^^^^^^^^^^

    当启用 :doc:`片外 RAM </api-guides/external-ram>` 时，可以根据配置调用标准 ``malloc`` 或通过 ``heap_caps_malloc(MALLOC_CAP_SPIRAM)`` 分配外部 SPI RAM，详情请参阅 :ref:`external_ram_config`。

    .. only:: esp32

        在 ESP32 上，只有不超过 4 MiB 的外部 SPI RAM 可以通过上述方式分配。要使用超过 4 MiB 限制的区域，可以使用 :doc:`himem API</api-reference/system/himem>`。

线程安全性
-------------

堆函数是线程安全的，因此可不受限制，在不同任务中同时调用多个堆函数。

从中断处理程序 (ISR) 上下文中调用 ``malloc``、 ``free`` 和相关函数虽然在技术层面可行（请参阅 :ref:`calling-heap-related-functions-from-isr`），但不建议使用此种方法，因为调用堆函数可能会延迟其他中断。建议重构应用程序，将 ISR 使用的任何缓冲区预先分配到 ISR 之外。之后可能会删除从 ISR 调用堆函数的功能。

.. _calling-heap-related-functions-from-isr:

从 ISR 调用堆相关函数
---------------------------------------

堆组件中的以下函数可以在中断处理程序 (ISR) 中调用：

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

    不建议使用此种方法。

堆跟踪及调试
------------------------

以下功能介绍详见 :doc:`堆内存调试 </api-reference/system/heap_debug>`：

- :ref:`堆信息 <heap-information>` （释放内存空间等）
- :ref:`堆分配与释放函数挂钩 <heap-allocation-free>`
- :ref:`堆损坏检测 <heap-corruption>`
- :ref:`堆跟踪 <heap-tracing>` （检测、监控内存泄漏等）

实现说明
--------------------

堆属性分配器对芯片内存区域的了解源于 SoC 组件，该组件包含芯片的内存布局信息以及每个区域的不同属性。各区域的功能为首要考虑因素，如会优先使用 DRAM 和 IRAM 特定区域而非用途更广的 D/IRAM 区域来分配内存。

每个连续的内存区域都包含其自己的内存堆，由 :ref:`multi_heap <multi-heap>` 函数创建。 ``multi_heap`` 允许将任何连续的内存区域作为堆使用。

堆属性分配器通过对内存区域的了解初始化每个单独的堆，堆属性 API 中的分配函数将基于所需的属性、可用空间和每个区域使用的首选项为分配函数找到最合适的堆，随后为位于特定内存区域的堆调用 :cpp:func:`multi_heap_malloc`。

调用 ``free()`` 查找对应释放地址的特定堆，随后在特定的 ``multi_heap`` 实例上调用 :cpp:func:`multi_heap_free`。


API 参考 - 堆分配
-------------------------------

.. include-build-file:: inc/esp_heap_caps.inc


API 参考 - 初始化
------------------------------

.. include-build-file:: inc/esp_heap_caps_init.inc

.. _multi-heap:

API 参考 - 多堆 API
------------------------------

（注意：堆属性分配器在内部使用多堆 API，而多数 IDF 程序不需要直接调用此 API。）

.. include-build-file:: inc/multi_heap.inc
