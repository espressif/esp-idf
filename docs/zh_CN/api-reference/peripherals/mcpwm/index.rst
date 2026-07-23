===========================
电机控制脉宽调制器 (MCPWM)
===========================

:link_to_translation:`en:[English]`

从这里开始
============

MCPWM 将计数器转换为时序精确的输出边沿。当 LEDC 的简单 PWM 已无法满足需求时，可使用 MCPWM：电机桥需要互补输出和死区，逆变器需要同步相位，传感器则需要精确测量脉宽。

最小可用的 MCPWM 设计由四个对象构成：:doc:`定时器 <mcpwm_timer>` 提供时间基准，:doc:`操作器 <mcpwm_operator>` 管理波形资源，:doc:`比较器 <mcpwm_cmpr>` 决定边沿位置，:doc:`生成器 <mcpwm_gen>` 驱动 GPIO。其他模块均在此基础上扩展。

构建一路 PWM 输出
==================

初次生成 PWM 输出时，请按下图从左至右创建对象。主线上各阶段按角色配色：时间基准（蓝色）、操作器核心（紫色）、波形配置（青色）、启用与输出（绿色）。琥珀色节点为基础输出正常后按需添加的扩展，红色节点为安全制动。只有在所有输出动作均已配置完成后，才启动定时器。

.. mermaid::

    flowchart LR
        T1["1. 创建定时器<br/>mcpwm_new_timer"]:::time
        O1["2. 创建操作器<br/>mcpwm_new_operator"]:::core
        LINK["3. 连接时间基准<br/>mcpwm_operator_connect_timer"]:::core
        C1["4. 创建比较器<br/>mcpwm_new_comparator"]:::wave
        G1["5. 创建生成器<br/>mcpwm_new_generator"]:::wave
        A1["6. 描述边沿<br/>mcpwm_generator_set_action_on_*_event"]:::wave
        RUN["7. 启用并启动<br/>mcpwm_timer_enable<br/>mcpwm_timer_start_stop"]:::run
        PIN["GPIO 输出 PWM"]:::output

        T1 --> O1 --> LINK --> C1 --> G1 --> A1 --> RUN --> PIN

        DT["死区<br/>mcpwm_generator_set_dead_time"]:::optional
        BR["故障与制动<br/>mcpwm_new_*_fault<br/>mcpwm_operator_set_brake_on_fault"]:::safety
        SY["相位同步<br/>mcpwm_new_*_sync_src<br/>mcpwm_timer_set_phase_on_sync"]:::optional
        CA["载波调制<br/>mcpwm_operator_apply_carrier"]:::optional

        A1 -. 扩展 .-> DT
        O1 -. 保护 .-> BR
        T1 -. 对齐 .-> SY
        O1 -. 调制 .-> CA

        classDef time fill:#dbeafe,stroke:#2563eb,color:#172554
        classDef core fill:#ede9fe,stroke:#7c3aed,color:#2e1065
        classDef wave fill:#cffafe,stroke:#0891b2,color:#164e63
        classDef run fill:#dcfce7,stroke:#16a34a,color:#14532d
        classDef output fill:#bbf7d0,stroke:#15803d,color:#14532d
        classDef optional fill:#fef3c7,stroke:#d97706,color:#78350f
        classDef safety fill:#fee2e2,stroke:#dc2626,color:#7f1d1d

下面这段代码创建一路 20 kHz、30% 占空比的 PWM 输出，可作为阅读后续各页前的整体参考。它展示了对象的创建顺序，也说明了运行时最常改动的其实是比较器，而不是重新配置整条链路。

