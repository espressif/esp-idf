ESP 定时器（高分辨率定时器）
============================

:link_to_translation:`en:[English]`

{IDF_TARGET_HR_TIMER:default = "SYSTIMER", esp32 = "LAC 定时器"}

{IDF_TARGET_HR_TIMER_Resolution:default = "52", esp32 = "64", esp32s2 = "64"}

.. only:: html

    本文介绍了 ESP-IDF 的 ESP 定时器功能，章节目录如下：

    .. contents::
        :local:
        :depth: 2


概述
----

使用 ESP 定时器可以创建软件定时器并在超时时调用其回调函数（分发回调函数）。尤其适用于当软件需要执行延迟操作或周期性操作的场景，例如：延迟启动设备或停止设备、周期性采样传感器数据等。

ESP 定时器可以简化一些操作，例如：管理多个定时器、分发回调函数、处理时钟频率变化（如果启用了动态调频），以及从浅睡眠中唤醒后保持准确的时间。

如果应用场景对实时性能要求较高（例如需要生成波形），或要求定时器分辨率可配置，建议使用 :doc:`GPTimer </api-reference/peripherals/gptimer>`。此外，GPTimer 还提供一些其他功能，例如事件捕获。

FreeRTOS 有其单独的软件定时器，但分辨率比 ESP 定时器低得多，优势在于 FreeRTOS 定时器有更好的移植性，因为它不依赖于 ESP-IDF。详情请参阅 :ref:`FreeRTOS Timers`。


功能与概念
----------

ESP 定时器 API 提供：

- 一次性和周期性定时器
- 多种分发回调函数的方法
- 处理过期回调函数
- 计数器位宽：{IDF_TARGET_HR_TIMER_Resolution} 位
- 时间分辨率：1 us


一次性和周期性定时器
^^^^^^^^^^^^^^^^^^^^

一次性定时器到期时仅调用一次回调函数便停止运行。一次性定时器适用于单次延迟操作，例如在指定时间间隔后关闭设备或读取传感器。

周期性定时器在到期时会调用回调函数并自动重启，回调函数会在指定时间间隔内被反复调用，直至手动暂停周期性定时器。周期性定时器适用于重复操作，例如采样传感器数据、更新显示信息或生成波形。


.. _Callback Methods:

分发回调函数的方法
^^^^^^^^^^^^^^^^^^

定时器回调函数可以使用以下方法进行分发：

- 任务分发法（默认）：

    - 从单个高优先级 ESP 定时器任务中分发定时器回调函数（esp_timer 任务（由 ISR 通知）> 回调）。
    - 适用于对时序要求不高的定时器回调函数。

- 中断分发法 (:cpp:enumerator:`ESP_TIMER_ISR <esp_timer_dispatch_t::ESP_TIMER_ISR>`)：

    - 直接从中断处理程序分发定时器回调函数（ISR > 回调）。
    - 适合运行时间仅为几微秒的简单、低延迟定时器回调函数。
    - 能确保事件和执行回调之间的延迟较短。
    - 不受其他活动任务影响。


任务分发细节
~~~~~~~~~~~~

在 ESP 定时器任务中，回调函数是串行执行的，若多个超时同时发生，执行前一项回调函数会延迟后续回调函数的执行。因此，建议尽量保持回调函数简短。如果需要回调函数执行更多工作，应使用 FreeRTOS 原语（如队列和信号量）将工作推迟到低优先级任务中执行。

如果有其他更高优先级的 FreeRTOS 任务正在运行（例如 SPI flash 操作），则回调函数的分发将被延迟，直到 ESP 定时器任务有机会再次运行。

为了确保任务的可预测性和及时执行，回调函数不应进行阻塞（等待资源）或让步（放弃控制）操作，否则将中断回调函数的串行执行。


中断分发细节
~~~~~~~~~~~~

使用中断分发法的定时器，其回调由中断处理程序执行。由于中断可以抢占所有任务，中断分发法带来的延迟较低。中断分发的定时器回调函数不应尝试阻塞，也不应尝试通过 ``portYIELD_FROM_ISR()`` 触发上下文切换，而应使用 :cpp:func:`esp_timer_isr_dispatch_need_yield` 函数。上下文切换将在处理完所有使用 ISR 分发法的定时器后进行。

