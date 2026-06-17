==========
模拟比较器
==========

:link_to_translation:`en:[English]`

.. contents::
    :local:
    :depth: 2

概览
====

模拟比较器用于判断一个源信号当前是高于还是低于参考信号。参考信号可以来自内部或者外部。

因此，模拟比较器适合以下任务：

- 检测波形是否越过某个固定阈值
- 以某个参考电平为界，把正弦波转换为数字方波
- 比较两个模拟信号的大小关系
- 在比较结果发生变化时触发中断或 ETM 事件
- 在不持续占用 CPU 的情况下构建简单的硬件信号处理链路

.. note::

    模拟比较器外设并不是像某些独立模拟比较器芯片或运放方案那样以“连续时间”方式工作。该外设由时钟驱动，因此比较结果带有采样特性。实际应用中，时钟源、扫描时间、去抖和 ETM 配置都会影响你观察到的结果。

快速开始
========

如果你第一次接触这个外设，推荐按以下最小流程上手：

1. 创建比较器单元
2. 选择源信号和参考信号来源
3. 配置内部参考电压或外部参考输入
4. 按需配置去抖
5. 注册回调或创建 ETM 连接
6. 使能比较器

下面的示例使用内部参考电压和中断回调，检测源信号何时越过 50% VDD：

.. code:: c

    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = 0,                            // 选择当前目标上可用的比较器单元
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,  // 大多数应用可以使用默认时钟源
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL, // 使用内部参考，适合简单的固定阈值
        .cross_type = ANA_CMPR_CROSS_ANY,     // 既关注上穿也关注下穿
        .src_chan0_gpio = 0,                  // 需替换为你板卡上实际连接的模拟比较器源 GPIO
    };
    ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));

    ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD, // 设置内部参考电压为 VDD 的 50%，即判断源信号是否高于半个电源电压
    };
    ESP_ERROR_CHECK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));

    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 10, // 设置 10 微秒的去抖时间，以抑制信号在阈值附近的噪声导致的重复中断
    };
    ESP_ERROR_CHECK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));

    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = example_ana_cmpr_on_cross_callback, // 注册一个回调函数，在发生跨越事件时调用；这个回调函数应该在你的代码的其他地方定义
    };
    ESP_ERROR_CHECK(ana_cmpr_register_event_callbacks(cmpr, &cbs, NULL));

    ESP_ERROR_CHECK(ana_cmpr_enable(cmpr));

这个流程引入了最重要的几个概念：

- :cpp:func:`ana_cmpr_new_unit` 创建比较器实例，并选择源 GPIO 和参考来源。
- :cpp:func:`ana_cmpr_set_internal_reference` 在使用内部参考时设置阈值。
- :cpp:func:`ana_cmpr_set_debounce` 可以抑制噪声或慢速信号在阈值附近反复抖动导致的重复中断。
- :cpp:func:`ana_cmpr_register_event_callbacks` 让应用在比较结果变化时执行自定义逻辑。
- :cpp:func:`ana_cmpr_enable` 在配置完成后启动硬件。

后面的章节会围绕典型应用场景，继续展开这些 API 的实际用法。

生命周期与 API 有效状态
------------------------

下图展示了模拟比较器的生命周期，以及这些 API 分别适合在哪个状态下使用：

