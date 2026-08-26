=============================
MCPWM ETM：外设间的硬件级联动
=============================

.. contents::
    :local:
    :depth: 2

.. only:: SOC_MCPWM_SUPPORT_ETM

    事件任务矩阵（ETM）可将 MCPWM 定时器或比较器事件直接路由到 ETM 任务，避免 ISR 延迟。适用于其他外设必须在精确 PWM 相位响应的场景。

    先从定时器或比较器创建 ETM 事件，再创建目标外设的兼容任务，最后通过 ETM 通道连接两者。目标外设驱动文档定义了其任务和完整通道配置。关于 ETM 的整体用法——分配通道、把事件连接到任务——请参阅 :doc:`ETM </api-reference/peripherals/etm>` 文档。

    .. mermaid::

        flowchart LR
            T["MCPWM 定时器<br/>TEZ/TEP 事件"]:::source --> E["ETM 通道"]:::route
            C["MCPWM 比较器<br/>比较事件"]:::source --> E
            E --> D["目标<br/>外设任务"]:::dest
            classDef source fill:#dbeafe,stroke:#2563eb,color:#172554
            classDef route fill:#ede9fe,stroke:#7c3aed,color:#2e1065
            classDef dest fill:#dcfce7,stroke:#16a34a,color:#14532d

    定时器会产生 ``TEZ`` （定时器归零）或 ``TEP`` （定时器到达峰值）事件。要获取一个事件：

    .. code-block:: c

        esp_etm_event_handle_t timer_event = NULL;
        ESP_ERROR_CHECK(mcpwm_timer_new_etm_event(timer,
            &(mcpwm_timer_etm_event_config_t) {
                .event_type = MCPWM_TIMER_ETM_EVENT_TEZ,
            }, &timer_event));
        // 创建目标 ETM task、分配 channel，然后连接：
        // esp_etm_channel_connect(channel, timer_event, destination_task);

    比较器提供 ``EQUAL`` 事件，每当定时器计数值等于比较器取值时触发。它能把事件固定到 PWM 周期中的任意相位，而不只是波峰或波谷。要获取一个事件：

    .. code-block:: cpp

        esp_etm_event_handle_t cmp_event = NULL;
        ESP_ERROR_CHECK(mcpwm_comparator_new_etm_event(cmp,
            &(mcpwm_cmpr_etm_event_config_t) {
                .event_type = MCPWM_CMPR_ETM_EVENT_EQUAL,
            }, &cmp_event));
        // esp_etm_channel_connect(channel, cmp_event, destination_task);

    使用完毕后调用 :cpp:func:`esp_etm_del_event()` 释放事件。

    比较器事件一个非常常见的用途是触发 ADC 采样：把比较器设置在想要采样的相位点，再由比较器事件启动 ADC，使转换器在与 PWM 周期精确同步的相位点采样稳定、干净的波形。由于整条链路完全由硬件完成，采样点能无 CPU 参与、无 ISR 延迟地跟随 PWM。

    .. only:: SOC_MCPWM_SUPPORT_EVENT_COMPARATOR

        用哪种比较器提供事件很关键。 **运算比较器** （:cpp:func:`mcpwm_new_comparator()`）同时驱动生成器，其比较值会定义实际的 PWM 输出边沿，因此它的 ETM 事件被限制在对应的 PWM 边沿上——你无法要求它在不是自己产生的边沿相位上触发。 **事件比较器** （:cpp:func:`mcpwm_new_event_comparator()`）则是专门的 ETM 定时标记：它不驱动任何生成器，也不占用运算比较器槽位，因此可以在 PWM 周期内的 *任意* 相位触发，且对 PWM 波形毫无影响。这种自由度正是 ADC 触发所需要的，所以更推荐用事件比较器作为事件源——把采样点放在电压已稳定的位置，而不只是恰好有边沿的地方。

    API 参考
    ========

    MCPWM ETM 驱动函数
    ------------------

    .. include-build-file:: inc/mcpwm_etm.inc

.. only:: not SOC_MCPWM_SUPPORT_ETM

    {IDF_TARGET_NAME} 不支持 MCPWM ETM 事件。
