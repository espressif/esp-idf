Sigma-Delta 调制器 (SDM)
===========================

:link_to_translation:`en:[English]`

简介
----

{IDF_TARGET_NAME} 具备二阶 Sigma-Delta 调制器，可以为多个通道生成独立的脉冲密度调制 (PDM) 脉冲。请参阅技术参考手册，查看可用的硬件通道数量。[1]_

Sigma-Delta 调制器可以将模拟电压信号转换为脉冲频率或脉冲密度，该过程称为脉冲密度调制 (PDM)（请参阅 |wiki_ref|_）。

与 I2S 外设中的 PDM 模式和和数模转换器 (DAC) 相比，SDM 中的 PDM 主要有以下特点：

1. SDM 没有时钟信号，类似于 PDM 的 DAC 模式；
2. SDM 没有 DMA 支持，无法持续改变其输出密度。如果需要改变 SDM 的输出密度，可以在定时器的回调函数中进行操作；
3. 基于以上两点，不同于 DAC，要还原模拟波形，还必须使用外部的有源或无源低通滤波器，详情请参阅 :ref:`convert_to_analog_signal`。

Sigma-Delta 调制通道通常应用于以下场景：

-  LED 调光
-  使用有源 RC 低通滤波器，实现简单的数模转换（8 位分辨率）
-  结合半桥或全桥回路，以及 LC 低通滤波器，实现 D 级功率放大

功能概述
--------

下文将分节概述安装和操作 SDM 通道的一般步骤：

- :ref:`sdm-resource-allocation` - 介绍如何初始化和配置 SDM 通道，以及在通道完成任务后如何回收相关资源。
- :ref:`sdm-enable-and-disable-channel` - 介绍如何启用和禁用 SDM 通道。
- :ref:`sdm-set-equivalent-duty-cycle` - 介绍如何设置 PDM 脉冲的等效占空比。
- :ref:`sdm-power-management` - 介绍不同时钟源对功耗的影响。
- :ref:`sdm-iram-safe` - 介绍禁用 cache 时仍可使用的功能。
- :ref:`sdm-thread-safety` - 介绍由驱动程序认证为线程安全的 API。
- :ref:`sdm-kconfig-options` - 介绍 SDM 驱动程序支持的各种 Kconfig 选项，这些选项可以给驱动程序的行为造成不同影响。

.. _sdm-resource-allocation:

资源分配
^^^^^^^^

在 ESP-IDF 中，SDM 通道的信息和属性通过特定的数据结构进行管理和访问，该数据结构表示为 :cpp:type:`sdm_channel_handle_t`。每个通道都可以输出由硬件生成的二进制信号，且这些信号都经过 Sigma-Delta 调制。所有可用通道均存放在资源池中，由驱动程序管理，无需手动将固定通道分配给 GPIO。

要安装 SDM 通道，请调用 :cpp:func:`sdm_new_channel` 获取通道句柄。通道的具体配置信息由结构体 :cpp:type:`sdm_config_t` 传递。

- :cpp:member:`sdm_config_t::gpio_num` 设置 PDM 脉冲输出的 GPIO 管脚号。
- :cpp:member:`sdm_config_t::clk_src` 选择 SDM 模块的时钟源。注意，所有通道选择的时钟源应保持一致。
- :cpp:member:`sdm_config_t::sample_rate_hz` 设置 SDM 模块的采样率。
- :cpp:member:`sdm_config_t::invert_out` 设置是否反转输出信号。
- :cpp:member:`sdm_config_t::io_loop_back` 通过 GPIO 矩阵外设，启用 GPIO 的输入和输出功能。注意，该字段仅供调试使用。

函数 :cpp:func:`sdm_new_channel` 可能因为各种原因失败，如内存不足、参数无效等。当缺少空闲通道（即所有的硬件 SDM 通道均在使用中）时，将返回 :c:macro:`ESP_ERR_NOT_FOUND`。

SDM 通道完成任务后，请调用 :cpp:func:`sdm_del_channel` 回收相应资源，以便底层硬件通道用于其他目的。在删除 SDM 通道句柄前，请通过 :cpp:func:`sdm_channel_disable` 禁用要删除的通道，或确保该通道尚未由 :cpp:func:`sdm_channel_enable` 启用，再继续删除操作。

创建采样率为 1 MHz 的 SDM 通道
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    sdm_channel_handle_t chan = NULL;
    sdm_config_t config = {
        .clk_src = SDM_CLK_SRC_DEFAULT,
        .sample_rate_hz = 1 * 1000 * 1000,
        .gpio_num = 0,
    };
   ESP_ERROR_CHECK(sdm_new_channel(&config, &chan));

.. _sdm-enable-and-disable-channel:

启用和禁用通道
^^^^^^^^^^^^^^

在对 SDM 通道进行进一步的 IO 控制之前，需要先调用 :cpp:func:`sdm_channel_enable` 启用通道。在内部，该函数实现了以下操作：

* 将通道状态从 **init** 切换到 **enable**
* 如果选择了特定时钟源（如 APB 锁），则会获取合适的电源管理锁。要了解更多有关信息，请参阅 :ref:`sdm-power-management`。

调用 :cpp:func:`sdm_channel_disable` 则执行相反操作，即将通道恢复到 **init** 状态，并释放电源管理锁。

