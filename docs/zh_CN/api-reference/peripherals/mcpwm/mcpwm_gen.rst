============================
MCPWM 生成器：生成 PWM 波形
============================

.. contents::
    :local:
    :depth: 2

生成器是最终的数字输出。它没有固定的"占空比"属性，而是配置在定时器和比较器事件发生时输出何种电平，因此可实现简单、非对称、互补和相位相关的波形。

这也是 MCPWM 与简单 PWM 外设的重要区别之一：它不是只让你填一个频率和占空比，而是让你直接描述"在什么时刻做什么动作"。代价是初学时概念更多，但换来的是对波形结构更高的控制力。

生成第一路 PWM
===============

这是对 :doc:`定时器 <mcpwm_timer>`、:doc:`操作器 <mcpwm_operator>` 和 :doc:`比较器 <mcpwm_cmpr>` 配置的收尾：定时器归零时输出高电平，比较器到达 15 时输出低电平。对于 50 Tick 的周期，输出高电平 15 Tick，即 30% 占空比。

应用场景：基础单路输出
----------------------

适用于最基础的单路 PWM 输出，例如 RC 舵机控制信号、LED 调光，或后级电路已处理好极性和保护关系的简单占空比输出。

.. code-block:: c

    mcpwm_gen_handle_t generator = NULL;
    mcpwm_generator_config_t gen_config = { .gen_gpio_num = 18 };
    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &gen_config, &generator));

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
        generator, MCPWM_GEN_TIMER_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY,
            MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW)));

.. figure:: /../_static/mcpwm/single_edge_asym_active_high.svg
    :align: center
    :alt: 向上计数、有效高 PWM：归零时置高，比较点置低。

    向上计数、有效高 PWM：归零时置高，比较点置低。

生成器配置很简单：

.. list::

    - :cpp:member:`gen_gpio_num <mcpwm_generator_config_t::gen_gpio_num>` — 输出 PWM 的 GPIO。同一操作器中另一个生成器若配置相同的动作，即可从同一时间基准驱动第二个引脚。
    - :cpp:member:`invert_pwm <mcpwm_generator_config_t::flags::invert_pwm>` — 通过 GPIO 矩阵反相 PWM 信号。这是对最终输出的硬件反相，与修改动作不同；两者选其一，不要同时使用。

动作配置
========

配置名称直接描述了行为：在某个事件发生时执行 :cpp:enumerator:`MCPWM_GEN_ACTION_HIGH <mcpwm_generator_action_t::MCPWM_GEN_ACTION_HIGH>`、:cpp:enumerator:`MCPWM_GEN_ACTION_LOW <mcpwm_generator_action_t::MCPWM_GEN_ACTION_LOW>` 或 :cpp:enumerator:`MCPWM_GEN_ACTION_TOGGLE <mcpwm_generator_action_t::MCPWM_GEN_ACTION_TOGGLE>`。辅助宏在调用处清晰展示三个关键选择——方向、事件源和输出电平。

对于向上计数定时器，:cpp:enumerator:`MCPWM_TIMER_EVENT_EMPTY <mcpwm_timer_event_t::MCPWM_TIMER_EVENT_EMPTY>` 是归零边界，:cpp:enumerator:`MCPWM_TIMER_EVENT_FULL <mcpwm_timer_event_t::MCPWM_TIMER_EVENT_FULL>` 在定时器到达峰值时触发。向上计数模式下峰值即周期，因此 ``FULL`` 落在周期边界；向上-向下模式下峰值是 ``period_ticks / 2``，因此 ``FULL`` 落在周期中间。比较动作使用比较器阈值。因此，第一个例子的含义是"周期开始时置高，计数到 15 时结束有效部分"。超出定时器范围的比较值不会产生比较事件。

每个动作都必须指定定时器方向，尽管这一选择只在向上-向下模式下才有可见差异。向上计数模式下计数器只向上走，因此为 :cpp:enumerator:`MCPWM_TIMER_DIRECTION_UP <mcpwm_timer_direction_t::MCPWM_TIMER_DIRECTION_UP>` 配置的动作才会触发——仍需显式写出。在向上-向下模式中，两个边沿都重要时，应同时为 :cpp:enumerator:`MCPWM_TIMER_DIRECTION_UP <mcpwm_timer_direction_t::MCPWM_TIMER_DIRECTION_UP>` 和 :cpp:enumerator:`MCPWM_TIMER_DIRECTION_DOWN <mcpwm_timer_direction_t::MCPWM_TIMER_DIRECTION_DOWN>` 配置动作；这样便能用一个比较器生成中心对齐 PWM。

经典波形案例
============