.. code-block:: c

    mcpwm_timer_handle_t timer = NULL;
    mcpwm_oper_handle_t oper = NULL;
    mcpwm_cmpr_handle_t comparator = NULL;
    mcpwm_gen_handle_t generator = NULL;

    // 1 MHz → 1 tick = 1 µs
    // 50 ticks → 50 µs 周期 → 20 kHz
    ESP_ERROR_CHECK(mcpwm_new_timer(
        &(mcpwm_timer_config_t) {
            .group_id = 0,
            .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
            .resolution_hz = 1000000,
            .period_ticks = 50,
            .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        },
        &timer));

    ESP_ERROR_CHECK(mcpwm_new_operator(
        &(mcpwm_operator_config_t) {
            .group_id = 0,
        },
        &oper));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));

    ESP_ERROR_CHECK(mcpwm_new_comparator(
        oper,
        &(mcpwm_comparator_config_t) {
            .flags.update_cmp_on_tez = true,
        },
        &comparator));
    // 15 / 50 = 30% 占空比
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, 15));

    ESP_ERROR_CHECK(mcpwm_new_generator(
        oper,
        &(mcpwm_generator_config_t) {
            .gen_gpio_num = 18,
        },
        &generator));

    // 定时器归零 → 输出 HIGH；比较器匹配 → 输出 LOW
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
        generator,
        MCPWM_GEN_TIMER_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP,
            MCPWM_TIMER_EVENT_EMPTY,
            MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator,
        MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP,
            comparator,
            MCPWM_GEN_ACTION_LOW)));

    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    // 运行时修改比较值即可调整占空比，无需重建生成器动作。
    // 25 / 50 = 50% 占空比
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, 25));

定时器的 ``resolution_hz`` 和 ``period_ticks`` 确定时序刻度；比较器的 ``compare_value`` 在该刻度中选择边沿位置；生成器动作 API 决定在定时器边界或比较器越过阈值时输出何种电平。这种分工也便于调参：改变定时器可调整频率，改变比较器可调整占空比或边沿位置，改变生成器动作可调整极性或波形形状。

波形配置完成后，调用 :cpp:func:`mcpwm_timer_enable()` 和 :cpp:func:`mcpwm_timer_start_stop()`。运行时应通过 :cpp:func:`mcpwm_comparator_set_compare_value()` 更新比较器，而不是重新配置生成器动作。仅在应用需要时添加对应扩展：半桥使用死区，安全路径使用故障与制动，相位对齐使用同步，隔离式驱动使用载波。

功能地图
========

.. list-table::
    :header-rows: 1
    :widths: 16 36 34 24

    * - 目标
      - 先看哪些页
      - 关键 API
      - 典型应用
    * - 输出单路 PWM
      - :doc:`定时器 <mcpwm_timer>` -> :doc:`操作器 <mcpwm_operator>` -> :doc:`比较器 <mcpwm_cmpr>`

        :doc:`生成器 <mcpwm_gen>`
      - :cpp:func:`mcpwm_new_timer`

        :cpp:func:`mcpwm_new_comparator`

        ``mcpwm_generator_set_action_on_*_event``
      - 舵机、调光、基础功率控制
    * - 输出互补半桥 PWM
      - :doc:`生成器 <mcpwm_gen>` 中的死区小节 + :doc:`故障 <mcpwm_fault>`
      - :cpp:func:`mcpwm_generator_set_dead_time`

        :cpp:func:`mcpwm_operator_set_brake_on_fault`
      - 半桥、逆变桥臂
    * - 多路同频对齐或移相
      - :doc:`同步 <mcpwm_sync>`
      - :cpp:func:`mcpwm_timer_set_phase_on_sync`

        :cpp:func:`mcpwm_new_timer_sync_src`
      - 多相电机、并联变换器
    * - 测量输入脉宽或周期
      - :doc:`捕获 <mcpwm_cap>`
      - :cpp:func:`mcpwm_new_capture_timer`

        :cpp:func:`mcpwm_capture_channel_register_event_callbacks`
      - HC-SR04、转速计、RC 输入
    * - 外设间硬件联动
      - :doc:`ETM <mcpwm_etm>`
      - :cpp:func:`mcpwm_timer_new_etm_event`

        :cpp:func:`mcpwm_new_event_comparator`
      - ADC 触发、跨外设定时链路

