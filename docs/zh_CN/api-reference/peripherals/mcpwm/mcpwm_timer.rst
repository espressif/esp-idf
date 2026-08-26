=======================
MCPWM 定时器：设定频率
=======================

.. contents::
    :local:
    :depth: 2

定时器为与其操作器相连的每一路 PWM 提供时间基准。它以 :cpp:member:`resolution_hz <mcpwm_timer_config_t::resolution_hz>` 的频率计数，并在达到 :cpp:member:`period_ticks <mcpwm_timer_config_t::period_ticks>` 时回绕。先选择分辨率——它决定了边沿放置的最小步长——再选择目标频率对应的周期。

无论是舵机、调速还是逆变器，定时器都回答两个最基本的问题："一个 Tick 有多细？" 和 "一个 PWM 周期有多长？"。后续比较器和生成器都只是在这个时间基准上放置边沿。

构建 20 kHz 时间基准
=====================

对于向上计数模式， ``period_ticks = resolution_hz / frequency_hz`` 。以下配置的 Tick 为 1 MHz（每 Tick 1 微秒），周期为 50 Tick，频率即为 20 kHz。下图展示了计数器从 0 上升到 50 然后复位的过程 —— ``TEZ`` （定时器归零事件）和 ``TEP`` （定时器峰值事件）是生成器使用的两个边界。

.. figure:: /../_static/mcpwm/timer_up_count.svg
    :align: center
    :alt: 向上计数：计数器形成锯齿波，从 0 上升到 50，TEZ 在归零时触发，TEP 在峰值时触发。

    向上计数：计数器形成锯齿波，从 0 上升到 50，TEZ 在归零时触发，TEP 在峰值时触发。

.. code-block:: c

    mcpwm_timer_handle_t timer = NULL;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .period_ticks = 50,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

示例代码只覆盖了最核心的配置，但 :cpp:type:`mcpwm_timer_config_t` 中还有一些未出现的字段，在特定场景下同样重要：

.. list::

    - :cpp:member:`group_id <mcpwm_timer_config_t::group_id>` — 定时器从哪个 MCPWM 组分配。不同芯片会提供不同数量的组，每个组包含一组共享时钟分频器的定时器、操作器、比较器和生成器。 ``0`` 表示第一个组，大多数设计用它就够了。
    - :cpp:member:`clk_src <mcpwm_timer_config_t::clk_src>` — 定时器的时钟源。:c:macro:`MCPWM_TIMER_CLK_SRC_DEFAULT` 选择 PLL 时钟，适合绝大多数应用。部分芯片还提供其他时钟源，可在 PLL 被关闭（例如浅睡眠）时显式指定，让定时器继续计数。
    - :cpp:member:`resolution_hz <mcpwm_timer_config_t::resolution_hz>` — 计数器的 Tick 频率。一个 Tick 持续 ``1 / resolution_hz`` 秒，1 MHz 即每 Tick 1 微秒。它决定了比较器能放置的边沿最细粒度。
    - :cpp:member:`period_ticks <mcpwm_timer_config_t::period_ticks>` — 一个完整 PWM 周期包含的 Tick 数。频率为 ``resolution_hz / period_ticks``。
    - :cpp:member:`count_mode <mcpwm_timer_config_t::count_mode>` — 计数器是只向上计数（边沿对齐 PWM），还是先上后下（中心对齐 PWM）。两种计数形态见 :ref:`计数模式与波形 <mcpwm-timer-counting-modes>`；硬件还支持向下计数。
    - :cpp:member:`intr_priority <mcpwm_timer_config_t::intr_priority>` — 定时器回调使用的中断优先级。不设置（``0``）时由驱动选择较低优先级；当回调需要抢占其他中断时（例如对时序敏感的电机控制），可适当提高。

示例没有设置 :cpp:member:`flags <mcpwm_timer_config_t::flags>`，因此这些开关都处于关闭状态——这也是安全的默认值。其中两个值得了解：

.. list::

    - :cpp:member:`update_period_on_empty <mcpwm_timer_config_t::flags::update_period_on_empty>` 和 :cpp:member:`update_period_on_sync <mcpwm_timer_config_t::flags::update_period_on_sync>` — 默认关闭，因此 :cpp:func:`mcpwm_timer_set_period()` 会立即生效。打开后可把频率切换推迟到安全边界，见 :ref:`频率更新 <mcpwm-timer-safe-update>`。
    - :cpp:member:`allow_pd <mcpwm_timer_config_t::flags::allow_pd>` — 允许在睡眠时关闭 MCPWM 电源域。驱动会在睡眠前后备份并恢复定时器寄存器，从而省电，代价是额外占用 RAM。

在完成操作器、比较器和生成器的配置后，再启用并启动定时器：

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

:cpp:func:`mcpwm_timer_enable()` 启用定时器运行所需的系统服务：使能定时器中断，并在启用电源管理时持有该组电源管理锁，避免时钟变频干扰 PWM 时序。:cpp:func:`mcpwm_timer_start_stop()` 则负责启动和停止计数。删除定时器前，先调用 :cpp:func:`mcpwm_timer_disable()` 恢复原状，再调用 :cpp:func:`mcpwm_del_timer()`。

:cpp:func:`mcpwm_timer_start_stop()` 的第三个参数选择停止行为：

