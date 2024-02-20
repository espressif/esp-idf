FreeRTOS 概述
===================

:link_to_translation:`en:[English]`

概述
--------

FreeRTOS 是一个开源的 RTOS（实时操作系统）内核，它以组件的形式集成到 ESP-IDF 中。因此，所有的 ESP-IDF 应用程序及多种 ESP-IDF 组件都基于 FreeRTOS 编写。FreeRTOS 内核已移植到 ESP 芯片的所有 CPU 架构（即 Xtensa 和 RISC-V）中。

此外，ESP-IDF 还提供了不同的 FreeRTOS 实现，支持在多核 ESP 目标芯片上的 SMP（对称多处理）。本文档主要介绍了 FreeRTOS 组件、ESP-IDF 提供的 FreeRTOS 实现，并简要介绍了这些实现的共同之处。

实现
---------------

`官方 FreeRTOS <https://www.freertos.org/zh-cn-cmn-s/index.html>`__ （下文称为原生 FreeRTOS）是一个单核 RTOS。为了支持各种多核 ESP 目标芯片，ESP-IDF 支持下述不同的 FreeRTOS 实现。

ESP-IDF FreeRTOS
^^^^^^^^^^^^^^^^

    ESP-IDF FreeRTOS 是基于原生 FreeRTOS v10.5.1 的 FreeRTOS 实现，其中包含支持 SMP 的大量更新。ESP-IDF FreeRTOS 最多支持两个核（即双核 SMP），但在设计上对这种场景进行了优化。关于 ESP-IDF FreeRTOS 及具体更新内容，请参考 :doc:`freertos_idf` 文档。

    .. note::

        ESP-IDF FreeRTOS 是目前 ESP-IDF 默认的 FreeRTOS 实现。

.. only:: not esp32p4

    .. _amazon_smp_freertos:

    Amazon SMP FreeRTOS
    ^^^^^^^^^^^^^^^^^^^

        Amazon SMP FreeRTOS 是由 Amazon 官方支持的 FreeRTOS SMP 实现。Amazon SMP FreeRTOS 能够支持 N 核，即双核以上。通过 :ref:`CONFIG_FREERTOS_SMP` 选项能够启用 Amazon SMP FreeRTOS。关于 Amazon SMP FreeRTOS 的更多细节，请参考 `官方 Amazon SMP FreeRTOS 文档 <https://freertos.org/zh-cn-cmn-s/symmetric-multiprocessing-introduction.html>`__。

        .. warning::

            Amazon SMP FreeRTOS 实现（及其在 ESP-IDF 中的移植）目前处于试验/测试状态，因此，可能会发生重大的行为变化及 API 变更。

配置
-------------

内核配置
^^^^^^^^^^^^^^^^^^^^

原生 FreeRTOS 要求移植工具和应用程序通过在 ``FreeRTOSConfig.h`` 头文件中添加各种 ``#define config...`` 宏定义来配置内核。原生 FreeRTOS 支持一系列内核配置选项，允许启用或禁用各种内核行为和功能。

**然而，对于 ESP-IDF 中的所有 FreeRTOS 移植，FreeRTOSConfig.h 头文件被视为私有文件，用户不得修改。** 由于该选项在 ESP-IDF 中是必选项或不被支持，``FreeRTOSConfig.h`` 中的大量内核配置选项均为硬编码。所有用户可配置的内核配置选项都在 ``Component Config/FreeRTOS/Kernel`` 下的 menuconfig 中。

关于用户可配置内核选项的完整列表，参见 :doc:`/api-reference/kconfig`。下列为常用的内核配置选项：

- :ref:`CONFIG_FREERTOS_UNICORE`：仅在核 0 上运行 FreeRTOS。注意，这 **不等同于运行原生 FreeRTOS。** 另外，此选项还可能影响除 :component:`freertos` 外其他组件的行为。关于在单核上运行 FreeRTOS 的更多内容，请参考 :ref:`freertos-idf-single-core` （使用 ESP-IDF FreeRTOS 时）或参考 Amazon SMP FreeRTOS 的官方文档，还可以在 ESP-IDF 组件中搜索 ``CONFIG_FREERTOS_UNICORE``。

.. only:: not SOC_HP_CPU_HAS_MULTIPLE_CORES

    .. note::
        由于 {IDF_TARGET_NAME} 是一个单核 SoC，所以总是会启用 :ref:`CONFIG_FREERTOS_UNICORE` 配置。

- :ref:`CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY` 可以向后兼容某些 FreeRTOS 宏、类型或函数，这些宏、类型或函数已在 v8.0 及以上版本中弃用。

端口配置
^^^^^^^^^^^^^^^^^^

其他不属于内核配置的 FreeRTOS 相关配置选项都在 ``Component Config/FreeRTOS/Port`` 下的 menuconfig 中。这些选项可以配置以下内容：