.. _sdm-set-equivalent-duty-cycle:

设置脉冲密度
^^^^^^^^^^^^

在 PDM 中，脉冲密度决定了低通滤波器转换后的输出模拟电压，该模拟电压可以通过公式 ``Vout = VDD_IO / 256 * duty + VDD_IO / 2`` 计算。使用函数 :cpp:func:`sdm_channel_set_pulse_density` 时，需要传入一个名为 ``density`` 的参数。这个参数是一个整数值，范围在 -128 到 127 之间，表示一个 8 位有符号整数。根据 ``density`` 参数的不同取值，输出信号的占空比也会相应改变。例如，如果将 ``density`` 参数设置为零，输出信号的占空比约为 50%。

.. _sdm-power-management:

电源管理
^^^^^^^^

启用电源管理（即启用 :ref:`CONFIG_PM_ENABLE`）时，在进入 Light-sleep 模式前，系统会调整 APB 频率，这可能会改变 Sigma-Delta 调制器的采样率。

但是，通过获取类型为 :cpp:enumerator:`ESP_PM_APB_FREQ_MAX` 的电源管理锁，驱动程序可以防止系统改变 APB 频率。每当驱动程序创建 SDM 通道，且该通道选择 :cpp:enumerator:`SDM_CLK_SRC_APB` 作为其时钟源时，在通过 :cpp:func:`sdm_channel_enable` 启用通道的过程中，驱动程序会确保获取类型为 :cpp:enumerator:`ESP_PM_APB_FREQ_MAX` 的电源管理锁。反之，调用 :cpp:func:`sdm_channel_disable` 禁用通道时，驱动程序释放该锁。

.. _sdm-iram-safe:

IRAM 安全
^^^^^^^^^

Kconfig 选项 :ref:`CONFIG_SDM_CTRL_FUNC_IN_IRAM` 支持将常用的 IO 控制函数存放在 IRAM 中，以保证在禁用 cache 时可以正常使用函数。IO 控制函数如下所示：

- :cpp:func:`sdm_channel_set_pulse_density`

.. _sdm-thread-safety:

线程安全
^^^^^^^^

驱动程序会确保工厂函数 :cpp:func:`sdm_new_channel` 的线程安全，使用时，可以直接从不同的 RTOS 任务中调用此类函数，无需额外锁保护。

驱动程序设置了临界区，以防函数同时在任务和 ISR 中调用。因此，以下函数支持在 ISR 上下文运行：

- :cpp:func:`sdm_channel_set_pulse_density`

其他以 :cpp:type:`sdm_channel_handle_t` 作为第一个位置参数的函数均非线程安全，因此应避免从多个任务中调用这类函数。

.. _sdm-kconfig-options:

Kconfig 选项
^^^^^^^^^^^^

- :ref:`CONFIG_SDM_CTRL_FUNC_IN_IRAM` 控制 SDM 通道控制函数的存放位置（IRAM 或 flash）。更多信息请参阅 :ref:`sdm-iram-safe`。
- :ref:`CONFIG_SDM_ENABLE_DEBUG_LOG` 用于启用调试日志输出。启用此选项将增加固件的二进制文件大小。

.. _convert_to_analog_signal:

转换为模拟信号（可选）
----------------------

一般而言，Sigma-Delta 信号连接到 LED 用来调节明暗时，无需在信号和 LED 之间添加滤波器，因为人眼本身对光强变化有低通滤波作用。但是，如果你想测量实际电压，或观察模拟波形，就需要设计一个模拟低通滤波器。此外，建议使用有源滤波器，相较于无源滤波器，有源滤波器在处理信号时具有更强的抗干扰性，且损失的电压较少。

请参阅如下示例 `Sallen-Key 拓扑低通滤波器`_，了解滤波器的相关知识。

.. figure:: ../../../_static/typical_sallenkey_LP_filter.png
    :align: center
    :alt: Sallen-Key 拓扑低通滤波器
    :figclass: align-center

    Sallen-Key 拓扑低通滤波器


应用示例
--------

* 使用 Sigma-Delta 调制的 100 Hz 正弦波：:example:`peripherals/sigma_delta/sdm_dac`。
* 使用 Sigma-Delta 调制、并由 GPIO 驱动的 LED：:example:`peripherals/sigma_delta/sdm_led`。

API 参考
--------

.. include-build-file:: inc/sdm.inc
.. include-build-file:: inc/sdm_types.inc

.. [1]
   不同的 ESP 芯片系列可能具有不同数量的 SDM 通道，请参阅 {IDF_TARGET_NAME} 技术参考手册中的 `GPIO 和 IOMUX <{IDF_TARGET_TRM_EN_URL}#iomuxgpio>`__ 章节，了解更多详情。驱动程序对通道申请数量不做限制，但当硬件资源用尽时，驱动程序将返回错误。因此，每次进行通道分配（如调用 :cpp:func:`sdm_new_channel`）时，请注意检查返回值。

.. _Sallen-Key 拓扑低通滤波器: https://en.wikipedia.org/wiki/Sallen%E2%80%93Key_topology

.. |wiki_ref| replace:: 维基百科上有关 Sigma-Delta 调制的介绍
.. _wiki_ref: https://en.wikipedia.org/wiki/Delta-sigma_modulation
