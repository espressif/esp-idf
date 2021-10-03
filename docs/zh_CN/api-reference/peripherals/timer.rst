通用定时器
==========

:link_to_translation:`en:[English]`

{IDF_TARGET_TIMER_COUNTER_BIT_WIDTH:default="54", esp32="64", esp32s2="64", esp32c3="54"}

简介
----

.. only:: not esp32c3

    {IDF_TARGET_NAME} 芯片提供两组硬件定时器，每组包含两个通用硬件定时器。


.. only:: esp32c3

    {IDF_TARGET_NAME} 芯片提供两组硬件定时器，每组包含一个通用硬件定时器和一个主系统看门狗定时器。

所有通用定时器均基于 16 位预分频器和 {IDF_TARGET_TIMER_COUNTER_BIT_WIDTH} 位可自动重新加载向上/向下计数器。


功能概述
--------

下文介绍了配置和操作定时器的常规步骤：

* :ref:`timer-api-timer-initialization` - 启动定时器前应设置的参数，以及每个设置提供的具体功能。
* :ref:`timer-api-timer-control` - 如何读取定时器的值，如何暂停/启动定时器以及如何改变定时器的操作方式。
* :ref:`timer-api-alarms` - 如何设置和使用警报。
* :ref:`timer-api-interrupts`- 如何使用中断提供的回调函数。


.. _timer-api-timer-initialization:

定时器初始化
^^^^^^^^^^^^

两个 {IDF_TARGET_NAME} 定时器组中，每组都有两个定时器，两组共有四个定时器供使用。{IDF_TARGET_NAME} 定时器组的类型为 :cpp:type:`timer_group_t`，每组中的个体定时器类型为 :cpp:type:`timer_idx_t`。

首先调用 :cpp:func:`timer_init` 函数，并将 :cpp:type:`timer_config_t` 结构体传递给此函数，用于定义定时器的工作方式，实现定时器初始化。特别注意以下定时器参数可设置为：

.. list::

    :not esp32: - **时钟源**: 选择时钟源，与时钟分频器一起决定了定时器的分辨率。
    - **分频器**: 设置定时器中计数器计数的速度，:cpp:member:`divider` 的设置将用作输入时钟源的除数。默认的时钟源是 APB_CLK (一般是 80 MHz)。更多有关 APB_CLK 时钟频率信息，请查看 *{IDF_TARGET_NAME} 技术参考手册* > *复位和时钟* [`PDF <{IDF_TARGET_TRM_CN_URL}#resclk>`__] 章节。
    - **模式**: 设置计数器是递增还是递减。可通过从 :cpp:type:`timer_count_dir_t` 中选取一个值，后使用 :cpp:member:`counter_dir` 来选择模式。
    - **计数器使能**: 如果计数器已使能，则在调用 :cpp:func:`timer_init` 后计数器将立即开始递增/递减。您可通过从 :cpp:type:`timer_start_t` 中选取一个值，后使用 :cpp:member:`counter_en` 改变此行为。
    - **报警使能**: 可使用 :cpp:member:`alarm_en` 设置。
    - **自动重载**: 设置计数器是否应该在定时器警报上使用 :cpp:member:`auto_reload` 自动重载首个计数值，还是继续递增或递减。

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

处理中断事务
^^^^^^^^^^^^

调用 :cpp:func:`timer_isr_callback_add` 函数可以给某个定时器注册一个中断回调函数，顾名思义，该函数会在中断上下文中被执行，因此用户不能在回调函数中调用任何会阻塞 CPU 的 API。
相较于从头编写中断处理程序，使用中断回调函数的好处是，用户无需检测和处理中断的状态位，这些操作会由驱动中默认的中断处理程序替我们完成。

有关如何使用中断回调函数，请参考如下应用示例。


应用示例
--------

{IDF_TARGET_TIMER_COUNTER_BIT_WIDTH} 位通用硬件定时器示例：:example:`peripherals/timer_group`。


API 参考
--------

.. include-build-file:: inc/timer.inc
.. include-build-file:: inc/timer_types.inc