.. mermaid::

    flowchart TD
        NC([未创建]) -->|ana_cmpr_new_unit| INIT[Init / Disabled]
        INIT -->|ana_cmpr_enable| EN[Enabled]
        EN -->|ana_cmpr_disable| INIT
        INIT -->|ana_cmpr_del_unit| NC

        subgraph INIT_APIS [Init 状态 API]
            SCAN[ana_cmpr_set_scan_config]
            ADD[ana_cmpr_add_src_chan<br/>ana_cmpr_remove_src_chan]
            CB[ana_cmpr_register_event_callbacks]
        end

        subgraph BOTH_APIS [两个状态都可用的 API]
            IR[ana_cmpr_set_internal_reference]
            DBC[ana_cmpr_set_debounce]
        end

        subgraph EN_APIS [仅 Enabled 状态可用 API]
            TRIG[ana_cmpr_trigger_scan]
        end

        INIT -. 可调用 .-> SCAN
        INIT -. 可调用 .-> ADD
        INIT -. 可调用 .-> CB
        INIT -. 可调用 .-> IR
        INIT -. 可调用 .-> DBC
        EN -. 可调用 .-> IR
        EN -. 可调用 .-> DBC
        EN -. 可调用 .-> TRIG

        classDef stateNeutral fill:#f8fafc,stroke:#64748b,stroke-width:1.5px,color:#111827;
        classDef stateInit fill:#eff6ff,stroke:#2563eb,stroke-width:1.5px,color:#111827;
        classDef stateEnabled fill:#fdf2f8,stroke:#db2777,stroke-width:1.5px,color:#111827;
        classDef initApi fill:#eff6ff,stroke:#60a5fa,color:#111827;
        classDef bothApi fill:#ecfdf5,stroke:#10b981,color:#111827;
        classDef enabledApi fill:#fdf2f8,stroke:#f472b6,color:#111827;

        class NC stateNeutral;
        class INIT stateInit;
        class EN stateEnabled;
        class SCAN,ADD,CB initApi;
        class IR,DBC bothApi;
        class TRIG enabledApi;

可以重点记住下面几个约束：

- :cpp:func:`ana_cmpr_register_event_callbacks`、:cpp:func:`ana_cmpr_add_src_chan`、:cpp:func:`ana_cmpr_remove_src_chan` 和 :cpp:func:`ana_cmpr_set_scan_config` 适合在比较器仍处于 init 状态时调用。
- :cpp:func:`ana_cmpr_trigger_scan` 只能在 :cpp:func:`ana_cmpr_enable` 之后使用。
- :cpp:func:`ana_cmpr_del_unit` 要求比较器先处于 disabled 状态。

场景一：检测信号何时越过固定阈值
==================================

当你只有一个模拟输入，并且只关心它当前高于还是低于某个阈值时，最适合使用这种方式。

典型例子包括：

- 检测传感器输出是否超过某个门限
- 以固定阈值把正弦波转换成数字波形
- 以中点电压为界，检测信号的上下翻转

实现这种需求最简单的方法，就是把源信号与内部参考电压进行比较。

示例：内部参考加回调
--------------------

.. code:: c

    static bool IRAM_ATTR example_ana_cmpr_on_cross_callback(ana_cmpr_handle_t cmpr,
                                                             const ana_cmpr_cross_event_data_t *edata,
                                                             void *user_ctx)
    {
        if (edata->cross_type == ANA_CMPR_CROSS_POS) {
            gpio_set_level(EXAMPLE_MONITOR_GPIO_NUM, 1);
        } else if (edata->cross_type == ANA_CMPR_CROSS_NEG) {
            gpio_set_level(EXAMPLE_MONITOR_GPIO_NUM, 0);
        }
        return false;
    }

    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = 0,                            // 选择当前目标上有效的 unit 索引
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,  // 无特殊时序需求时优先使用默认时钟
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL, // 使用内部参考阈值
        .cross_type = ANA_CMPR_CROSS_ANY,     // 同时关注上穿和下穿
        .src_chan0_gpio = 0,                  // 需改成你板卡上实际接入的源通道 GPIO
    };
    ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));

    ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
    };
    ESP_ERROR_CHECK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));

    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 10,
    };
    ESP_ERROR_CHECK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));

    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = example_ana_cmpr_on_cross_callback,
    };
    ESP_ERROR_CHECK(ana_cmpr_register_event_callbacks(cmpr, &cbs, NULL));

    ESP_ERROR_CHECK(ana_cmpr_enable(cmpr));

如何理解这些配置
----------------

:cpp:type:`ana_cmpr_config_t` 决定了这个比较器实例的基本工作方式：

