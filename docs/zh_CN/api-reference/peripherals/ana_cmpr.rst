模拟比较器
==========

:link_to_translation:`en:[English]`

{IDF_TARGET_ANA_CMPR_SRC_CHAN0: default="未更新", esp32h2="GPIO11", esp32p4="GPIO52"}
{IDF_TARGET_ANA_CMPR_EXT_REF_CHAN0: default="未更新", esp32h2="GPIO10", esp32p4="GPIO51"}
{IDF_TARGET_ANA_CMPR_SRC_CHAN1: default="未更新", esp32p4="GPIO54"}
{IDF_TARGET_ANA_CMPR_EXT_REF_CHAN1: default="未更新", esp32p4="GPIO53"}

简介
----

模拟比较器是一种用于比较信号源与内部参考电压或外部参考信号的外设。

当用于比较模拟信号时，集成模拟比较器可以低成本替代运算放大器。不同于运算放大器的连续比较，ESP 模拟比较器由时钟源驱动，其采样频率取决于时钟的频率。

{IDF_TARGET_NAME} 上的模拟比较器有 {IDF_TARGET_SOC_ANA_CMPR_NUM} 个单元，单元中的通道如下：

**UNIT0**

- 源通道：{IDF_TARGET_ANA_CMPR_SRC_CHAN0}
- 外部参考通道：{IDF_TARGET_ANA_CMPR_EXT_REF_CHAN0}
- 内部参考通道：电压范围是 VDD 的 0% ~ 70%，步长为 VDD 的 10%

.. only:: esp32p4

    **UNIT1**

    - 源通道：{IDF_TARGET_ANA_CMPR_SRC_CHAN1}
    - 外部参考通道：{IDF_TARGET_ANA_CMPR_EXT_REF_CHAN1}
    - 内部参考通道：电压范围是 VDD 的 0% ~ 70%，步长为 VDD 的 10%

功能概述
--------

本文中的以下章节涵盖了安装及操作模拟比较器单元的基本步骤：

- :ref:`anacmpr-resource-allocation` - 涵盖了应设置哪些参数以获取单元句柄，以及完成工作后如何回收资源。
- :ref:`anacmpr-further-configurations` - 涵盖了可能需要指定的其他配置及其用途。
- :ref:`anacmpr-enable-and-disable-unit` - 涵盖了如何启用和禁用单元。
- :ref:`anacmpr-power-management` - 描述了不同时钟源对功耗的影响。
- :ref:`anacmpr-iram-safe` - 列出了在 cache 被禁用时也能起效的函数。
- :ref:`anacmpr-thread-safety` - 列出了驱动程序中线程安全的 API。
- :ref:`anacmpr-kconfig-options` - 列出了支持的 Kconfig 选项，这些选项可以对驱动程序产生不同影响。

.. only:: SOC_ANA_CMPR_SUPPORT_ETM

    - :ref:`anacmpr-etm-events` - 介绍了如何创建一个模拟比较器跨越事件。

.. _anacmpr-resource-allocation:

资源分配
^^^^^^^^

模拟比较器单元由 :cpp:type:`ana_cmpr_handle_t` 表示，每个单元都可支持内部或外部的参考信号。

通过调用 :cpp:func:`ana_cmpr_new_unit` 来获取单元句柄以分配资源。在分配单元时需要指定 :cpp:type:`ana_cmpr_config_t` 配置：

- :cpp:member:`ana_cmpr_config_t::unit` 选择模拟比较器单元。
- :cpp:member:`ana_cmpr_config_t::clk_src` 选择模拟比较器的时钟源，这将影响采样频率。请注意，模拟比较器的时钟源来自 IO MUX，与 Sigma-Delta 调制器 (SDM) 和毛刺过滤器 (Glitch Filter) 等 GPIO 扩展外设共享时钟源。如果为多个 GPIO 扩展外设指定不同的时钟源，则配置将失败。这些外设的默认时钟源是相同的，通常选择 :cpp:enumerator:`soc_periph_ana_cmpr_clk_src_t::ANA_CMPR_CLK_SRC_DEFAULT` 即可。
- :cpp:member:`ana_cmpr_config_t::ref_src` 选择内部参考电压或外部参考信号为参考信号源。
- :cpp:member:`ana_cmpr_config_t::cross_type` 选择哪种类型的跨零信号可以触发中断。

