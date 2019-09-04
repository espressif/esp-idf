定时器
============

:link_to_translation:`en:[English]`

简介
------------

ESP32 芯片提供两组硬件定时器，每组包含两个通用硬件定时器。所有定时器均为 64 位通用定时器，包括 16 位预分频器和 64 位自动重载向上/向下计数器。


功能概述
-------------------

下文介绍了配置和操作定时器的常规步骤：

* :ref:`timer-api-timer-initialization` - 启动定时器前应设置的参数，以及每个设置提供的具体功能。
* :ref:`timer-api-timer-control` - 如何读取定时器的值，如何暂停/启动定时器以及如何改变定时器的操作方式。
* :ref:`timer-api-alarms` - 如何设置和使用警报。
* :ref:`timer-api-interrupts`- 如何使能和使用中断。


.. _timer-api-timer-initialization:

定时器初始化
^^^^^^^^^^^^^^^^^^^^

两个 ESP32 定时器组中，每组都有两个定时器，两组共有四个定时器供使用。ESP32 定时器组应使用 :cpp:type:`timer_group_t` 识别，而每组中的个体定时器则应使用 :cpp:type:`timer_idx_t` 识别。

首先调用 :cpp:func:`timer_init` 函数，并将 :cpp:type:`timer_config_t` 结构体传递给此函数，用于定义定时器的工作方式，实现定时器初始化。特别注意以下定时器参数可设置为：

    * **分频器**: 设置定时器中计数器计数的速度，:cpp:member:`divider` 的设置将用作输入的 80 MHz APB_CLK 时钟的除数。
    * **模式**: 设置计数器是递增还是递减。可通过从 :cpp:type:`timer_count_dir_t` 中选取一个值，后使用 :cpp:member:`counter_dir` 来选择模式。
    * **计数器使能**: 如果计数器已使能，则在调用 :cpp:func:`timer_init` 后计数器将立即开始递增/递减。您可通过从 :cpp:type:`timer_start_t` 中选取一个值，后使用 :cpp:member:`counter_en` 改变此行为。
    * **报警使能**: 可使用 :cpp:member:`alarm_en` 设置。
    * **自动重载**: 设置计数器是否应该在定时器警报上使用 :cpp:member:`auto_reload` 自动重载首个计数值，还是继续递增或递减。
    * **中断类型**: 选择定时器警报上应触发的中断类型，请设置 :cpp:type:`timer_intr_mode_t` 中定义的值。

要获取定时器设置的当前值，请使用函数 :cpp:func:`timer_get_config`。


.. _timer-api-timer-control:

定时器控制
^^^^^^^^^^^^^

定时器使能后便开始计数。要使能定时器，可首先设置 :cpp:member:`counter_en` 为 ``true``，然后调用函数 :cpp:func:`timer_init`，或者直接调用函数 :cpp:func:`timer_start`。您可通过调用函数 :cpp:func:`timer_set_counter_value` 来指定定时器的首个计数值。要检查定时器的当前值，调用函数 :cpp:func:`timer_get_counter_value` 或 :cpp:func:`timer_get_counter_time_sec`。

可通过调用函数 :cpp:func:`timer_pause` 随时暂停定时器。要再次启动它，调用函数 :cpp:func:`timer_start`。

要重新配置定时器，可调用函数 :cpp:func:`timer_init`，该函数详细介绍见 :ref:`timer-api-timer-initialization`。

除此之外，还可通过使用专有函数更改个别设置来重新配置定时器：

=============  ===================================  ==========================================================================
设置             专有函数                             描述
=============  ===================================  ==========================================================================
 分频器        :cpp:func:`timer_set_divider`        更改计数频率。为避免发生不可预测情况，更改分频器时应暂停定时器。如果定时器正在运行，则使用 :cpp:func:`timer_set_divider` 将其暂停并更改设置，然后重启定时器。
模式           :cpp:func:`timer_set_counter_mode`   设置计数器应递增还是递减
自动重载       :cpp:func:`timer_set_auto_reload`    设置是否应在定时器警报上重载首个计数值
=============  ===================================  ==========================================================================

.. _timer-api-alarms:

警报
^^^^^^

要设置警报，先调用函数 :cpp:func:`timer_set_alarm_value`，然后使用 :cpp:func:`timer_set_alarm` 使能警报。当调用函数 :cpp:func:`timer_init` 时，也可以在定时器初始化阶段使能警报。

警报已使能且定时器达到警报值后，根据配置，可能会出现以下两种行为：

    * 如果先前已配置，此时将触发中断。有关如何配置中断，请参见 :ref:`timer-api-interrupts`。
    * 如 :cpp:member:`auto_reload` 已使能，定时器的计数器将重新加载，从先前配置好的值开始再次计数。应使用函数 :cpp:func:`timer_set_counter_value` 预先设置该值。

.. note::

    * 如果已设置警报值且定时器已超过该值，则将立即触发警报。
    * 一旦触发后，警报将自动关闭，需要重新使能以再次触发。

要检查某特定的警报值，调用函数 :cpp:func:`timer_get_alarm_value`。


.. _timer-api-interrupts:

中断
^^^^^^^^^^

可通过调用函数 :cpp:func:`timer_isr_register` 为特定定时器组和定时器注册中断处理程序。

调用 :cpp:func:`timer_group_intr_enable` 使能定时器组的中断程序，调用 :cpp:func:`timer_enable_intr` 使能某定时器的中断程序。调用 :cpp:func:`timer_group_intr_disable` 关闭定时器组的中断程序，调用 :cpp:func:`timer_disable_intr` 关闭某定时器的中断程序。

在中断服务程序（ISR）中处理中断时，需要明确地清除中断状态位。为此，请设置定义在 :component_file:`soc/esp32/include/soc/timer_group_struct.h` 中的 ``TIMERGN.int_clr_timers.tM`` 结构。该结构中 ``N`` 是定时器组别编号 [0, 1]，``M`` 是定时器编号 [0, 1]。例如，要清除定时器组别 0 中定时器 1 的中断状态位，请调用以下命令:: 

    TIMERG0.int_clr_timers.t1 = 1

有关如何使用中断，请参阅应用示例。


应用示例
-------------------

64 位硬件定时器示例：:example:`peripherals/timer_group`。

API 参考
-------------

.. include:: /_build/inc/timer.inc
