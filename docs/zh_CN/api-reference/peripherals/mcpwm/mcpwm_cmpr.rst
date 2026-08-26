=================================
MCPWM 比较器：将比例转换为边沿
=================================

.. contents::
    :local:
    :depth: 2

当定时器计数达到 ``cmp_ticks`` 时，比较器产生事件；生成器再将该事件转换为 GPIO 电平变化。通常在向上计数 PWM 中，比较 Tick 数就是高电平时间。

比较器的实际意义是把"我想要多宽、多晚、多早出现一个边沿"转换成硬件里的具体时刻。运行中的占空比调节，通常就是不断更新比较器阈值。

设置 30% 占空比
===============

从已有操作器创建比较器，再设置阈值。对 :doc:`定时器 <mcpwm_timer>` 中 50 Tick 的周期，15 Tick 即为 30%。下图展示了比较事件在 Tick 15 处触发——生成器可用此事件结束高电平脉冲。

.. figure:: /../_static/mcpwm/compare_event.svg
    :align: center
    :alt: 定时器向上计数；比较器在 Tick 15 触发。生成器将其转换为下降沿。

    定时器向上计数；比较器在 Tick 15 触发。生成器将其转换为下降沿。

.. code-block:: c

    mcpwm_cmpr_handle_t comparator = NULL;
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true, // 仅在周期边界改变占空比
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(oper, &comparator_config, &comparator));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, 15));

比较器配置除 flags 外只有一个字段：

.. list::

    - :cpp:member:`intr_priority <mcpwm_comparator_config_t::intr_priority>` — :cpp:member:`on_reach <mcpwm_comparator_event_callbacks_t::on_reach>` 回调使用的中断优先级。不设置（``0``）时由驱动选择较低优先级；当回调需要抢占其他中断时可适当提高。
    - :cpp:member:`flags <mcpwm_comparator_config_t::flags>` — 即下文介绍的更新点。示例启用了 :cpp:member:`update_cmp_on_tez <mcpwm_comparator_config_t::flags::update_cmp_on_tez>`，这也是在周期边界改变占空比的常见做法。

运行时百分比占空比的计算公式为 ``period_ticks * percent / 100``，结果必须不超过定时器周期。

这也是为什么改变占空比应优先改比较器，而不是改生成器动作：动作定义的是波形规则，比较器定义的是当前边沿位置。前者更像拓扑，后者更像运行参数。

为何要推迟更新
==============

立即更新比较器可能改变当前周期中的边沿。:cpp:member:`update_cmp_on_tez <mcpwm_comparator_config_t::flags::update_cmp_on_tez>` 会等待计数归零，:cpp:member:`update_cmp_on_tep <mcpwm_comparator_config_t::flags::update_cmp_on_tep>` 会等待计数到达峰值，:cpp:member:`update_cmp_on_sync <mcpwm_comparator_config_t::flags::update_cmp_on_sync>` 则等待同步事件。在向上计数或向下计数模式下，峰值即周期边界，与归零点几乎重合，因此 tez 与 tep 对应同一个更新点；仅在向上-向下计数模式下，峰值处于周期中点，tez 与 tep 才是两个不同的更新点。电机和电源转换通常应使用推迟更新。

两个比较器实现脉冲放置
=======================

一个比较器每周期产生一个边沿。在同一操作器中使用两个比较器，可在周期内任意位置放置脉冲——一个比较器打开脉冲，另一个关闭。适用于采样窗口、触发信号或非对称死区补偿。

这类需求在电机控制里很常见，例如想把 ADC 采样窗口放在开关噪声较小的位置，或者给外部器件输出一个不位于 PWM 边界的同步脉冲。

.. code-block:: c

    mcpwm_cmpr_handle_t cmp_a, cmp_b;
    mcpwm_new_comparator(oper, &comparator_config, &cmp_a);
    mcpwm_new_comparator(oper, &comparator_config, &cmp_b);
    mcpwm_comparator_set_compare_value(cmp_a, 10);
    mcpwm_comparator_set_compare_value(cmp_b, 30);

比较事件也可用于精确的软件时间标记
====================================

:cpp:member:`on_reach <mcpwm_comparator_event_callbacks_t::on_reach>` 回调在比较值到达时触发，适用于软件需要观察 PWM 周期中精确位置的场景。应在启动时间关键型工作前注册。回调在 ISR 上下文运行，应保持简短。

.. code-block:: c

    static bool IRAM_ATTR on_compare(mcpwm_cmpr_handle_t cmpr,
                                     const mcpwm_compare_event_data_t *edata,
                                     void *user_ctx)
    {
        // 通知任务或仅执行 ISR 安全工作
        return false;
    }

    mcpwm_comparator_event_callbacks_t callbacks = { .on_reach = on_compare };
    ESP_ERROR_CHECK(mcpwm_comparator_register_event_callbacks(comparator,
                                                              &callbacks, NULL));

比较器的种类
============

**操作器比较器** 通过 :cpp:func:`mcpwm_new_comparator()` 创建，其比较事件驱动生成器，从而塑造 PWM 输出。

.. only:: SOC_MCPWM_SUPPORT_EVENT_COMPARATOR

    还存在第二种： **事件比较器**，通过 :cpp:func:`mcpwm_new_event_comparator()` 创建。其比较事件只能经 :doc:`ETM </api-reference/peripherals/etm>` 传给其他外设，不驱动生成器，也不会影响 PWM 输出。

    .. note::

        名字容易误导：操作器比较器同样能产生 ETM 事件，因此事件比较器并非 MCPWM 接入 ETM 的唯一途径。事件比较器的定位是 *补充* 操作器比较器，而非替代。事件比较器不占用操作器比较器名额，也不会移动任何 PWM 边沿，当你只需要一个额外比较点作为纯定时标记时——例如在不扰动 PWM 波形的前提下，为 ADC 触发找到采样窗口——它就是更灵活的选择。

    两种比较器使用相同的比较值设置与 ETM 事件创建方式：

    .. code-block:: c

        mcpwm_event_comparator_config_t evt_cmp_cfg = {};
        mcpwm_cmpr_handle_t evt_cmpr = NULL;
        ESP_ERROR_CHECK(mcpwm_new_event_comparator(oper, &evt_cmp_cfg, &evt_cmpr));
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(evt_cmpr, 25));

        esp_etm_event_handle_t evt = NULL;
        ESP_ERROR_CHECK(mcpwm_comparator_new_etm_event(evt_cmpr,
            &(mcpwm_cmpr_etm_event_config_t){ .event_type = MCPWM_CMPR_ETM_EVENT_EQUAL },
            &evt));

API 参考
========

MCPWM 比较器驱动函数
--------------------

.. include-build-file:: inc/mcpwm_cmpr.inc
