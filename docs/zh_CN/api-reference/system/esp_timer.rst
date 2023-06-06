高分辨率定时器（ESP 定时器）
=================================

:link_to_translation:`en:[English]`

{IDF_TARGET_HR_TIMER:default = "SYSTIMER", esp32 = "LAC 定时器"}

{IDF_TARGET_HR_TIMER_Resolution:default = "Not updated", esp32 = "64", esp32s2 = "64", esp32c3 = "52", esp32s3 = "52", esp32c2 = "52", esp32c6 = "52", esp32h2 = "52"}


概述
--------

尽管 FreeRTOS 提供软件定时器，但 FreeRTOS 软件定时器存在一定局限性：

- 最大分辨率等于 RTOS 滴答周期
- 定时器回调函数从低优先级的定时器服务（即守护进程）任务中分发。该任务可能会被其他任务抢占，导致精度和准确性下降。

硬件定时器虽不受上述限制，但使用不便。例如，应用组件可能需要在特定的未来时间触发计时器事件，但硬件定时器通常只有一个“compare（比较）”值用于中断生成。为提升使用的便利性，应在硬件定时器的基础上构建某种机制来管理待处理事件列表，确保在相应的硬件中断发生时调度回调。

.. only:: esp32

    配置 :ref:`CONFIG_ESP_TIMER_INTERRUPT_LEVEL` 选项，设置硬件定时器中断的优先级（可设置为 1、2 或 3 级），提高定时器中断的优先级可以减少由中断延迟引起的定时器处理延迟。


``esp_timer`` API 集支持单次定时器和周期定时器、微秒级的时间分辨率、以及 {IDF_TARGET_HR_TIMER_Resolution} 位范围。

``esp_timer`` 内部使用 {IDF_TARGET_HR_TIMER_Resolution} 位硬件定时器，具体硬件实现取决于芯片型号，如 {IDF_TARGET_NAME} 使用的是 {IDF_TARGET_HR_TIMER}。

定时器回调可通过以下两种方式调度：

- ``ESP_TIMER_TASK``。
- ``ESP_TIMER_ISR``。仅当 :ref:`CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD` 被启用时可用（默认为禁用）。

使用 ``ESP_TIMER_TASK`` 这一途径时，定时器回调函数是从高优先级的 ``esp_timer`` 任务中调度的。由于所有回调函数都是从同一个任务中调度，因此建议在回调函数本身中仅执行最小化的工作，如使用队列向低优先级任务发布事件。

如果有优先级高于 ``esp_timer`` 的其他任务正在运行，则回调调度将延迟，直至 ``esp_timer`` 能够运行。例如，执行 SPI flash 操作时便会发生此类情况。

使用 ``ESP_TIMER_ISR`` 这一途径时，定时器回调由定时器中断处理程序直接调度。对旨在降低延迟的简单回调，建议使用此途径。

创建、启动定时器并调度回调需要一些时间。因此，单次 ``esp_timer`` 的超时值存在最小限制。若调用 :cpp:func:`esp_timer_start_once` 时设置的超时值小于 20 us，回调函数仍会在大约 20 微秒后被调度。

周期 ``esp_timer`` 将最小周期限制为 50 us。周期小于 50 us 的软件定时器会占用大部分 CPU 时间，因此它们并不实用。如需使用小周期定时器，请考虑使用专用硬件外设或 DMA 功能。

使用 ``esp_timer`` API
------------------------

单个定时器由 :cpp:type:`esp_timer_handle_t` 类型表示。每个定时器都有与之关联的回调函数，定时器超时时便会从 ``esp_timer`` 任务中调用此函数。

- 要创建定时器，请调用函数 :cpp:func:`esp_timer_create`。
- 要删除定时器，请调用函数 :cpp:func:`esp_timer_delete`。

定时器可以以单次模式和周期模式启动。

- 要以单次模式启动定时器，请调用函数 :cpp:func:`esp_timer_start_once`，传递应在多久后调用回调的时间间隔。调用回调时，定时器被视为停止工作。

- 要以周期模式启动定时器，请调用函数 :cpp:func:`esp_timer_start_periodic`，传递应调用回调的周期。计时器持续运行，直到调用函数 :cpp:func:`esp_timer_stop`。

