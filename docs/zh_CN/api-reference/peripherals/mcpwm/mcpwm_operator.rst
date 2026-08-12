=========================
MCPWM 操作器：组装输出级
=========================

.. contents::
    :local:
    :depth: 2

操作器位于定时器与生成器之间，管理比较器、生成器动作、制动、死区路由和载波调制。一个定时器可驱动同组的多个操作器，而一个操作器只连接一个定时器。

如果把定时器看成时钟源，操作器就是一套输出级的资源容器。它让多个输出共享同一时间基准，同时把保护、死区和载波这类功率级特性收拢到一个地方管理。

连接基本模块
============

操作器必须与定时器在同一组。先通过 :cpp:func:`mcpwm_operator_connect_timer()` 创建并连接操作器；连接前，生成器不能使用定时器事件。

.. code-block:: c

    mcpwm_oper_handle_t oper = NULL;
    mcpwm_operator_config_t oper_config = {
        .group_id = 0,
        .flags.update_gen_action_on_tez = true,
        .flags.update_dead_time_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&oper_config, &oper));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));

操作器配置很小，但有两个字段没有出现在示例中：

.. list::

    - :cpp:member:`group_id <mcpwm_operator_config_t::group_id>` — 操作器从哪个 MCPWM 组分配。它必须与定时器所在组一致，因为操作器只能连接同组内的定时器。
    - :cpp:member:`intr_priority <mcpwm_operator_config_t::intr_priority>` — 制动事件回调使用的中断优先级。不设置（``0``）时由驱动选择较低优先级；当制动通知需要抢占其他中断时可适当提高。

``flags`` 用于选择新的生成器动作和死区配置在何时生效。默认全部关闭，改动会立即生效——可能在 PWM 周期中间：

.. list::

    - :cpp:member:`update_gen_action_on_tez <mcpwm_operator_config_t::flags::update_gen_action_on_tez>`、:cpp:member:`update_gen_action_on_tep <mcpwm_operator_config_t::flags::update_gen_action_on_tep>` 和 :cpp:member:`update_gen_action_on_sync <mcpwm_operator_config_t::flags::update_gen_action_on_sync>` — 将生成器动作的改动缓冲到计数归零、峰值或同步事件时。
    - :cpp:member:`update_dead_time_on_tez <mcpwm_operator_config_t::flags::update_dead_time_on_tez>`、:cpp:member:`update_dead_time_on_tep <mcpwm_operator_config_t::flags::update_dead_time_on_tep>` 和 :cpp:member:`update_dead_time_on_sync <mcpwm_operator_config_t::flags::update_dead_time_on_sync>` — 死区改动同样缓冲，更新点规则见 :ref:`死区 <mcpwm-dead-time>`。

对于运行中的功率级，应使用归零（``tez``）、峰值（``tep``）或同步点更新，以避免部分周期。

一个定时器，多个操作器
======================

同一定时器可驱动多个操作器，每个操作器产生不同的波形。这适用于多相逆变器或多个同频但独立占空比的电机。

反过来，一个操作器只能连接一个定时器，因此该操作器下的所有比较器和生成器天然共享同一时间基准。这正是同相、互补或成对输出容易实现的原因。

.. code-block:: c

    mcpwm_oper_handle_t oper_b = NULL;
    mcpwm_operator_config_t oper_config_b = {
        .group_id = 0,
        .flags.update_gen_action_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&oper_config_b, &oper_b));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper_b, timer));
    // 在 oper_b 下创建独立的比较器和生成器。

.. _mcpwm-brake:

制动与安全输出
==============

操作器将 :doc:`故障 <mcpwm_fault>` 转换为制动动作。应在启动 PWM 前配置生成器的制动状态，使故障路径完全由硬件驱动，避免软件延迟。

恢复策略
--------

.. list::

    - **CBC（逐周期）：** 故障有效期间制动，并在配置的定时器归零或峰值处恢复，适合瞬态限流。
    - **OST（单次）：** 故障消失后仍保持制动，必须由软件显式恢复，适合互锁或严重过流。

对于 CBC，设置 :cpp:member:`cbc_recover_on_tez <mcpwm_brake_config_t::flags::cbc_recover_on_tez>` 或 :cpp:member:`cbc_recover_on_tep <mcpwm_brake_config_t::flags::cbc_recover_on_tep>`，选择已清除故障后释放输出的边界。在边界恢复可避免在 PWM 周期中间重新导通开关。

.. warning::

    不要同时启用 ``cbc_recover_on_tez`` 和 ``cbc_recover_on_tep``；应选择与波形和栅极驱动时序相匹配的边界。

.. _mcpwm-brake-fault-connection:

故障连接
--------

