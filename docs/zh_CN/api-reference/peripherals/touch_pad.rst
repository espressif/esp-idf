触摸传感器
============

:link_to_translation:`en:[English]`

概述
------------

触摸传感器系统由保护覆盖层、触摸电极、绝缘基板和走线组成，保护覆盖层位于最上层，绝缘基板上设有电极及走线。用户触摸覆盖层将产生电容变化，根据电容变化判断此次触摸是否为有效触摸行为。

ESP32 可支持最多 10 个电容式触摸板/GPIO，触摸板可以以矩阵或滑条等方式组合使用，从而覆盖更大触感区域及更多触感点。触摸传感由有限状态机 (FSM) 硬件控制，由软件或专用硬件计时器发起。

如需了解触摸传感器设计、操作及其控制寄存器等相关信息，请参考《`ESP32 技术参考手册 <https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_cn.pdf>`_》(PDF)，您也可以在《ESP32 技术参考手册》中查看这一子系统是如何运行的。

请参考 `触摸传感器应用方案简介 <https://github.com/espressif/esp-iot-solution/blob/master/documents/touch_pad_solution/touch_sensor_design_cn.md>`_，查看触摸传感器设计详情和固件开发指南。如果不想亲自在多种配置环境下测试触摸传感器，请查看 `ESP32 触摸功能开发套件 <https://github.com/espressif/esp-iot-solution/blob/master/documents/evaluation_boards/esp32_sense_kit_guide_cn.md>`_。

功能介绍
----------------------

下面将 API 分解成几个函数组进行介绍，帮助您快速了解以下功能：

- 初始化触摸板驱动程序
- 配置触摸板 GPIO 管脚
- 触摸状态测量
- 调整测量参数（优化测量）
- 过滤触摸测量
- 触摸监测方式
- 设置中断信号监测触碰动作
- 中断触发

请前往 :ref:`touch_pad-api-reference` 章节，查看某一函数的具体描述。:ref:`touch_pad-api-examples` 章节则介绍了此 API 的具体实现。


初始化触摸板驱动程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

使用触摸板之前，需要先调用 :cpp:func:`touch_pad_init` 函数初始化触摸板驱动程序。此函数设置了 :ref:`touch_pad-api-reference` 项下的 *Macros* 中列出的几项 ``.._DEFAULT`` 驱动程序参数，同时删除之前设置过的触摸板信息（如有），并禁用中断。

如果不再需要该驱动程序，可以调用 :cpp:func:`touch_pad_deinit` 释放已初始化的驱动程序。

配置触摸板 GPIO 管脚
^^^^^^^^^^^^^^^^^^^^^^^^^^

调用 :cpp:func:`touch_pad_config` 使能某一 GPIO 的触感功能。

使用 :cpp:func:`touch_pad_set_fsm_mode` 选择触摸板测量（由 FSM 操作）是由硬件计时器自动启动，还是由软件自动启动。如果选择软件模式，请使用 :cpp:func:`touch_pad_sw_start` 启动 FSM。

触摸状态测量
^^^^^^^^^^^^^^^^^^^^^^^^

借助以下两个函数从传感器读取原始数据或过滤后的数据：

* :cpp:func:`touch_pad_read`
* :cpp:func:`touch_pad_read_filtered`

这两个函数也可以用于检查触碰和释放触摸板时传感器读数变化范围，来评估触摸板设计，然后根据这些信息设定触摸阈值。

.. note::

    使用 :cpp:func:`touch_pad_read_filtered` 之前，需要先调用 `过滤触摸测量`_ 中特定的滤波器函数初始化并配置该滤波器。

请参考应用示例 :example:`peripherals/touch_pad_read`，查看如何使用这两个读值函数。

优化测量
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

触摸传感器设有数个可配置参数，以适应触摸板设计特点。例如，如果需要感知较细微的电容变化，则可以缩小触摸板充放电的参考电压范围。您可以使用 :cpp:func:`touch_pad_set_voltage` 函数设置电压参考低值和参考高值。

优化测量除了可以识别细微的电容变化之外，还可以降低应用程序功耗，但可能会增加测量噪声干扰。如果得到的动态读数范围结果比较理想，则可以调用 :cpp:func:`touch_pad_set_meas_time` 函数来减少测量时间，从而进一步降低功耗。

可用的测量参数及相应的 'set' 函数总结如下：

* 触摸板充放电参数：

    * 电压门限：:cpp:func:`touch_pad_set_voltage`
    * 速率（斜率） :cpp:func:`touch_pad_set_cnt_mode`

* 测量时间：:cpp:func:`touch_pad_set_meas_time`

电压门限（参考低值/参考高值）、速率（斜率）与测量时间的关系如下图所示：

.. figure:: ../../../_static/touch_pad-measurement-parameters.jpg
    :align: center
    :alt: Touch Pad - relationship between measurement parameters 
    :figclass: align-center

    触摸板 - 测量参数之间的关系