函数 :cpp:func:`ana_cmpr_new_unit` 可能因内存不足、参数无效等各种错误而失败。如果不再需要先前创建的模拟比较器单元，可通过调用 :cpp:func:`ana_cmpr_del_unit` 来回收资源，从而释放底层硬件通道的资源供其他用途。在删除模拟比较器单元句柄之前，首先应通过 :cpp:func:`ana_cmpr_disable` 禁用句柄，或者确保尚未通过 :cpp:func:`ana_cmpr_enable` 启用该单元。

.. code:: c

    #include "driver/ana_cmpr.h"

    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = 0,
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL,
        .cross_type = ANA_CMPR_CROSS_ANY,
    };
    ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));
    // ...
    ESP_ERROR_CHECK(ana_cmpr_del_unit(cmpr));

.. _anacmpr-further-configurations:

进一步配置
^^^^^^^^^^

- :cpp:func:`ana_cmpr_set_internal_reference` - 选择 :cpp:enumerator:`ana_cmpr_ref_source_t::ANA_CMPR_REF_SRC_INTERNAL` 作为参考源时，该函数可以指定内部参考电压。

需要由 :cpp:member:`ana_cmpr_internal_ref_config_t::ref_volt` 来指定电压。该电压受 VDD 电源电压限制，只能支持固定百分比的 VDD。目前在 {IDF_TARGET_NAME} 上，内部参考电压范围是 VDD 的 0% ~ 70%，步长为 VDD 的 10%。

.. code:: c

    #include "driver/ana_cmpr.h"

    ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
    };
    ESP_ERROR_CHECK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));

- :cpp:func:`ana_cmpr_set_debounce` - 设置去抖配置。

通过 :cpp:member:`ana_cmpr_debounce_config_t::wait_us` 可设置中断等待时间。跨零中断触发后，中断将暂时禁用 :cpp:member:`ana_cmpr_debounce_config_t::wait_us` 微秒，这样可以避免信号源跨越参考信号时频繁触发中断。因此，等待时间应与信号源和参考信号之间的相对频率成反比。如果中断等待时间设置得太短，则无法完全避免抖动，但如果设置得太长，则可能会错过下一个跨越中断。

.. code:: c

    #include "driver/ana_cmpr.h"

    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 1,
    };
    ESP_ERROR_CHECK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));

- :cpp:func:`ana_cmpr_set_cross_type` - 设置信号源跨越类型。

初始跨越类型在 :cpp:func:`ana_cmpr_new_unit` 中设置。即便在中断服务程序 (ISR) 的上下文中，此函数也可以更新跨越类型。

.. code:: c

    #include "driver/ana_cmpr.h"

    ESP_ERROR_CHECK(ana_cmpr_set_cross_type(cmpr, ANA_CMPR_CROSS_POS));

- :cpp:func:`ana_cmpr_register_event_callbacks` - 注册回调函数。

目前支持 :cpp:member:`ana_cmpr_event_callbacks_t::on_cross`。当发生跨越事件（由 :cpp:member:`ana_cmpr_config_t::cross_type` 指定）时，将调用该回调函数。

.. code:: c

    #include "driver/ana_cmpr.h"

    static bool IRAM_ATTR example_ana_cmpr_on_cross_callback(ana_cmpr_handle_t cmpr,
                                                         const ana_cmpr_cross_event_data_t *edata,
                                                         void *user_ctx)
    {
        // ...
        return false;
    }
    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = example_ana_cmpr_on_cross_callback,
    };
    ESP_ERROR_CHECK(ana_cmpr_register_event_callbacks(cmpr, &cbs, NULL));

.. note::

    当启用 :ref:`CONFIG_ANA_CMPR_ISR_IRAM_SAFE` 时，应添加属性 ``IRAM_ATTR``，确保回调上下文和涉及的数据位于内部 RAM 中（详情请参阅 :ref:`anacmpr-iram-safe`）。

.. _anacmpr-enable-and-disable-unit:

启用和禁用单元
^^^^^^^^^^^^^^^^

- :cpp:func:`ana_cmpr_enable` - 启用模拟比较器单元。
- :cpp:func:`ana_cmpr_disable` - 禁用模拟比较器单元。

启用模拟比较器单元与跨越事件中断后，若同时启用了电源管理（详见 :ref:`anacmpr-power-management`），则将获得电源管理锁。在 **启用** 状态下，只能调用 :cpp:func:`ana_cmpr_set_internal_reference` 和 :cpp:func:`ana_cmpr_set_debounce`，其他函数可在禁用单元后调用。

调用 :cpp:func:`ana_cmpr_disable` 则会执行与上述过程相反的操作。