- FreeRTOS 端口本身（如 tick 定时器选择，ISR 堆栈大小）
- 其他添加到 FreeRTOS 实现或端口的功能

使用 FreeRTOS
----------------

应用程序入口点
^^^^^^^^^^^^^^^^^^^^^^^

与原生 FreeRTOS 不同，在 ESP-IDF 中使用 FreeRTOS 的用户 **永远不应调用** :cpp:func:`vTaskStartScheduler` 和 :cpp:func:`vTaskEndScheduler`。相反，ESP-IDF 会自动启动 FreeRTOS。用户必须定义一个 ``void app_main(void)`` 函数作为用户应用程序的入口点，并在 ESP-IDF 启动时被自动调用。

- 通常，用户会从 ``app_main`` 中启动应用程序的其他任务。
- ``app_main`` 函数可以在任何时候返回（应用终止前）。
- ``app_main`` 函数由 ``main`` 任务调用。

.. _freertos_system_tasks:

后台任务
^^^^^^^^^^^^^^^^

在启动过程中，ESP-IDF 和 FreeRTOS 内核会自动创建多个在后台运行的任务，如下表所示。

.. list-table:: 启动过程创建任务列表
    :widths: 10 75 5 5 5
    :header-rows: 1

    * - 任务名称
      - 描述
      - 堆栈大小
      - 亲和性
      - 优先级
    * - 空闲任务 (``IDLEx``)
      - 为每个 CPU 核创建并分配一个空闲任务 (``IDLEx``)，其中 ``x`` 是 CPU 核的编号。 当启用单核配置时，``x`` 将被删除。
      - :ref:`CONFIG_FREERTOS_IDLE_TASK_STACKSIZE`
      - 核 x
      - ``0``
    * - FreeRTOS 定时器任务 (``Tmr Svc``)
      - 如果应用程序调用了任何 FreeRTOS 定时器 API，FreeRTOS 会创建定时器服务或守护任务
      - :ref:`CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH`
      - 核 0
      - :ref:`CONFIG_FREERTOS_TIMER_TASK_PRIORITY`
    * - 主任务 (``main``)
      - 简单调用 ``app_main`` 的任务在 ``app_main`` 返回时会自我删除
      - :ref:`CONFIG_ESP_MAIN_TASK_STACK_SIZE`
      - :ref:`CONFIG_ESP_MAIN_TASK_AFFINITY`
      - ``1``
    * - IPC 任务 (``ipcx``)
      - 当 :ref:`CONFIG_FREERTOS_UNICORE` 为假时，为每个 CPU 核创建并分配一个 IPC 任务 (``ipcx``)。IPC 任务用于实现处理器间调用 (IPC) 功能
      - :ref:`CONFIG_ESP_IPC_TASK_STACK_SIZE`
      - 核 x
      - ``24``
    * - ESP 定时器任务 (``esp_timer``)
      - ESP-IDF 创建 ESP 定时器任务用于处理 ESP 定时器回调
      - :ref:`CONFIG_ESP_TIMER_TASK_STACK_SIZE`
      - 核 0
      - ``22``

.. note::
    注意，如果应用程序使用了其他 ESP-IDF 功能（如 Wi-Fi 或蓝牙），那么这些功能可能会在上表的任务之外创建自己的后台任务。

FreeRTOS 附加功能
------------------

ESP-IDF 还为 FreeRTOS 提供了一些补充功能，如环形 buffer、ESP-IDF 风格的 tick 钩子和 idle 钩子、以及 TLSP 删除回调。要了解更多信息，请参见 :doc:`freertos_additions`。

.. _freertos-heap:

FreeRTOS 堆
-------------

原生 FreeRTOS 自带 `堆实现选择 <https://www.freertos.org/a00111.html>`_，然而 ESP-IDF 已经实现了自己的堆（参见 :doc:`/api-reference/system/mem_alloc`），因此不使用原生 FreeRTOS 的堆实现。ESP-IDF 中的所有 FreeRTOS 端口都将 FreeRTOS 内存分配或释放调用（例如 ``pvPortMalloc()`` 和 ``pvPortFree()``）映射到 ESP-IDF 堆 API（即 :cpp:func:`heap_caps_malloc` 和 :cpp:func:`heap_caps_free`）。然而 FreeRTOS 端口可以确保 FreeRTOS 分配的所有动态内存都放在内部内存中。

.. note::
    如果希望将 FreeRTOS 任务或对象放在外部内存中，可以使用以下方法：

    - 使用一个 ``...CreateWithCaps()`` API，如 :cpp:func:`xTaskCreateWithCaps` 和 :cpp:func:`xQueueCreateWithCaps` 来分配任务或对象（参见 :ref:`freertos-idf-additional-api` 获取更多详细信息）。
    - 使用 :cpp:func:`heap_caps_malloc` 为这些对象手动分配外部内存，然后使用 FreeRTOS 的一个 ``...CreateStatic()`` 函数从分配的内存中创建对象。