- :cpp:member:`ana_cmpr_config_t::unit` 选择使用哪个比较器单元。
- :cpp:member:`ana_cmpr_config_t::clk_src` 选择比较器的时钟源。对于大多数应用，:cpp:enumerator:`ANA_CMPR_CLK_SRC_DEFAULT` 是合适的起点。
- :cpp:member:`ana_cmpr_config_t::ref_src` 决定参考信号来自内部参考电压还是外部模拟输入。
- :cpp:member:`ana_cmpr_config_t::cross_type` 决定哪些跨越会触发回调：上穿、下穿，或者两者都触发。
- :cpp:member:`ana_cmpr_config_t::src_chan0_gpio` 在模拟比较器 PAD 可配置的目标上，用于指定源通道 GPIO。
- :cpp:member:`ana_cmpr_config_t::resample_limit` 表示在接受一次通道状态更新前，要求该 unit 连续获得若干次一致的采样结果。

当使用内部参考时，:cpp:func:`ana_cmpr_set_internal_reference` 会通过 :cpp:member:`ana_cmpr_internal_ref_config_t::ref_volt` 设置阈值。这个阈值并不是任意电压，而是 VDD 的固定百分比。例如 :cpp:enumerator:`ANA_CMPR_REF_VOLT_50_PCT_VDD` 表示判断源信号当前是否高于半个电源电压。

去抖与信号稳定性
----------------

真实模拟信号在阈值附近通常会有噪声。若不配置去抖，当信号缓慢越过阈值，或者在阈值附近来回抖动时，可能会在很短时间内触发多次跨越中断。

可以通过 :cpp:func:`ana_cmpr_set_debounce` 抑制这些重复跨越：

- :cpp:member:`ana_cmpr_debounce_config_t::wait_us` 表示在一次跨越被报告后，接下来有多长时间暂时忽略新的跨越中断。

经验上可以这样调：

- 如果信号噪声较大、重复中断太多，增大 ``wait_us``。
- 如果信号变化较快、开始漏掉真实跨越，减小 ``wait_us``。

回调行为
--------

使用 :cpp:func:`ana_cmpr_register_event_callbacks` 注册回调，并且应在使能比较器之前完成注册。

当前驱动提供的回调成员为：

- :cpp:member:`ana_cmpr_event_callbacks_t::on_cross`

回调参数中的 :cpp:type:`ana_cmpr_cross_event_data_t` 可以告诉你：

- :cpp:member:`ana_cmpr_cross_event_data_t::cross_type` - 本次是上穿还是下穿
- :cpp:member:`ana_cmpr_cross_event_data_t::src_chan_id` - 在支持多源通道的目标上，是哪个源通道触发了事件

在不支持独立上穿/下穿中断上报的目标上，:cpp:member:`ana_cmpr_cross_event_data_t::cross_type` 可能始终上报为 :cpp:enumerator:`ANA_CMPR_CROSS_ANY`。

由于回调运行在中断上下文中，因此应保持短小、避免阻塞。

.. note::

    如果启用了 :ref:`CONFIG_ANA_CMPR_ISR_CACHE_SAFE`，请确保回调函数和所访问的数据都位于内部 RAM。

场景二：使用外部参考信号比较两个模拟量
========================================

当参考电平不是一个固定的 VDD 百分比时，就更适合使用外部参考。

典型例子包括：

- 将传感器信号与电阻分压得到的参考电压进行比较
- 比较两个波形的大小关系
- 让参考信号来自其他模拟前端电路

这种模式下，比较器的源信号仍然是你的输入信号，而参考信号来自另一个模拟输入。

示例：外部参考输入
------------------