下面的示例都基于上一节的第一路 PWM，复用相同的定时器、操作器和比较器对象，生成其他常用波形。

反转有效极性
------------

某些栅极驱动器和 LED 为低有效，例如低有效驱动器使能、反相 LED 接法，或板级电路已固定反相的接口。与其增加 GPIO 反相，不如在周期边界置低、比较点置高：

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
        generator, MCPWM_GEN_TIMER_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_LOW)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_HIGH)));

.. figure:: /../_static/mcpwm/single_edge_asym_active_low.svg
    :align: center
    :alt: 向上计数、有效低 PWM。当输出逻辑极性是设计的一部分时，修改动作而非接线。

    向上计数、有效低 PWM。当输出逻辑极性是设计的一部分时，修改动作而非接线。

在周期内放置脉冲
----------------

当需要把某个短脉冲精确放到周期内部——例如 ADC 采样窗口、外设触发脉冲或锁存控制信号——两个比较值分别决定脉冲的开始和结束边沿：

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, comparator_a, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, comparator_b, MCPWM_GEN_ACTION_LOW)));

.. figure:: /../_static/mcpwm/pulse_placement_asym.svg
    :align: center
    :alt: 脉冲放置：两个比较值之差就是脉冲宽度。

    脉冲放置：两个比较值之差就是脉冲宽度。

应使 ``comparator_a`` 小于 ``comparator_b``。同时以相同 Tick 数移动二者可改变位置而不改变宽度；只移动其中一个则改变宽度。硬件会放置两个边沿，因此比定时器回调更精确。

双沿非对称 PWM
--------------

当需要在一个周期内独立布置多个边沿、且高电平区间不必围绕周期中心对称时——例如某些非对称逆变器调制或定制驱动时序——使用两个生成器和两个比较器。每个生成器在每周期有自己的边沿，使高电平时间分布在周期边界两侧：

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        gen_a, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, cmp_a, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        gen_a, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, cmp_b, MCPWM_GEN_ACTION_LOW)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        gen_b, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, cmp_a, MCPWM_GEN_ACTION_LOW)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        gen_b, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, cmp_b, MCPWM_GEN_ACTION_HIGH)));

.. figure:: /../_static/mcpwm/dual_edge_asym_active_low.svg
    :align: center
    :alt: 双沿非对称（边沿对齐）PWM：两个生成器产生互补输出，每周期两个边沿。

    双沿非对称（边沿对齐）PWM：两个生成器产生互补输出，每周期两个边沿。

中心对齐 PWM
------------

电机控制、逆变器等关注谐波表现的功率级通常更偏好中心对齐 PWM，因为它能带来更对称的开关时序和更低的谐波失真。创建定时器时选择 ``MCPWM_TIMER_COUNT_MODE_UP_DOWN``，随后在两个方向使用同一阈值：

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_DOWN, comparator, MCPWM_GEN_ACTION_LOW)));

.. figure:: /../_static/mcpwm/dual_edge_sym_active_low.svg
    :align: center
    :alt: 中心对齐 PWM：向上和向下计数动作在周期中心两侧形成对称边沿。

    中心对齐 PWM：向上和向下计数动作在周期中心两侧形成对称边沿。

一次完整的向上-向下周期会到达峰值后回到零点，计算频率时需计入两个计数阶段。为第二个生成器配置相反动作可形成逻辑互补输出：

.. figure:: /../_static/mcpwm/dual_edge_sym_complementary.svg
    :align: center
    :alt: 仅靠互补生成器动作不含死区，不能直接连接到功率级。

    仅靠互补生成器动作不含死区，不能直接连接到功率级。

.. warning::

    逻辑互补输出不等于可直接驱动半桥的安全互补输出。只要上下管存在关断延迟，就必须继续配置死区，并在实际栅极处验证无重叠导通。

占空比调整
==========

改变占空比时修改比较器阈值，而不是生成器动作。对于向上计数的有效高波形，``compare_value / period_ticks`` 即为占空比。应选择足够高的定时器分辨率，使一个 Tick 满足应用所需的调节精度。

.. code-block:: c

    mcpwm_comparator_set_compare_value(comparator, 25); // 50 %

强制电平
========

调节比较器阈值改变的是正常占空比。若需要临时接管输出、忽略所有事件动作而直接固定某个电平时，可改用强制电平接口。:func:`mcpwm_generator_set_force_level` 有两个关键参数：``level`` 和 ``hold_on``。

- ``level``（第二个参数）指定要强制输出的原始生成器电平：``0`` 或 ``1`` 会覆盖所有事件动作，``-1`` 则解除强制并恢复事件控制。
- ``hold_on``（第三个参数）决定强制电平持续多久：``true`` 会一直保持直到下一次调用解除；``false`` 则允许下一个事件动作将其覆盖。