使用中断分发的定时器时，应避免使用标准的日志记录或调试方法（例如 ``printf``）。若想调试应用程序或在控制台中显示某些信息，应使用 ESP-IDF 日志宏，例如 :c:macro:`ESP_DRAM_LOGI` 和 :c:macro:`ESP_EARLY_LOGI` 等。这些宏专为在各种上下文（包括中断服务程序）中工作而设计。


获取当前时间
^^^^^^^^^^^^

可以使用便捷函数 :cpp:func:`esp_timer_get_time` 获取自 ESP 定时器初始化以来经过的时间。在调用 ``app_main`` 函数之前不久，ESP 定时器会开始初始化。该便捷函数速度极快，没有能引入延迟或冲突的锁机制，因此可用于细粒度定时，在任务和 ISR 例程中的精度为 1 μs。

与 ``gettimeofday()`` 函数不同，:cpp:func:`esp_timer_get_time` 具有以下特点：

- 从深睡眠状态中唤醒后，初始化定时器将从零开始。
- 返回值没有时区设置或夏令时调整。


系统集成
--------

本节主要介绍如何优化 ESP 定时器的操作并将其与其他 ESP-IDF 功能进行集成。


超时值限制
^^^^^^^^^^

分发回调不可能是瞬时的，使用 ESP 定时器创建的一次性和周期性定时器具有超时值限制。由于这些限制取决于多个因素，所以无法进行精确估计。

例如，ESP32 以 240 MHz 的频率运行并使用任务分发法，其最小超时值大约如下：

* 一次性定时器：~20 μs

    * 如果调用 :cpp:func:`esp_timer_start_once`，这是系统能够分发回调函数的最小超时值。

* 周期性定时器：~50 μs

    * 具有较小超时值的周期性软件定时器将消耗大部分 CPU 时间，因此不实用。

CPU 频率越低，最小超时值就越高。一般来说，如果所需的超时值在几十微秒的范围内，则应用程序需要经过彻底测试才能确保稳定运行。

如果最小超时值稍稍超过要求，可以考虑使用中断分发法。

.. only:: not SOC_PARLIO_SUPPORTED and SOC_RMT_SUPPORTED

    若需要更小的超时值，例如生成或接收波形、进行位操作时，ESP 定时器的分辨率可能不能满足要求。此时建议使用专用外设，例如 :doc:`GPTimer </api-reference/peripherals/gptimer>` 或 :doc:`RMT </api-reference/peripherals/rmt>`，以及使用它们的 DMA 功能（如果可用）。

.. only:: SOC_PARLIO_SUPPORTED

    若需要更小的超时值，例如生成或接收波形、进行位操作时，ESP 定时器的分辨率可能不能满足要求。此时建议使用专用外设，例如 :doc:`并行 IO </api-reference/peripherals/parlio>`，以及使用它们的 DMA 功能（如果可用）。


睡眠模式注意事项
^^^^^^^^^^^^^^^^

如果启动了定时器，并且在等待时间内没有执行其他任务，则可以将芯片置于睡眠状态以优化功耗。

可以通过以下方式进入不同睡眠模式：

* **自动睡眠** 由 :doc:`电源管理 API <power_management>` 提供：如果没有正在执行的任务，芯片会自动进入浅睡眠状态，并在适当时间自动唤醒，以便 ESP 定时器分发待处理的回调函数。
* **手动睡眠** 由 :doc:`睡眠模式 API <sleep_modes>` 提供：无论是否正在执行其他任务，都可以将芯片置于睡眠状态。

若手动设置睡眠状态，则可以选择以下睡眠模式：

* Deep-sleep 模式：ESP 定时器停用

    从深睡眠状态中唤醒时，应用程序即刻重新启动，因此该模式不适用于连续的 ESP 定时器操作。但如果不需要定时器在唤醒后持续运行，则可进入深睡眠状态。