.. list::

    - :c:macro:`MCPWM_TIMER_START_NO_STOP` — 持续运行，直到显式停止。
    - :c:macro:`MCPWM_TIMER_START_STOP_EMPTY` — 下次计数到零（TEZ）时自动停止。适用于单次或需要完整周期后再停止的同步启动。
    - :c:macro:`MCPWM_TIMER_START_STOP_FULL` — 下次计数到峰值（TEP）时自动停止。适用于单个完整周期后停止。

.. _mcpwm-timer-counting-modes:

计数模式与波形
==============

**向上计数** 模式下，计数器从 0 计数到 :cpp:member:`period_ticks <mcpwm_timer_config_t::period_ticks>` 然后复位。波形为锯齿波，PWM 边沿对齐在周期的一侧——这称为 *边沿对齐* PWM。

**向上-向下计数** 模式下，计数器先向上到 ``period_ticks / 2``，再向下回 0。波形为三角波，PWM 边沿围绕周期中心对称 —— *中心对齐* PWM。中心对齐 PWM 因谐波失真更小，常用于电机控制。

.. figure:: /../_static/mcpwm/timer_up_down_count.svg
    :align: center
    :alt: 向上-向下计数：计数器形成三角波，上升到 25（50 的一半），再下降回 0。

    向上-向下计数：计数器形成三角波，上升到 25（50 的一半），再下降回 0。

两种模式的频率均为 ``resolution_hz / period_ticks``。选择足够高的分辨率以满足所需的占空比精度，再选择周期以达到目标频率。

.. important::

    :cpp:member:`period_ticks <mcpwm_timer_config_t::period_ticks>` 表示完整 PWM 周期的总 Tick 数，而不是所有模式下都表示计数器峰值。

    .. list::

        - 在 ``MCPWM_TIMER_COUNT_MODE_UP`` 中，计数器范围是 ``0 -> period_ticks``。
        - 在 ``MCPWM_TIMER_COUNT_MODE_UP_DOWN`` 中，硬件峰值是 ``period_ticks / 2``，完整周期是 ``0 -> peak -> 0``。

    例如 :cpp:member:`resolution_hz <mcpwm_timer_config_t::resolution_hz>` = 1 MHz、:cpp:member:`period_ticks <mcpwm_timer_config_t::period_ticks>` = 50 时：向上计数模式为 ``0 -> 50``，向上-向下模式为 ``0 -> 25 -> 0``。两者完整周期都是 50 微秒，因此频率都为 20 kHz，只是边沿分布不同。

.. _mcpwm-timer-safe-update:

频率更新
========

默认情况下，:cpp:func:`mcpwm_timer_set_period()` 立即生效，可能截断当前周期并产生不完整的脉冲。设置 :cpp:member:`update_period_on_empty <mcpwm_timer_config_t::flags::update_period_on_empty>` 可在计数归零时更新，设置 :cpp:member:`update_period_on_sync <mcpwm_timer_config_t::flags::update_period_on_sync>` 则在同步事件时更新。若需保持占空比不变，应同时按比例更新比较值：

.. code-block:: c

    // 周期由 50 改为 100，同时保持 40% 占空比
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, 40));
    ESP_ERROR_CHECK(mcpwm_timer_set_period(timer, 100));

对大多数实时调参场景，优先修改比较器以改变占空比，只有在确实需要改 PWM 频率时才修改定时器周期。电机和电源应用通常还应配合 :cpp:member:`update_period_on_empty <mcpwm_timer_config_t::flags::update_period_on_empty>` 或同步更新，避免在周期中间切换参数。

定时器事件回调
==============

定时器可在达到峰值（:cpp:member:`on_full <mcpwm_timer_event_callbacks_t::on_full>`）、归零（:cpp:member:`on_empty <mcpwm_timer_event_callbacks_t::on_empty>`）或停止（:cpp:member:`on_stop <mcpwm_timer_event_callbacks_t::on_stop>`）时通知应用。必须在启用定时器前注册回调。回调在 ISR 上下文执行，不能阻塞、分配内存或调用普通 FreeRTOS API，应使用 ``...FromISR`` 变体。

.. note::

    定时器和捕获定时器可能与同组其他对象共享分频器。若同一组里需要多种分辨率，请按请求分辨率单调顺序创建对象，避免分频冲突。详细规则见 :doc:`高级主题 <mcpwm_advanced>`。

.. code-block:: c

    static bool IRAM_ATTR on_timer_empty(mcpwm_timer_handle_t timer,
                                         const mcpwm_timer_event_data_t *edata,
                                         void *user_ctx)
    {
        BaseType_t high_task_woken = pdFALSE;
        vTaskNotifyGiveFromISR((TaskHandle_t)user_ctx, &high_task_woken);
        return high_task_woken == pdTRUE;
    }

    mcpwm_timer_event_callbacks_t cbs = { .on_empty = on_timer_empty };
    ESP_ERROR_CHECK(mcpwm_timer_register_event_callbacks(timer, &cbs,
                                                          xTaskGetCurrentTaskHandle()));

如何在同步边沿将定时器加载到指定相位，见 :doc:`同步 <mcpwm_sync>`。

API 参考
========

MCPWM 定时器驱动函数
--------------------

.. include-build-file:: inc/mcpwm_timer.inc