.. code:: c

    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = 0,                             // 选择当前目标上有效的 unit 索引
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,   // 一般先使用默认时钟
        .ref_src = ANA_CMPR_REF_SRC_EXTERNAL,  // 切换到外部参考模式
        .cross_type = ANA_CMPR_CROSS_ANY,      // 同时关注两种跨越方向
        .src_chan0_gpio = 0,                   // 源信号 GPIO，按实际板卡连接修改
        .ext_ref_gpio = 1,                     // 外部参考 GPIO，按实际板卡连接修改
    };
    ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));

    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 10,
    };
    ESP_ERROR_CHECK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));

    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = example_ana_cmpr_on_cross_callback,
    };
    ESP_ERROR_CHECK(ana_cmpr_register_event_callbacks(cmpr, &cbs, NULL));

    ESP_ERROR_CHECK(ana_cmpr_enable(cmpr));

与场景一相比，变化在哪里
------------------------

最主要的区别在于 :cpp:member:`ana_cmpr_config_t::ref_src`：

- :cpp:enumerator:`ANA_CMPR_REF_SRC_INTERNAL` 表示参考信号由比较器内部产生。
- :cpp:enumerator:`ANA_CMPR_REF_SRC_EXTERNAL` 表示参考信号来自外部模拟输入。

当使用外部参考时：

- 在模拟比较器 PAD 可配置的目标上，:cpp:member:`ana_cmpr_config_t::ext_ref_gpio` 就是必需配置项。
- 不再使用 :cpp:func:`ana_cmpr_set_internal_reference`。

如果需要确认当前实例实际使用了哪些 GPIO，可以通过 :cpp:func:`ana_cmpr_get_channel_gpio` 查询：

.. code:: c

    gpio_num_t src_gpio = -1;
    gpio_num_t ext_ref_gpio = -1;
    // 初始化后回读实际通道映射，便于确认配置是否符合预期。
    ESP_ERROR_CHECK(ana_cmpr_get_channel_gpio(cmpr, ANA_CMPR_SOURCE_CHAN, 0, &src_gpio));
    ESP_ERROR_CHECK(ana_cmpr_get_channel_gpio(cmpr, ANA_CMPR_EXT_REF_CHAN, 0, &ext_ref_gpio));