* Light-sleep 模式：ESP 定时器暂停

    在浅睡眠状态下，ESP 定时器的计数器和回调函数会被暂停。RTC 定时器可保持系统时间。一旦芯片被唤醒，ESP 定时器的计数器会自动为系统增加睡眠期间的时长，之后时间保持和回调函数执行将恢复。

    此时，ESP 定时器将尝试分发所有未处理的回调函数（如果有的话），可能会导致 ESP 定时器回调执行队列的溢出。某些应用中不应出现此类行为，为避免这种情况，可参阅 :ref:`Handling Callbacks in Light Sleep`。


.. _FreeRTOS Timers:

FreeRTOS 定时器
^^^^^^^^^^^^^^^

尽管 FreeRTOS 提供了 `软件定时器 <https://www.freertos.org/RTOS-software-timer.html>`_，但它们有以下限制：

- FreeRTOS 定时器的分辨率受 `tick 频率 <https://www.freertos.org/a00110.html#configTICK_RATE_HZ>`_ 的限制，该频率通常在 100 到 1000 Hz 之间。
- 定时器回调函数由低优先级定时器任务分发，该任务可能会被其他任务抢占，导致定时器精度和准确度下降。

但 FreeRTOS 定时器是可移植的（不依赖于 ESP-IDF），且不会从 ISR 中分发，因此具有确定性。


.. only:: SOC_ETM_SUPPORTED and SOC_SYSTIMER_SUPPORT_ETM

    ETM 事件
    ^^^^^^^^

    ESP 定时器连接到 :doc:`事件任务矩阵 </api-reference/peripherals/etm>` (ETM) 模块。该模块能够在不涉及 CPU 中断的情况下通知多个外设。直接通知可以减少延迟并降低 CPU 负载。可以调用函数 :cpp:func:`esp_timer_new_etm_alarm_event` 获取相应的 ETM 事件句柄。


使用方法
--------

在设置 ESP-IDF 项目时，请确保：

- 在 ``CMakeLists.txt`` 中添加所需的组件依赖项 ``esp_timer``。
- 在 ``.c`` 文件中包含所需的头文件 ``esp_timer.h``。
- （可选）设置 Kconfig 选项。详见 :ref:`Kconfig 选项 <component-config-esp-timer-high-resolution-timer->` > *ESP 定时器（高分辨率定时器）*


.. _General Procedure:

一般程序
^^^^^^^^

创建、启动、暂停和删除定时器的一般程序如下：

1. 创建定时器

    - 使用类型 :cpp:type:`esp_timer_handle_t` 定义定时器句柄
    - 通过定义结构体 :cpp:struct:`esp_timer_create_args_t` （包括回调函数）来设置定时器配置参数。

        .. note::

            建议尽量使回调函数保持简短，避免延迟其他回调函数的执行。

    - 调用函数 :cpp:func:`esp_timer_create` 来创建定时器。

2. 根据需求启动一次性或周期性的定时器

    - 调用函数 :cpp:func:`esp_timer_start_once`，启动一次性定时器。
    - 调用函数 :cpp:func:`esp_timer_start_periodic`，启动周期性定时器。在调用函数 :cpp:func:`esp_timer_stop` 显式暂停定时器前，该周期性定时器将持续运行。

    .. note::

        执行启动函数前，请确保定时器未在运行。如果定时器正在运行，请先调用 :cpp:func:`esp_timer_restart`，或是调用 :cpp:func:`esp_timer_stop` 暂停定时器，然后再使用上述启动函数。

3. 暂停定时器

    - 调用函数 :cpp:func:`esp_timer_stop`，可暂停运行中的定时器。

4. 删除定时器

    - 使用函数 :cpp:func:`esp_timer_delete`，可删除不需要的定时器以释放内存。


.. _Using ESP_TIMER_ISR Callback Method:

使用中断分发法
^^^^^^^^^^^^^^

在可用的 :ref:`分发回调函数的方法 <Callback Methods>` 中，如果选择中断分发法，请按以下步骤操作：

