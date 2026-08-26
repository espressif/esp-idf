===========================
MCPWM 同步：对齐 PWM 相位
===========================

.. contents::
    :local:
    :depth: 2

为什么需要同步
==============

每个 MCPWM 定时器是独立的硬件计数器。当你调用 :cpp:func:`mcpwm_timer_start()` 启动两个定时器时，CPU 会依次发出两条写指令——第二个定时器比第一个晚几十个 CPU 周期才启动。即使两者的周期配置完全相同，它们的计数器在周期中的位置也是不同的，PWM 输出的相位关系无法预测。

同步通过在同步边沿到达时给**正在运行**的定时器加载指定的计数值和方向来解决这个问题。定时器必须已经在运行；同步不会启动或停止定时器。它是在运行时修正相位的一种机制。

如果同步边沿在每个周期都到达（例如来自 TEZ 处的定时器同步源），那么修正每周期重复一次，相位关系可以无限期保持。这就是典型的用法：一个定时器作为参考，其他定时器在每个周期都重新对齐到它。

MCPWM 提供三种同步源。所有源都产生 :cpp:type:`mcpwm_sync_handle_t` 类型的句柄，且任何源都可以供给同组内的任意定时器。

GPIO 同步源
============

GPIO 同步源对外部引脚上的边沿做出反应——当外部控制器、传感器或编码器提供周期性参考信号时非常有用。

.. code-block:: c

    mcpwm_sync_handle_t sync = NULL;
    ESP_ERROR_CHECK(mcpwm_new_gpio_sync_src(
        &(mcpwm_gpio_sync_src_config_t) {
            .group_id = 0,
            .gpio_num = 5,
            .flags.active_neg = false,
        }, &sync));

GPIO 同步源配置很简单：

.. list::

    - :cpp:member:`group_id <mcpwm_gpio_sync_src_config_t::group_id>` — 同步源所属的 MCPWM 组。必须与所有接收该同步的定时器所在组一致。
    - :cpp:member:`gpio_num <mcpwm_gpio_sync_src_config_t::gpio_num>` — 承载同步信号的 GPIO。
    - :cpp:member:`active_neg <mcpwm_gpio_sync_src_config_t::flags::active_neg>` — 默认上升沿为有效边沿；设置后改为下降沿有效。

软件同步源
==========

软件同步源由应用代码按需产生同步边沿。它没有配置字段；创建后即可在需要时激活。

.. code-block:: c

    mcpwm_sync_handle_t soft_sync = NULL;
    ESP_ERROR_CHECK(mcpwm_new_soft_sync_src(NULL, &soft_sync));

    // 后续当应用决定同步时：
    ESP_ERROR_CHECK(mcpwm_soft_sync_activate(soft_sync));

.. note::

    必须先通过 :cpp:func:`mcpwm_timer_set_phase_on_sync()` 或 :cpp:func:`mcpwm_capture_timer_set_phase_on_sync()` 将软件同步源绑定到定时器，再调用激活。驱动在创建时不会分配定时器；在绑定前调用 :cpp:func:`mcpwm_soft_sync_activate()` 属于未定义行为。

这在定时器已经在运行、应用需要触发一次性的相位修正时有用——例如故障恢复后，或开始新的控制周期之前。由于软件同步是一次性的，如果后续没有更多同步边沿到来，相位关系会随时间漂移。如需持续锁相，应使用周期性源（GPIO 或定时器同步源）。

定时器同步源
============

定时器同步源在定时器到达指定事件时产生同步边沿——例如每次定时器计到零（TEZ）。这可以让一个定时器作为其他定时器的周期性参考，每周期都保持相位锁定。

.. code-block:: c

    mcpwm_sync_handle_t timer_sync = NULL;
    ESP_ERROR_CHECK(mcpwm_new_timer_sync_src(
        timer_a,
        &(mcpwm_timer_sync_src_config_t) {
            .timer_event = MCPWM_TIMER_EVENT_EMPTY,
        },
        &timer_sync));