将故障连接到操作器，然后为每个生成器指定制动模式下的状态。此例在两个计数方向将原始生成器置低，使用 OST 制动：

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_operator_set_brake_on_fault(oper,
        &(mcpwm_brake_config_t) {
            .fault = fault,
            .brake_mode = MCPWM_OPER_BRAKE_MODE_OST,
        }));

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_brake_event(
        generator, MCPWM_GEN_BRAKE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_OST,
            MCPWM_GEN_ACTION_LOW)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_brake_event(
        generator, MCPWM_GEN_BRAKE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_DOWN, MCPWM_OPER_BRAKE_MODE_OST,
            MCPWM_GEN_ACTION_LOW)));

半桥必须为两个生成器都配置相同的制动动作。逻辑低电平可能被死区、GPIO 矩阵或外部电路反相，因此应在栅极驱动器处确认实际安全状态。

生成器级故障动作与操作器制动的区别在于：前者适合单个输出的即时边沿响应，后者负责整个输出级的安全状态、锁存行为和恢复策略。主保护路径通常应优先使用操作器制动。

OST 故障恢复
------------

CBC 在故障消失后在配置的边界自动恢复。OST 恢复前，先移除并确认故障根因已经消失，然后调用：

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_operator_recover_from_fault(oper, fault));

故障仍有效时该调用会失败。

.. figure:: /../_static/mcpwm/brake_cbc_ost.svg
    :align: center
    :alt: PWM 运行期间故障被触发。CBC 在故障有效期间将输出保持在制动电平，并在下一个周期边界恢复；OST 则保持锁存直至软件恢复。

    CBC 仅在故障有效期间制动，并在下一个周期边界恢复；OST 保持锁存直至软件恢复。

制动事件回调
------------

操作器可通过 :cpp:member:`on_brake_cbc <mcpwm_operator_event_callbacks_t::on_brake_cbc>` 和 :cpp:member:`on_brake_ost <mcpwm_operator_event_callbacks_t::on_brake_ost>` 回调报告制动事件。二者均在 ISR 上下文运行，只应用于通知，不应用于阻塞式恢复。

.. code-block:: c

    mcpwm_operator_event_callbacks_t cbs = {
        .on_brake_cbc = my_brake_cbc_cb,
        .on_brake_ost = my_brake_ost_cb,
    };
    ESP_ERROR_CHECK(mcpwm_operator_register_event_callbacks(oper, &cbs, NULL));

.. _mcpwm-carrier:

载波调制
========

载波调制在操作器的 PWM 输出上叠加高频载波，常用于变压器隔离式栅极驱动：即使基础 PWM 保持 100% 占空比，调制后仍有可穿过隔离栅的跳变。应先完成基础 PWM 的配置；载波设置影响此操作器的所有生成器。

载波配置
--------

.. code-block:: c

    mcpwm_carrier_config_t carrier = {
        .clk_src = MCPWM_CARRIER_CLK_SRC_DEFAULT,
        .frequency_hz = 100000,
        .duty_cycle = 0.5f,
        .first_pulse_duration_us = 20,
    };
    ESP_ERROR_CHECK(mcpwm_operator_apply_carrier(oper, &carrier));

.. figure:: /../_static/mcpwm/carrier_modulation.svg
    :align: center
    :alt: 50% 占空比基础 PWM 的载波调制

    100 kHz 载波与 50% 占空比基础 PWM 做与操作。首个脉冲被拉伸到 20 us（两个载波周期）；基础 PWM 为低电平时不做斩波。

载波参数
--------

.. list::

    - :cpp:member:`clk_src <mcpwm_carrier_config_t::clk_src>` 选择载波时钟源。默认指向一个内部 PLL 时钟（如 PLL_F160M），部分芯片还提供 RC_FAST 或 XTAL 作为备选。不同时钟源的分辨率与功耗不同；默认时钟源即可满足大多数应用，仅在需要避开某个时钟引入的噪声、PLL 精度不够，或对功耗有要求时，才需要手动指定。
    - :cpp:member:`frequency_hz <mcpwm_carrier_config_t::frequency_hz>` 是载波频率；应选择同时满足隔离变压器、栅极驱动器、开关损耗预算和目标芯片时钟分辨率的值。
    - :cpp:member:`duty_cycle <mcpwm_carrier_config_t::duty_cycle>` 只能取硬件支持的 0.125、0.25、0.375、0.5、0.625、0.75 或 0.875，而非任意比例。
    - :cpp:member:`first_pulse_duration_us <mcpwm_carrier_config_t::first_pulse_duration_us>` 控制调制开始后的首个脉冲。它必须非零，且至少为一个载波周期。较长的首脉冲可帮助在感性隔离路径中建立电流，但必须保持在栅极驱动系统允许范围内。
    - :cpp:member:`invert_before_modulate <mcpwm_carrier_config_t::flags::invert_before_modulate>` 改变原始 PWM 极性，:cpp:member:`invert_after_modulate <mcpwm_carrier_config_t::flags::invert_after_modulate>` 改变调制后极性。

不需要载波调制时，向 :cpp:func:`mcpwm_operator_apply_carrier` 传入 ``NULL`` 配置即可禁用。

API 参考
========

MCPWM 操作器驱动函数
--------------------

.. include-build-file:: inc/mcpwm_oper.inc