1. 设置 Kconfig 选项

    - 启用 :ref:`CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD`。

2. 创建定时器

    - 通过定义结构体 :cpp:struct:`esp_timer_create_args_t` 来设置定时器配置参数：

    .. code-block:: c

        const esp_timer_create_args_t timer = {
            ... ,
            .dispatch_method = ESP_TIMER_ISR,
            ...
        };

    - 调用函数 :cpp:func:`esp_timer_create` 来创建定时器。

更多步骤请参阅 :ref:`General Procedure`。


.. _Handling Callbacks in Light Sleep:

在 Light-sleep 模式下处理回调函数
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

浅睡眠状态下，既能快速唤醒以执行特定操作，又能节省功耗。要想结合 Light-sleep 模式使用 ESP 定时器，请参阅 :doc:`睡眠模式 API <sleep_modes>`。

在浅睡眠状态下，为控制未处理的回调函数，并且避免唤醒时 ESP 定时器回调执行队列的溢出，请执行以下任一操作：

- 首先要防止调用回调函数：在进入浅睡眠状态前，请使用函数 :cpp:func:`esp_timer_stop` 暂停定时器。
- 若出于某种原因不希望调用停止函数，请使用选项 :cpp:member:`esp_timer_create_args_t::skip_unhandled_events`。此时，若周期性定时器在浅睡眠状态下到期一次或多次，则唤醒时只执行一次回调函数。


调试定时器
^^^^^^^^^^

使用函数 :cpp:func:`esp_timer_dump`，可转储所有定时器或仅运行中的定时器的相关信息：如定时器的参数、定时器启动次数、触发次数、跳过次数以及执行定时器回调函数所需的时间，这些信息能够帮助调试定时器。

请按照以下步骤调试定时器：

1. 设置 Kconfig 选项以获取更详细的输出：

    - 启用 :ref:`CONFIG_ESP_TIMER_PROFILING`。

    .. note::

        启用此选项会增加代码大小和堆内存使用量。

2. 调用函数 :cpp:func:`esp_timer_dump`，在代码中必要的位置打印信息并用于调试定时器。

3. 结束调试后，考虑禁用 :ref:`CONFIG_ESP_TIMER_PROFILING`。


故障排除
--------

回调函数分发时间不稳定
^^^^^^^^^^^^^^^^^^^^^^

如果多次分发相同的回调函数时响应时间变化较大，请尝试下列方法，使分发时间趋于稳定：

.. list::

    - :ref:`使用中断分发法 <Using ESP_TIMER_ISR Callback Method>`。
    :SOC_HP_CPU_HAS_MULTIPLE_CORES: - 使用 Kconfig 选项 :ref:`CONFIG_ESP_TIMER_TASK_AFFINITY`，将 esp_timer 安装到负载较轻的 CPU 核上运行。


分发回调函数时延迟显著
^^^^^^^^^^^^^^^^^^^^^^

若分发回调函数需要相当长的时间，问题可能出在回调函数本身。更准确地说，由于所有回调函数都在单个 esp_timer 任务中逐个处理，延迟可能是由队列中较早的其他回调函数引起的。

因此，要确保应用程序中的所有回调函数都能快速独立地执行，并且没有任何阻塞操作。


唤醒后重复分发回调函数
^^^^^^^^^^^^^^^^^^^^^^

从睡眠模式中唤醒后，若回调函数重复执行，请参阅 :ref:`Handling Callbacks in Light Sleep`。


在分发回调函数时栈溢出
^^^^^^^^^^^^^^^^^^^^^^

如果在执行回调函数时遇到栈溢出的错误，请考虑减少回调函数内的栈使用量；或者，尝试通过调整 :ref:`CONFIG_ESP_TIMER_TASK_STACK_SIZE` 来增加 ESP 定时器任务栈的大小。


应用示例
--------

* :example:`system/esp_timer` 创建并启动一次性及周期性的软件定时器，展示了如何结合 Light-sleep 模式使用定时器，然后停止并删除定时器。


API 参考
--------

.. include-build-file:: inc/esp_timer.inc