.. _anacmpr-power-management:

电源管理
^^^^^^^^

当启用电源管理时（即开启 :ref:`CONFIG_PM_ENABLE`），系统会在进入 Light-sleep 模式前调整 APB 频率，因此模拟比较器的分辨率也可能随之更改。

通过获取类型为 :cpp:enumerator:`ESP_PM_NO_LIGHT_SLEEP` 的电源管理锁，驱动程序可以防止系统更改 APB 频率。只要驱动程序创建的模拟比较器单元实例选择 :cpp:enumerator:`ANA_CMPR_CLK_SRC_DEFAULT` 或 :cpp:enumerator:`ANA_CMPR_CLK_SRC_XTAL` 作为其时钟源，驱动程序会确保通过 :cpp:func:`ana_cmpr_enable` 启用通道，并获取电源管理锁。同理，当为该通道调用 :cpp:func:`ana_cmpr_disable` 时，驱动程序会释放锁。

.. _anacmpr-iram-safe:

IRAM 安全
^^^^^^^^^

默认情况下，当 cache 因写入或擦除 flash 等原因而被禁用时，模拟比较器的中断服务将会延迟，造成警报中断无法及时执行。在实时应用程序中通常需要避免这一情况发生。

Kconfig 选项 :ref:`CONFIG_ANA_CMPR_ISR_IRAM_SAFE` 支持：

1. 即使 cache 被禁用也能启用中断服务。
2. 将 ISR 使用的所有函数放入 IRAM。 [1]_
3. 将驱动程序对象放入 DRAM（以防它被分配到 PSRAM上）。

启用上述 Kconfig 选项以保证在禁用 cache 时可以正常使用函数，但这会增加 IRAM 消耗。

另一个 Kconfig 选项 :ref:`CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM` 也支持将常用的 IO 控制函数放入 IRAM 中，以保证在禁用 cache 时可以正常使用函数。IO 控制函数如下所示：

- :cpp:func:`ana_cmpr_set_internal_reference`
- :cpp:func:`ana_cmpr_set_debounce`
- :cpp:func:`ana_cmpr_set_cross_type`

.. _anacmpr-thread-safety:

线程安全
^^^^^^^^

驱动程序会确保工厂函数 :cpp:func:`ana_cmpr_new_unit` 的线程安全。使用时，可以直接从不同的 RTOS 任务中调用此类函数，无需额外锁保护。

驱动程序设置了临界区，以防函数同时在任务和 ISR 中被调用。因此，以下函数支持在 ISR 上下文运行：

- :cpp:func:`ana_cmpr_set_internal_reference`
- :cpp:func:`ana_cmpr_set_debounce`
- :cpp:func:`ana_cmpr_set_cross_type`

其他以 :cpp:type:`ana_cmpr_handle_t` 为第一个位置参数的函数均非线程安全，因此应避免从多个任务中调用这类函数。

.. _anacmpr-kconfig-options:

Kconfig 选项
^^^^^^^^^^^^

- :ref:`CONFIG_ANA_CMPR_ISR_IRAM_SAFE` 控制默认的 ISR 句柄在 cache 被禁用时是否可以正常工作，详见 :ref:`anacmpr-iram-safe`。
- :ref:`CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM` 控制模拟比较器控制函数的存放位置（IRAM 或 flash），详见 :ref:`anacmpr-iram-safe`。
- :ref:`CONFIG_ANA_CMPR_ENABLE_DEBUG_LOG` 用于启用调试日志输出。启用此选项将增加固件的二进制文件大小。

.. only:: SOC_ANA_CMPR_SUPPORT_ETM

    .. _anacmpr-etm-events:

    ETM 事件
    ^^^^^^^^

    创建一个模拟比较器跨越事件，需要额外包含头文件 ``driver/ana_cmpr_etm.h``，并调用函数 :cpp:func:`ana_cmpr_new_etm_event` 来分配事件。有关如何将事件连接到任务，请参考 :doc:`ETM </api-reference/peripherals/etm>`。

应用示例
--------

* :example:`peripherals/analog_comparator` 展示了模拟比较器的基本用法以及其他用途（如迟滞比较器和 SPWM 发生器）。

API 参考
--------

.. include-build-file:: inc/ana_cmpr.inc
.. include-build-file:: inc/ana_cmpr_types.inc

.. [1]
   :cpp:member:`ana_cmpr_event_callbacks_t::on_cross` 回调函数，以及由其调用的其他函数也应放置在 IRAM 中，请妥善处理。