.. only:: SOC_ANA_CMPR_SUPPORT_ETM_SCAN

    场景三：用 ETM 构建低 CPU 占用的硬件信号通路
    ================================================

    当你希望硬件在信号跨越时直接作出反应，而不是每次都经过 CPU 处理时，可以使用这种方式。

    典型例子包括：

    - 以极低 CPU 占用把采样后的模拟输入转换成 GPIO 波形
    - 使用定时器周期性触发比较器扫描
    - 通过 ETM 将比较器跨越事件连接到其他外设

    示例：使用 GPTimer 和 ETM 做周期扫描
    ------------------------------------

    :example:`peripherals/analog_comparator/etm_periodic_scan` 示例构建了如下硬件链路：

    - GPTimer alarm event -> GPTimer enable-alarm task
    - GPTimer alarm event -> Analog comparator start task
    - Analog comparator positive cross event -> GPIO set task
    - Analog comparator negative cross event -> GPIO clear task

    也就是说，定时器负责调度扫描，比较器负责采样源信号，而跨越结果则通过 ETM 直接驱动监控 GPIO。

    比较器侧的关键配置如下：

    .. code:: c

        ana_cmpr_handle_t cmpr = NULL;
        ana_cmpr_config_t config = {
            .unit = 0,                              // 选择当前目标上有效的 unit 索引
            .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,    // 无特殊要求时使用默认时钟
            .ref_src = ANA_CMPR_REF_SRC_INTERNAL,   // 使用内部参考进行阈值比较
            .cross_type = ANA_CMPR_CROSS_ANY,       // ETM 示例通常需要同时处理上穿和下穿
            .src_chan0_gpio = 0,                    // 按板卡实际接线设置源通道 GPIO
            .resample_limit = 3,                    // 提高抗噪能力，减少抖动触发
        };
        ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));

        ana_cmpr_internal_ref_config_t ref_cfg = {
            .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
        };
        ESP_ERROR_CHECK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));

        ana_cmpr_scan_config_t scan_cfg = {
            .scan_mode = ANA_CMPR_SCAN_MODE_FULL,  // 每轮扫描覆盖所有已配置通道
            .poll_period_us = 2,                   // 根据响应速度和时序裕量调节
        };
        ESP_ERROR_CHECK(ana_cmpr_set_scan_config(cmpr, &scan_cfg));

        ana_cmpr_etm_task_config_t cmpr_task_cfg = {
            .task_type = ANA_CMPR_TASK_START,
        };
        ESP_ERROR_CHECK(ana_cmpr_new_etm_task(cmpr, &cmpr_task_cfg, &cmpr_start_task));

        ana_cmpr_etm_event_config_t pos_evt_cfg = {
            .event_type = ANA_CMPR_EVENT_POS_CROSS, // 上穿事件
            .src_chan_id = 0,                       // 本示例监听通道 0
        };
        ESP_ERROR_CHECK(ana_cmpr_new_etm_event(cmpr, &pos_evt_cfg, &cmpr_pos_evt));

        ana_cmpr_etm_event_config_t neg_evt_cfg = {
            .event_type = ANA_CMPR_EVENT_NEG_CROSS, // 下穿事件
            .src_chan_id = 0,                       // 与上穿事件使用同一通道
        };
        ESP_ERROR_CHECK(ana_cmpr_new_etm_event(cmpr, &neg_evt_cfg, &cmpr_neg_evt));

    如何理解扫描配置
    ----------------

    在支持源通道扫描的目标上，:cpp:func:`ana_cmpr_set_scan_config` 用来控制比较器如何轮询各个源通道：

    - :cpp:member:`ana_cmpr_scan_config_t::scan_mode` 选择扫描方式。
    - :cpp:member:`ana_cmpr_scan_config_t::poll_period_us` 设置通道切换之间的等待时间。

    这些参数在以下场景尤其重要：

    - 使用多个源通道
    - 输入信号噪声较大
    - 希望按固定周期采样，而不是每次变化都触发 CPU 中断

    实践上可以这样理解：

    - ``poll_period_us`` 越短，扫描响应越快，但时序越紧。

    ETM 事件与任务句柄
    ------------------

    模拟比较器通过两个 API 提供 ETM 句柄：

    - :cpp:func:`ana_cmpr_new_etm_event` 从比较器跨越创建事件句柄
    - :cpp:func:`ana_cmpr_new_etm_task` 创建可被其他 ETM 事件触发的任务句柄

    当前驱动中：

    - ETM 事件类型由 :cpp:member:`ana_cmpr_etm_event_config_t::event_type` 选择
    - 源通道由 :cpp:member:`ana_cmpr_etm_event_config_t::src_chan_id` 选择
    - 可选的通道延迟由 :cpp:member:`ana_cmpr_etm_event_config_t::event_delay` 配置
    - 任务类型由 :cpp:member:`ana_cmpr_etm_task_config_t::task_type` 选择

    如何把这些事件和任务连接到 ETM 通道，请参考 :doc:`ETM </api-reference/peripherals/etm>` 文档。

多源通道的使用
==============

有些目标的一个比较器单元支持多个源通道。如果你的应用需要让多个模拟输入共用同一个参考信号，就可以在比较器处于 init 状态时，额外配置这些源通道。

相关 API 包括：

- :cpp:func:`ana_cmpr_add_src_chan`
- :cpp:func:`ana_cmpr_remove_src_chan`
- :cpp:func:`ana_cmpr_set_src_chan_cross_type`

示例：

.. code:: c

    ana_cmpr_src_chan_config_t src_cfg = {
        .gpio_num = EXAMPLE_SRC_CHAN1_GPIO,
        .cross_type = ANA_CMPR_CROSS_ANY,
    };
    ESP_ERROR_CHECK(ana_cmpr_add_src_chan(cmpr, 1, &src_cfg));

使用多源通道时请注意：