上图中的 *Output* 代表触摸传感器读值，即一个测量周期内测得的脉冲计数值。

所有函数均成对出现，用于设定某一特定参数，并获取当前参数值。例如：:cpp:func:`touch_pad_set_voltage` 和 :cpp:func:`touch_pad_get_voltage`。

.. _touch_pad-api-filtering-of-measurements:

过滤触摸测量
^^^^^^^^^^^^^^^^^^^^^^^^^

如果测量中存在噪声，可以使用提供的 API 函数对测量进行过滤。使用滤波器之前，请先调用 :cpp:func:`touch_pad_filter_start` 启动该滤波器。

滤波器类型为 IIR（无限脉冲响应滤波器），您可以调用 :cpp:func:`touch_pad_set_filter_period` 配置此类滤波器的采样周期。

如需停止滤波器，请调用 :cpp:func:`touch_pad_filter_stop` 函数。如果不再使用该滤波器，请调用 :cpp:func:`touch_pad_filter_delete` 删除此滤波器。

触摸监测
^^^^^^^^^^^^^^^

触摸监测基于用户配置的阈值和 FSM 执行的原始测量，并由 ESP32 硬件实现。你可以调用 :cpp:func:`touch_pad_get_status` 查看被触碰的触摸板，或调用 :cpp:func:`touch_pad_clear_status` 清除触摸状态信息。

您也可以将硬件触摸监测连接至中断，详细介绍见下一章节。

如果测量中存在噪声，且电容变化幅度较小，硬件触摸监测结果可能就不太理想。如需解决这一问题，不建议使用硬件监测或中断信号，建议您在自己的应用程序中采用测量过滤，并执行触摸监测。请参考 :example:`peripherals/touch_pad_interrupt`，查看以上两种触摸监测的实现方式。

中断触发
^^^^^^^^^^^^^^^^^^^^^^^^^^

在对触摸监测启用中断之前，请先设置一个触摸监测阈值。然后使用 `触摸状态测量`_ 中所述的函数读取并显示触摸和释放触摸板时测得的结果。如果测量中存在噪声且相对电容变化较小，请使用滤波器。您也可以根据应用程序和环境条件，测试温度和电源电压变化对测量值的影响。

确定监测阈值后就可以在初始化时调用 :cpp:func:`touch_pad_config` 设置此阈值，或在运行时调用 :cpp:func:`touch_pad_set_thresh` 设置此阈值。 

下一步就是设置如何触发中断。您可以设置在阈值以下或以上触发中断，具体触发模式由函数 :cpp:func:`touch_pad_set_trigger_mode` 设置。

最后您可以使用以下函数配置和管理中断调用：

* :cpp:func:`touch_pad_isr_register` / :cpp:func:`touch_pad_isr_deregister`
* :cpp:func:`touch_pad_intr_enable` / :cpp:func:`touch_pad_intr_disable`

中断配置完成后，您可以调用 :cpp:func:`touch_pad_get_status` 查看中断信号来自哪个触摸板，也可以调用 :cpp:func:`touch_pad_clear_status` 清除触摸板状态信息。

.. note::

    触摸监测中的中断信号基于原始/未经过滤的测量值（对比用户设置的阈值），并在硬件中实现。启用软件滤波 API 并不会影响这一过程，见 :ref:`touch_pad-api-filtering-of-measurements`。



从睡眠模式唤醒
^^^^^^^^^^^^^^^^^^^^^^

如果使用触摸板中断将芯片从睡眠模式唤醒，您可以选择配置一些触摸板，例如 SET1 或 SET1 和 SET2，触摸这些触摸板将触发中断并唤醒芯片。请调用 :cpp:func:`touch_pad_set_trigger_source` 实现上述操作。 

您可以使用以下函数管理 'SET' 中触摸板所需的位模式配置：

* :cpp:func:`touch_pad_set_group_mask` / :cpp:func:`touch_pad_get_group_mask`
* :cpp:func:`touch_pad_clear_group_mask`


.. _touch_pad-api-examples:

应用示例
--------------------

- 触摸传感器读值示例：:example:`peripherals/touch_pad_read`
- 触摸传感器中断示例：:example:`peripherals/touch_pad_interrupt`

.. _touch_pad-api-reference:

API 参考
-------------

.. include:: /_build/inc/touch_pad.inc

GPIO 宏查找表
^^^^^^^^^^^^^^^^^^
您可以使用宏定义某一触摸板通道的 GPIO，或定义某一 GPIO 的通道。例如：

1. ``TOUCH_PAD_NUM5_GPIO_NUM`` 定义了通道 5 的 GPIO（即 GPIO 12）；
2. ``TOUCH_PAD_GPIO4_CHANNEL`` 定义了 GPIO 4 的通道（即通道 0）。

.. include:: /_build/inc/touch_channel.inc