本指南中每个页面介绍一个 MCPWM 模块：

.. toctree::
    :maxdepth: 1

    mcpwm_timer
    mcpwm_operator
    mcpwm_cmpr
    mcpwm_gen
    mcpwm_fault
    mcpwm_sync
    mcpwm_cap
    mcpwm_etm
    mcpwm_advanced

资源与生命周期
==============

所有对象都属于一个 MCPWM 组。连接的定时器与操作器必须位于同一组；GPIO 故障源和 GPIO 同步源也只能在本组中使用。硬件资源有限，创建时可能返回 :c:macro:`ESP_ERR_NOT_FOUND`。

每个对象都由 ``mcpwm_new_*()`` 工厂函数创建并返回一个不透明句柄，由对应的 ``mcpwm_del_*()`` 函数释放，例如 :cpp:func:`mcpwm_new_timer()` 与 :cpp:func:`mcpwm_del_timer()`。先创建父对象，再创建子对象；释放时按相反顺序执行：先删除生成器/比较器，再删除操作器，最后删除定时器。删除定时器前必须禁用它；删除捕获定时器前必须删除其通道。

组时钟分频器由定时器共享，部分芯片的捕获定时器也共享它。按目标分辨率单调顺序（从高到低或从低到高）创建对象，可避免分频冲突。详见 :doc:`高级主题 <mcpwm_advanced>`。

术语速查
========

.. list::

    - **TEZ：** Timer equals zero，定时器计数等于零时触发的事件。
    - **TEP：** Timer equals peak，定时器计数达到峰值时触发的事件。
    - **定时器（Timer）：** MCPWM 的时间基准，决定频率和 Tick 刻度。
    - **操作器（Operator）：** 连接定时器与输出逻辑的容器，管理比较器、生成器、制动、死区和载波。
    - **比较器（Comparator）：** 当计数达到阈值时发出事件，常用于决定边沿位置和占空比。
    - **生成器（Generator）：** 根据定时器/比较器/故障/同步事件输出 GPIO 电平。
    - **死区（Dead Time）：** 在半桥上下管切换之间插入的非重叠时间，避免直通。
    - **故障（Fault）：** 进入保护路径的异常源，可来自 GPIO 或软件。
    - **制动（Brake）：** 故障触发后的输出安全策略。
    - **CBC：** Cycle By Cycle，故障有效时制动，清除后在周期边界自动恢复。
    - **OST：** One Shot，一次制动后保持锁存，需软件显式恢复。
    - **同步（Sync）：** 在同步边沿把定时器加载到指定计数值和方向，以实现对齐或移相。
    - **捕获（Capture）：** 对输入边沿打时间戳，用于测脉宽、周期或转速。

应用示例
========

.. list::

    - :example:`peripherals/mcpwm/mcpwm_servo_control`：RC 舵机的单路 PWM。
    - :example:`peripherals/mcpwm/mcpwm_bdc_speed_control`：有刷直流电机及速度反馈。
    - :example:`peripherals/mcpwm/mcpwm_bldc_hall_control`：使用霍尔传感器反馈进行 BLDC 换相。
    - :example:`peripherals/mcpwm/mcpwm_capture_hc_sr04`：使用 HC-SR04 测量脉宽。
    - :example:`peripherals/mcpwm/mcpwm_sync`：GPIO、定时器和软件同步。
    - :example:`peripherals/mcpwm/mcpwm_foc_svpwm_open_loop`：开环 FOC 的三对互补 PWM。

API 参考
========

通用类型
--------

.. include-build-file:: inc/components/esp_driver_mcpwm/include/driver/mcpwm_types.inc
.. include-build-file:: inc/components/esp_hal_mcpwm/include/hal/mcpwm_types.inc