例如 ``mcpwm_generator_set_force_level(generator, 0, true)`` 会覆盖所有事件动作并保持原始生成器为低电平。强制电平位于死区与 GPIO 反相之前，应使用示波器确认实际引脚电平。

强制电平常用于上电自检、故障后的临时安全输出、切换工作模式时的过渡状态，而不是长期替代正常 PWM 配置。

对于半桥，添加第二个生成器并配置 :ref:`死区模块 <mcpwm-dead-time>` 以产生非重叠互补输出。

.. _mcpwm-dead-time:

死区与半桥驱动
==============

死区会延迟输出边沿，在半桥两个开关之间留出同时关闭的短暂间隔，以补偿晶体管关断延迟并降低直通风险。连接功率级前应配置并验证死区。

半桥通过高边与低边两个开关将直流母线连接到负载。两个开关通常都采用 N 沟道 MOSFET：低边源极接 GND、易于驱动，而高边源极随输出摆动，其栅极需要高于母线电压的驱动电压。MCPWM 输出是 3.3 V 逻辑信号，不能直接驱动栅极。以 IRS2101 为例，它使用独立的低压驱动电源 VCC（通常为 10-20 V），COM 接功率地。自举二极管应从 VCC 接到 VB，外部自举电容接在 VB 与 VS 之间，VS 必须连接到 OUT 半桥开关节点。高边输出为 HO，低边输出为 LO，两者都应通过栅极电阻连接到对应 MOSFET 的栅极。VCC 不是高压直流母线：低边开关导通时，自举二极管从受调节的驱动电源为自举电容充电。若两个开关同时开通和关断，仍在关断的开关会与已经开通的开关重叠导通，使母线经两个开关直通到地。死区留出两者同时关断的间隔，让下一个开关在前一个完全关断之后再开通：

.. figure:: /../_static/mcpwm/half_bridge_dead_time.svg
    :align: center
    :alt: 带自举电源和死区对比的半桥栅极驱动电路。

    带自举电源和死区对比的半桥栅极驱动电路。

创建互补输出
------------

在同一操作器中创建两个生成器。将生成器 A 的上升沿延迟后输出到自身，再将其下降沿延迟并反相后输出到生成器 B。

.. note::

    这里约定，生成器 A 是通过操作器句柄申请的第一个生成器，生成器 B 是通过操作器句柄申请的第二个生成器。

.. code-block:: c

    mcpwm_dead_time_config_t dead_time = { .posedge_delay_ticks = 2 };
    ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(gen_a, gen_a, &dead_time));
    dead_time = (mcpwm_dead_time_config_t) {
        .negedge_delay_ticks = 2,
        .flags.invert_output = true,
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(gen_a, gen_b, &dead_time));

.. figure:: /../_static/mcpwm/deadtime_active_high_complementary.svg
    :align: center
    :alt: 带死区间隔的互补 PWM。

    带死区间隔的互补 PWM。

理解路由与参数
--------------

:func:`mcpwm_generator_set_dead_time(in_generator, out_generator, config) <mcpwm_generator_set_dead_time>` 将死区视为一个小型信号处理级。两个生成器句柄相同会原地改变该输出；将 ``gen_a`` 作为输入、 ``gen_b`` 作为输出则从 A 派生 B，这正是互补例子共享一个 PWM 源的方式。

:cpp:member:`posedge_delay_ticks <mcpwm_dead_time_config_t::posedge_delay_ticks>` 延迟上升沿，:cpp:member:`negedge_delay_ticks <mcpwm_dead_time_config_t::negedge_delay_ticks>` 延迟下降沿，单位为连接定时器的 Tick。例如分辨率为 10 MHz 时，2 Tick 等于 200 ns。下图展示了基本效果：``pwm_A`` 的上升沿被延迟（RED），``pwm_B`` 的下降沿被延迟（FED）。应先采用开关管和栅极驱动器数据手册中的最大关断延迟并留出余量；在晶体管栅极实测后，确认工艺、温度和布局仍有足够余量，才可减小该值。两个延迟均设为零可旁路死区模块。:cpp:member:`invert_output <mcpwm_dead_time_config_t::flags::invert_output>` 在该级之后改变极性。

.. figure:: /../_static/mcpwm/deadtime_active_high.svg
    :align: center
    :alt: 基本死区效果：上升沿延迟（RED）和下降沿延迟（FED）。

    基本死区效果：上升沿延迟（RED）和下降沿延迟（FED）。

每个操作器的资源限制
--------------------