.. list::

    - :cpp:member:`timer_event <mcpwm_timer_sync_src_config_t::timer_event>` — 触发同步输出的定时器事件。常用 :cpp:enumerator:`MCPWM_TIMER_EVENT_EMPTY` （零）表示每个周期开始，或 :cpp:enumerator:`MCPWM_TIMER_EVENT_PEAK` 表示峰值位置。在向上计数模式中峰值就是周期边界；在向上-向下计数模式中峰值是周期的中点。
    - :cpp:member:`propagate_input_sync <mcpwm_timer_sync_src_config_t::flags::propagate_input_sync>` — 设置后，该定时器会将其接收到的输入同步转发到其输出，无需额外 GPIO 接线即可实现定时器同步链。此模式下硬件选择输入同步作为输出源，因此 :cpp:member:`timer_event` 字段会被忽略。

每个定时器最多只能创建一个同步源。多个定时器可以接收同一个同步源。

由于定时器同步源每周期都会触发，接收定时器在每个周期都会得到修正。这是维持多路 PWM 通道间稳定相位关系最常用的方式。

设置接收相位
============

无论选择哪种同步源，接收定时器都使用相同的 API。调用 :cpp:func:`mcpwm_timer_set_phase_on_sync()` 配置同步边沿到达时的行为。定时器必须已经在运行，同步才会生效。

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_timer_set_phase_on_sync(timer,
        &(mcpwm_timer_sync_phase_config_t) {
            .sync_src = sync,
            .count_value = 25,
            .direction = MCPWM_TIMER_DIRECTION_UP,
        }));

.. list::

    - :cpp:member:`sync_src <mcpwm_timer_sync_phase_config_t::sync_src>` — 源对象。设为 ``NULL`` 可取消同步。
    - :cpp:member:`count_value <mcpwm_timer_sync_phase_config_t::count_value>` — 同步事件到达时加载的计数值。应保持在定时器周期范围内。
    - :cpp:member:`direction <mcpwm_timer_sync_phase_config_t::direction>` — 加载后的计数方向。

两路 90 度移相
==============

现在你已经了解了三种同步源以及如何设置接收相位，下面是一个完整示例。它使用定时器同步源：``timer_a`` 每次到达零时发出同步，``timer_b`` 收到后加载 ``count_value = 25``，产生 90 度相位滞后。由于同步每周期重复一次，两路输出的相位关系可以无限期保持。

.. code-block:: c

    mcpwm_timer_handle_t timer_a = NULL;
    mcpwm_timer_handle_t timer_b = NULL;
    mcpwm_sync_handle_t timer_a_sync = NULL;

    // timer_a 和 timer_b 均已创建，period_ticks = 100

    ESP_ERROR_CHECK(mcpwm_new_timer_sync_src(
        timer_a,
        &(mcpwm_timer_sync_src_config_t) {
            .timer_event = MCPWM_TIMER_EVENT_EMPTY,
        },
        &timer_a_sync));

    ESP_ERROR_CHECK(mcpwm_timer_set_phase_on_sync(timer_b,
        &(mcpwm_timer_sync_phase_config_t) {
            .sync_src = timer_a_sync,
            .count_value = 25,
            .direction = MCPWM_TIMER_DIRECTION_UP,
        }));

    // timer_a 在 TEZ 输出同步；timer_b 收到后从 Tick 25 开始继续计数。

理解领先与滞后
--------------

下面的示意中，``PWM_A`` 先开始一个周期，``PWM_B`` 在其后四分之一周期出现，因此 ``PWM_B`` 滞后 ``PWM_A`` 90 度；反过来说，``PWM_A`` 领先 ``PWM_B`` 90 度。

.. figure:: /../_static/mcpwm/phase_shift.svg
    :align: center
    :alt: PWM 相移 90 度滞后

    PWM_A 与 PWM_B 之间的 90 度相移：PWM_B 在 PWM_A 之后 25 个 tick 处开始上升。

其他注意事项
============

捕获定时器也可通过 :cpp:func:`mcpwm_capture_timer_set_phase_on_sync()` 使用同一同步源，捕获始终向上计数。接收端和源必须保留在同一组中。删除源之前，应先取消同步或删除所有使用它的对象。

API 参考
========

MCPWM 同步驱动函数
------------------

.. include-build-file:: inc/mcpwm_sync.inc