- 只能在 :cpp:func:`ana_cmpr_enable` 之前添加或删除通道。
- 如果把所有已配置的源通道都移除了，调用 :cpp:func:`ana_cmpr_enable` 会返回 ``ESP_ERR_INVALID_STATE``。
- :cpp:func:`ana_cmpr_set_src_chan_cross_type` 仅在 ESP32-H2 上支持运行时调整。

运行期观测与时间辅助 API
========================

以下 API 属于通用运行期观测能力，不依赖“多源通道”场景本身：

- :cpp:func:`ana_cmpr_trigger_scan`：触发一次软件控制的扫描序列（仅在支持软件触发扫描的目标上可用）。
- :cpp:func:`ana_cmpr_get_output_level`：读取某个源通道最近一次采样得到的数字比较结果（仅在支持输出电平读取的目标上可用）。
- :cpp:func:`ana_cmpr_get_clock_resolution_hz`：获取 PAD_COMP_CLK 分辨率（Hz）。
- :cpp:func:`ana_cmpr_get_capture_timestamps`：读取最近/上一次跨越的时间戳（仅在支持捕获的目标上可用）。

当你需要在软件中读取当前比较状态时，使用 :cpp:func:`ana_cmpr_get_output_level`。当你需要把 PAD_COMP_CLK tick（例如捕获时间戳）换算为时间单位时，使用 :cpp:func:`ana_cmpr_get_clock_resolution_hz`。当你希望以软件控制节奏刷新比较结果时，使用 :cpp:func:`ana_cmpr_trigger_scan`；该 API 只能在 :cpp:func:`ana_cmpr_enable` 后调用，init 状态下会返回 ``ESP_ERR_INVALID_STATE``。

要获得有效的捕获时间戳，需要在创建 unit 时开启捕获定时器：

.. code:: c

    ana_cmpr_config_t config = {
        .unit = 0,                            // 选择当前目标上有效的 unit 索引
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,  // PAD_COMP_CLK 来源
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL, // 按实际捕获场景选择参考模式
        .cross_type = ANA_CMPR_CROSS_ANY,     // 默认同时关注上穿和下穿
        .src_chan0_gpio = 0,                  // 需与板卡源信号接线一致
        .en_capture_timer = true,             // 捕获时间戳 API 的前置条件
    };

随后可按如下方式把 tick 换算为时间：

.. code:: c

    uint32_t resolution_hz = 0;
    uint32_t current_ticks = 0;
    ESP_ERROR_CHECK(ana_cmpr_get_clock_resolution_hz(cmpr, &resolution_hz));
    ESP_ERROR_CHECK(ana_cmpr_get_capture_timestamps(cmpr, 0, &current_ticks, NULL));
    uint32_t time_us = (uint32_t)(((uint64_t)current_ticks * 1000000U) / resolution_hz);

实用配置建议
============

如何选择时钟源
--------------

比较器的时钟会影响采样行为。对于大多数应用，优先使用 :cpp:enumerator:`ANA_CMPR_CLK_SRC_DEFAULT`，除非你有明确的时钟需求。

还要注意，在某些目标上，模拟比较器的时钟源会和其他 GPIO 扩展类外设共享。如果不同外设请求了不兼容的时钟源，初始化可能会失败。

如何选择内部参考电平
--------------------

内部参考电平本质上是 VDD 的固定百分比，而不是任意“多少伏”。

它更适合以下情况：

- 输入信号天然围绕供电范围变化
- 你只需要一个简单的中点或供电相关阈值

它不太适合以下情况：

- 你需要一个与 VDD 变化无关的精确阈值
- 你希望阈值连续可调，而不是固定档位

如何调节去抖
------------

去抖通常是上板后最先需要调的参数之一。

- 去抖太小：会出现重复中断，或者在阈值附近结果不稳定
- 去抖太大：会漏掉快速变化信号的真实跨越

没有一个参数适合所有场景。建议从较小值开始，根据实际信号表现逐步调整。