每个操作器仅有一份上升沿延迟和一份下降沿延迟资源，不能将同一类延迟分别分配给两个生成器。下面的配置两次请求同一上升沿资源，因此无效：

.. code-block:: c

    mcpwm_dead_time_config_t rise_delay = { .posedge_delay_ticks = 10 };
    ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(gen_a, gen_a, &rise_delay));
    // 无法再分配第二份独立的上升沿延迟。
    ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(gen_b, gen_b, &rise_delay));

可以将上升沿延迟分配给 A、下降沿延迟分配给 B；也可以让 A 旁路模块，而让 B 使用两种延迟资源。如果第一个生成器同时使用两种延迟资源，另一个生成器便不能使用死区。

更多输出模式
------------

上面的互补配置是半桥的常用起点。交换输出反相设置可使两路输出均为低有效，同时保留非重叠时间：

.. figure:: /../_static/mcpwm/deadtime_active_low_complementary.svg
    :align: center
    :alt: 低有效互补输出。时序资源相同，仅改变死区后的输出极性。

    低有效互补输出。时序资源相同，仅改变死区后的输出极性。

仅有一个通道需要边沿延迟时，死区同样有用。向其中一个输出传入零延迟配置使其旁路，再将可用延迟应用到另一个输出：

.. figure:: /../_static/mcpwm/deadtime_reda_bypassb.svg
    :align: center
    :alt: 延迟 A 的上升沿，B 旁路死区。这不是互补半桥，而是独立的边沿放置工具。

    延迟 A 的上升沿，B 旁路死区。这不是互补半桥，而是独立的边沿放置工具。

.. figure:: /../_static/mcpwm/deadtime_redb_fedb_bypassa.svg
    :align: center
    :alt: 旁路 A，延迟 B 的两个边沿，并占用两种延迟资源。

    旁路 A，延迟 B 的两个边沿，并占用两种延迟资源。

单个边沿延迟也可独立应用。下图展示了仅对 B 的下降沿延迟，A 被旁路，仅使用 FED 资源：

.. figure:: /../_static/mcpwm/deadtime_fedb_bypassa.svg
    :align: center
    :alt: 仅对 B 应用下降沿延迟，A 保持不变。使用一个延迟资源。

    仅对 B 应用下降沿延迟，A 保持不变。使用一个延迟资源。

输出反相时，死区行为随之变化。下图展示了基本延迟的低有效版本，反相标志翻转了两个输出的极性：

.. figure:: /../_static/mcpwm/deadtime_active_low.svg
    :align: center
    :alt: 低有效死区：相同的延迟资源，但输出极性在延迟级后被反相。

    低有效死区：相同的延迟资源，但输出极性在延迟级后被反相。

.. note::

    若波形的每个边沿都需要独立调整位置，应使用两个比较器和生成器动作。需求明确是"延迟边沿并控制极性"时，死区模块更合适。

在安全边界更新
--------------

若要让改变仅在安全边界生效，请设置操作器的 :cpp:member:`update_dead_time_on_tez <mcpwm_operator_config_t::flags::update_dead_time_on_tez>`、:cpp:member:`update_dead_time_on_tep <mcpwm_operator_config_t::flags::update_dead_time_on_tep>` 或 :cpp:member:`update_dead_time_on_sync <mcpwm_operator_config_t::flags::update_dead_time_on_sync>`。

.. note::

    应探测两个实际栅极引脚：GPIO 反相、载波调制和栅极驱动器极性都可能改变晶体管端看到的信号。当多个环节都会反转信号时，两次反相可能相互抵消，软件里看着正确，硬件却并非预期行为，务必以真实波形为准进行验证。

其他事件源
============

生成器也可直接响应 GPIO 故障和同步事件：

.. list::

    - :cpp:func:`mcpwm_generator_set_action_on_fault_event()` — 立即硬件响应 GPIO 故障。会占用有限的操作器触发槽。
    - :cpp:func:`mcpwm_generator_set_action_on_sync_event()` — 在同步边沿转换。每个生成器只有一个同步动作槽。
    - :cpp:func:`mcpwm_generator_set_action_on_brake_event()` — 操作器 :ref:`制动 <mcpwm-brake>` 期间各生成器的输出状态。需要按制动模式和定时器方向分别设置，完整示例见 :ref:`故障连接 <mcpwm-brake-fault-connection>`。

对于安全策略和持续制动，应使用操作器 :ref:`制动机制 <mcpwm-brake>`。生成器故障动作适合局部的边沿级响应；制动则为整个输出级定义安全状态和恢复行为。

API 参考
========

MCPWM 生成器驱动函数
--------------------

.. include-build-file:: inc/mcpwm_gen.inc