注意，当调用函数 :cpp:func:`esp_timer_start_once` 或 :cpp:func:`esp_timer_start_periodic` 时，应确保定时器处于非运行状态。因此，要重启运行中的定时器，需首先调用函数 :cpp:func:`esp_timer_stop` 停止定时器，然后调用 :cpp:func:`esp_timer_start_once` 或 :cpp:func:`esp_timer_start_periodic` 来启动定时器。

回调函数
------------------

.. note:: 回调函数应尽可能简略，避免影响所有定时器。

若定时器回调由 ``ESP_TIMER_ISR`` 方式处理，则该回调不应调用切换上下文的 ``portYIELD_FROM_ISR()``，而应调用函数 :cpp:func:`esp_timer_isr_dispatch_need_yield`。如果系统有此需求，上下文切换将在所有 ISR 调度定时器处理完毕后进行。

.. only:: SOC_ETM_SUPPORTED and SOC_SYSTIMER_SUPPORT_ETM

    ETM 事件
    ---------

    ``esp_timer`` 的构建基于 *systimer* 硬件定时器，能够产生报警事件并与 :doc:`ETM </api-reference/peripherals/etm>` 模块交互。调用函数 :cpp:func:`esp_timer_new_etm_alarm_event` 以获取相应的 ETM 事件句柄。

    如需了解如何将 ETM 事件连接到相应 ETM 通道，请参阅 :doc:`ETM </api-reference/peripherals/etm>`。

Light-sleep 模式下的 ``esp_timer``
-----------------------------------

在 Light-sleep 期间， ``esp_timer`` 计数器停止工作，并且不调用回调函数，而是由 RTC 计数器负责计算时间。唤醒后，系统得到两个计数器间的差值，并调用函数推进 ``esp_timer`` 计数器计数。计数器计数被推进后，系统开始调用 Light-sleep 期间未被调用的回调。回调数量取决于 Light-sleep 模式持续时长和定时器周期，这可能会导致某些队列溢出。以上情况仅适用于周期性定时器，单次定时器只会被调用一次。

通过在 Light-sleep 模式前调用函数 :cpp:func:`esp_timer_stop` 可以改变上述行为。但在某些情况下这可能并不方便。比起使用停止函数，在 :cpp:func:`esp_timer_create` 中使用 ``skip_unhandled_events`` 选项将更加便利。 当 ``skip_unhandled_events`` 为真时，如果一个周期性定时器在 Light-sleep 期间超时一次或多次，那么在唤醒时只有一个回调会被调用。

使用带有自动 Light-sleep 的 ``skip_unhandled_events`` 选项（请参阅 :doc:`电源管理 <power_management>`），有助于在系统处于 Light-sleep 状态时降低功耗。 Light-sleep 的持续时间也在一定程度上由下一个事件发生的时间确定。具有 ``skip_unhandled_events``` 选项的定时器不会唤醒系统。

处理回调
------------------

设计 ``esp_timer`` 是为了使定时器实现高分辨率和低延迟，并具备处理延迟事件的能力。如果定时器延迟，回调将被尽快调用，不会丢失。在最糟的情况下，周期性定时器可能超过一个周期还没有被处理，此时回调将被陆续调用，而不会等待设定的周期。这会给某些应用带来负面影响，为避免此类情况发生，特引入 ``skip_unhandled_events`` 选项。设置该选项后，即使一个周期性定时器多次超时且无法调用回调，该定时器在恢复处理能力后，仍将只产生一个回调事件。

获取当前时间
----------------------

``esp_timer`` 还提供了一个便捷函数 :cpp:func:`esp_timer_get_time` 以获取自启动以来经过的时间，可精确到微秒。这个函数通常会在 ``app_main`` 函数即将被调用前，返回自 ``esp_timer`` 启动以来的微秒数。

不同于 ``gettimeofday`` 函数，:cpp:func:`esp_timer_get_time` 返回的值：

- 芯片从 Deep-sleep 中唤醒后，从零开始
- 没有应用时区或 DST 调整

应用示例
-------------------

``esp_timer`` API 的详细用法可参阅 :example:`system/esp_timer`。


API 参考
-------------

.. include-build-file:: inc/esp_timer.inc