如何调节扫描时序
----------------

在支持扫描的目标上，应把扫描时序和重采样阈值一起调：

- 想要更快更新结果，可以减小 ``poll_period_us``
- 想要更强抗噪能力，可以增大 :cpp:member:`ana_cmpr_config_t::resample_limit`
- 不要把扫描速度设置得远慢于你真正想检测的输入变化速度

运行相关说明
============

使能与禁用的生命周期
--------------------

比较器的典型生命周期很简单：

1. 创建并完成配置
2. 使能比较器
3. 运行应用
4. 删除前先禁用比较器

请把 :cpp:func:`ana_cmpr_enable` 和 :cpp:func:`ana_cmpr_disable` 成对使用。

一旦比较器进入使能状态，就只能修改少量运行期控制项。像源通道管理、扫描配置这类 API，都必须在使能前完成。

电源管理
--------

当启用 :ref:`CONFIG_PM_ENABLE` 后，睡眠和时钟切换可能会影响比较器行为。驱动会在需要时自动持有电源管理锁，因此使能比较器后，系统可能会被阻止进入 light sleep。

如果你的应用比较关注功耗，建议只在确实需要时才使能比较器。

IRAM 安全
---------

如果你希望在 cache 被禁用时，比较器中断仍能正常工作，请启用 :ref:`CONFIG_ANA_CMPR_ISR_CACHE_SAFE`。

如果还希望相关控制函数在这种情况下也可调用，请启用 :ref:`CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM`。

以下控制 API 可以放入 IRAM：

- :cpp:func:`ana_cmpr_set_internal_reference`
- :cpp:func:`ana_cmpr_set_debounce`
- :cpp:func:`ana_cmpr_set_src_chan_cross_type`
- :cpp:func:`ana_cmpr_trigger_scan`

线程安全
--------

驱动通过以下机制保证同一个 :cpp:type:`ana_cmpr_handle_t` 的并发 API 调用是线程安全的：

- 每个 handle 的 FSM（INIT/ENABLE/WAIT）配合原子状态切换，用于串行化不兼容操作
- 短临界区保护共享软件状态与硬件寄存器，确保更新原子可见
- create/delete 期间的全局 slot 生命周期保护

仍需遵循生命周期约束：删除前先调用 :cpp:func:`ana_cmpr_disable`，并在 :cpp:func:`ana_cmpr_del_unit` 前确保无活动依赖对象（例如 ETM 句柄）。

Kconfig 选项
------------

最常用的 Kconfig 选项有：

- :ref:`CONFIG_ANA_CMPR_ISR_CACHE_SAFE`
    让默认 ISR 路径在 cache 关闭时仍可工作。如果你的比较器中断需要覆盖 cache-off 场景，应启用此项。
- :ref:`CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM`
    将可在 ISR 中调用的运行期控制 API 放入 IRAM，保证这些 API 在 cache 关闭时仍可调用。
- :ref:`CONFIG_ANA_CMPR_ENABLE_DEBUG_LOG`
    打开驱动调试日志，便于联调与问题定位。启用后会增加固件体积和日志输出量。

示例
====

.. list::

    :SOC_ANA_CMPR_SUPPORT_AUTO_SCAN: - :example:`peripherals/analog_comparator/auto_scan` 展示了基于自动扫描功能的阈值检测。比较器使能后硬件持续扫描并实时更新输出，监控 GPIO 根据目标能力通过中断或 ETM 驱动。
    :SOC_ANA_CMPR_SUPPORT_ETM_SCAN: - :example:`peripherals/analog_comparator/etm_periodic_scan` 展示了如何使用 GPTimer 和 ETM 周期性触发比较器扫描，并通过比较器跨越事件驱动监控 GPIO。

API 参考
========

Driver APIs
-----------

.. include-build-file:: inc/ana_cmpr.inc

Driver Types
------------

.. include-build-file:: inc/ana_cmpr_types.inc
