触摸传感器
============

:link_to_translation:`en:[English]`

{IDF_TARGET_TOUCH_SENSOR_VERSION:default="v2", esp32="v1"}

概述
------------

触摸传感器系统由保护覆盖层、触摸电极、绝缘基板和走线组成，保护覆盖层位于最上层，绝缘基板上设有电极及走线。触摸覆盖层将引起电容变化，根据电容变化，可以判断此次触摸是否为有效触摸行为。

触摸传感器可以以矩阵或滑条等方式组合使用，从而覆盖更大触感区域及更多触感点。触摸传感由软件或专用硬件计时器发起，由有限状态机 (FSM) 硬件控制。

如需了解触摸传感器设计、操作及其控制寄存器等相关信息，请参考《`{IDF_TARGET_NAME} 技术参考手册 <{IDF_TARGET_TRM_CN_URL}>`_》(PDF) 中“片上传感器与模拟信号处理”章节。

请参考 `触摸传感器应用方案简介 <https://github.com/espressif/esp-iot-solution/blob/release/v1.0/documents/touch_pad_solution/touch_sensor_design_en.md>`_，查看触摸传感器设计详情和固件开发指南。

.. only:: esp32

  如果想评估触摸传感器的多种应用场景，请查看 `ESP32 触摸功能开发套件 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32/esp32-sense-kit/user_guide.html>`_。

功能介绍
----------------------

下面将 API 分解成几个函数组进行介绍：

- 初始化触摸传感器驱动程序
- 配置触摸传感器 GPIO 管脚
- 触摸状态测量
- 调整测量参数（优化测量）
- 滤波采样
- 触摸监测方式
- 设置中断信号监测触碰动作
- 中断触发，唤醒睡眠模式

请前往 :ref:`touch_pad-api-reference` 章节，查看某一函数的具体描述。:ref:`应用示例 <touch_pad-api-examples>` 章节则介绍了此 API 的具体实现。

初始化触摸传感器驱动程序
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

使用触摸传感器之前，需要先调用 :cpp:func:`touch_pad_init` 函数初始化触摸传感器驱动程序。此函数设置了 :ref:`touch_pad-api-reference` 项下的 *Macros* 中列出的几项 ``.._DEFAULT`` 驱动程序参数，同时删除之前设置过的触摸传感器信息（如有），并禁用中断。

如果不再需要该驱动程序，可以调用 :cpp:func:`touch_pad_deinit` 释放已初始化的驱动程序。

配置触摸传感器 GPIO 管脚
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

    可调用 :cpp:func:`touch_pad_config()` 使能某一 GPIO 的触感功能。{IDF_TARGET_NAME} 最多可支持 10 个电容式触摸传感器通道。

    .. list-table::
        :align: center
        :widths: 50 50
        :header-rows: 1

        * - 触摸传感器通道
          - GPIO 管脚
        * - T0
          - GPIO4
        * - T1
          - GPIO0
        * - T2
          - GPIO2
        * - T3
          - MTDO
        * - T4
          - MTCK
        * - T5
          - MTDI
        * - T6
          - MTMS
        * - T7
          - GPIO27
        * - T8
          - 32K_XN
        * - T9
          - 32K_XP

.. only:: esp32s2 or esp32s3

    可调用 :cpp:func:`touch_pad_config()` 使能某一 GPIO 的触感功能。{IDF_TARGET_NAME} 最多可支持 14 个电容式触摸传感器通道。

    .. list-table::
        :align: center
        :widths: 50 50
        :header-rows: 1

        * - 触摸传感器通道
          - 管脚
        * - T0
          - 内部通道，无对应管脚
        * - T1
          - GPIO1
        * - T2
          - GPIO2
        * - T3
          - GPIO3
        * - T4
          - GPIO4
        * - T5
          - GPIO5
        * - T6
          - GPIO6
        * - T7
          - GPIO7
        * - T8
          - GPIO8
        * - T9
          - GPIO9
        * - T10
          - GPIO10
        * - T11
          - GPIO11
        * - T12
          - GPIO12
        * - T13
          - GPIO13
        * - T14
          - GPIO14

使用 :cpp:func:`touch_pad_set_fsm_mode` 选择触摸传感器测量（由 FSM 操作）是由硬件定时器自动启动，还是由软件自动启动。如果选择软件模式，请使用 :cpp:func:`touch_pad_sw_start` 启动 FSM。

触摸状态测量
^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

    借助以下两个函数从传感器读取原始数据和滤波后的数据：

    * :cpp:func:`touch_pad_read_raw_data`
    * :cpp:func:`touch_pad_read_filtered`

    这两个函数也可以用于检查触碰和释放触摸传感器时传感器读数变化范围，然后根据这些信息设定触摸传感器的触摸阈值。

    .. note::

        使用 :cpp:func:`touch_pad_read_filtered` 之前，需要先调用 `滤波采样`_ 中特定的滤波器函数来初始化并配置该滤波器。

.. only:: esp32s2 or esp32s3

    借助以下函数从传感器读取原始数据：

    * :cpp:func:`touch_pad_read_raw_data`

    该函数也可以用于检查触碰和释放触摸传感器时传感器读数变化范围，然后根据这些信息设定触摸传感器的触摸阈值。

请参考应用示例 :example:`peripherals/touch_sensor/touch_sensor_{IDF_TARGET_TOUCH_SENSOR_VERSION}/touch_pad_read`，查看如何使用读取触摸传感器数据。

测量方式
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

    触摸传感器会统计固定时间内的充放电次数，其计数结果即为原始数据，可由 :cpp:func:`touch_pad_read_raw_data` 读出。上述固定时间可通过 :cpp:func:`touch_pad_set_measurement_clock_cycles` 设置。完成一次测量后，触摸传感器会在下次测量开始前保持睡眠状态。两次测量之前的间隔时间可由 :cpp:func:`touch_pad_set_measurement_interval` 进行设置。

    .. note::

        若设置的计数时间太短（即测量持续的时钟周期数太小），则可能导致结果不准确，但是过大的计数时间也会造成功耗上升。另外，若睡眠时间加测量时间的总时间过长，则会造成触摸传感器响应变慢。

.. only:: esp32s2 or esp32s3

    触摸传感器会统计固定充放电次数所需的时间（即所需时钟周期数），其结果即为原始数据，可由 :cpp:func:`touch_pad_read_raw_data` 读出。上述固定的充放电次数可通过 :cpp:func:`touch_pad_set_charge_discharge_times` 设置。完成一次测量后，触摸传感器会在下次测量开始前保持睡眠状态。两次测量之前的间隔时间可由 :cpp:func:`touch_pad_set_measurement_interval` 进行设置。

    .. note::

        若设置的充放电次数太少，则可能导致结果不准确，但是充放电次数过多也会造成功耗上升。另外，若睡眠时间加测量时间的总时间过长，则会造成触摸传感器响应变慢。

优化测量
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

触摸传感器设有数个可配置参数，以适应触摸传感器设计特点。例如，如果需要感知较细微的电容变化，则可以缩小触摸传感器充放电的参考电压范围。使用 :cpp:func:`touch_pad_set_voltage` 函数，可以设置电压参考低值和参考高值。

.. only:: esp32

    优化测量除了可以识别细微的电容变化之外，还可以降低应用程序功耗，但可能会增加测量噪声干扰。如果得到的动态读数范围结果比较理想，则可以调用 :cpp:func:`touch_pad_set_measurement_clock_cycles` 函数来减少测量时间，从而进一步降低功耗。

.. only:: esp32s2 or esp32s3

    优化测量除了可以识别细微的电容变化之外，还可以降低应用程序功耗，但可能会增加测量噪声干扰。如果得到的动态读数范围结果比较理想，则可以调用 :cpp:func:`touch_pad_set_charge_discharge_times` 函数来减少测量时间，从而进一步降低功耗。

可用的测量参数及相应的 'set' 函数总结如下：

* 触摸传感器充放电参数：

    * 电压门限：:cpp:func:`touch_pad_set_voltage`
    * 速率（斜率）：:cpp:func:`touch_pad_set_cnt_mode`

.. only:: esp32

    * 单次测量所用的时钟周期：:cpp:func:`touch_pad_set_measurement_clock_cycles`

.. only:: esp32s2 or esp32s3

    * 单次测量所需充放电次数：:cpp:func:`touch_pad_set_charge_discharge_times`

电压门限（参考低值/参考高值）、速率（斜率）与测量时间的关系如下图所示：

.. only:: esp32

    .. figure:: ../../../_static/touch_pad-measurement-parameters.jpg
        :align: center
        :alt: Touch Pad - relationship between measurement parameters
        :figclass: align-center

        触摸传感器 - 测量参数之间的关系

    上图中的 **Output** 代表触摸传感器读值，即一个测量周期内测得的脉冲计数值。

.. only:: esp32s2 or esp32s3

    .. figure:: ../../../_static/touch_pad-measurement-parameters-version2.png
        :align: center
        :alt: Touch Pad - relationship between measurement parameters
        :figclass: align-center

        触摸传感器 - 测量参数之间的关系

    上图中的 **Output** 代表触摸传感器读值，即固定充放电次数所需的时间。

所有函数均成对出现，用于设定某一特定参数，并获取当前参数值。例如：:cpp:func:`touch_pad_set_voltage` 和 :cpp:func:`touch_pad_get_voltage`。

.. _touch_pad-api-filtering-of-measurements:

滤波采样
^^^^^^^^^^^^^^^^^^^^^^^^^
.. only:: esp32

    如果测量中存在噪声，可以使用提供的 API 函数对采样进行滤波。使用滤波器之前，请先调用 :cpp:func:`touch_pad_filter_start` 启动该滤波器。

    滤波器类型为 IIR（无限脉冲响应滤波器），可以调用 :cpp:func:`touch_pad_set_filter_period` 配置此类滤波器的采样周期。

    如需停止滤波器，请调用 :cpp:func:`touch_pad_filter_stop` 函数。如果不再使用该滤波器，请调用 :cpp:func:`touch_pad_filter_delete` 删除此滤波器。

.. only:: esp32s2 or esp32s3

    如果测量中存在噪声，可以使用提供的 API 函数对采样进行滤波。{IDF_TARGET_NAME} 的触摸功能提供了两套 API 可实现此功能。

    一个是内部触摸通道，它没有连接到任何外部 GPIO。该降噪板的测量值可用于过滤所有通道上的干扰，如来自电源和外部 EMI 的噪声。

    降噪参数由 :cpp:func:`touch_pad_denoise_set_config` 设置并由 :cpp:func:`touch_pad_denoise_enable` 启动。

    另一是可配置的硬件实现 IIR-滤波器（无限脉冲响应滤波器），该滤波器可通过调用 :cpp:func:`touch_pad_filter_set_config` 函数进行配置，调用 :cpp:func:`touch_pad_filter_enable` 函数启用。

触摸监测
^^^^^^^^^^^^^^^

触摸监测基于配置的阈值和 FSM 执行的原始测量，并由 ESP32 硬件实现。可以调用 :cpp:func:`touch_pad_get_status` 查看被触碰的触摸传感器，或调用 :cpp:func:`touch_pad_clear_status` 清除触摸状态信息。

也可以将硬件触摸监测连接至中断，详细介绍见下一章节。

如果测量中存在噪声，且电容变化幅度较小，硬件触摸监测结果可能就不太理想。如需解决这一问题，不建议使用硬件监测或中断信号，建议在自己的应用程序中进行采样滤波，并执行触摸监测。请参考 :example:`peripherals/touch_sensor/touch_sensor_{IDF_TARGET_TOUCH_SENSOR_VERSION}/touch_pad_interrupt`，查看以上两种触摸监测的实现方式。

中断触发
^^^^^^^^^^^^^^^^^^^^^^^^^^

在对触摸监测启用中断之前，请先设置一个触摸监测阈值。然后使用 `触摸状态测量`_ 中所述的函数读取并显示触摸和释放触摸传感器时测得的结果。如果测量中存在噪声且相对电容变化较小，请使用滤波器。也可以根据应用程序和环境条件，测试温度和电源电压变化对测量值的影响。

确定监测阈值后就可以在初始化时调用 :cpp:func:`touch_pad_config` 设置此阈值，或在运行时调用 :cpp:func:`touch_pad_set_thresh` 设置此阈值。

.. only:: esp32

    下一步就是设置如何触发中断。可以设置在阈值以下或以上触发中断，具体触发模式由函数 :cpp:func:`touch_pad_set_trigger_mode` 设置。

最后可以使用以下函数配置和管理中断调用：

* :cpp:func:`touch_pad_isr_register` / :cpp:func:`touch_pad_isr_deregister`
* :cpp:func:`touch_pad_intr_enable` / :cpp:func:`touch_pad_intr_disable`

中断配置完成后，可以调用 :cpp:func:`touch_pad_get_status` 查看中断信号来自哪个触摸传感器，也可以调用 :cpp:func:`touch_pad_clear_status` 清除触摸传感器状态信息。

.. only:: esp32

    .. note::

        触摸监测中的中断信号基于原始/未经滤波的采样（对比设置的阈值），并在硬件中实现。启用软件滤波 API（请参考 :ref:`touch_pad-api-filtering-of-measurements`）并不会影响这一过程。

.. only:: esp32

    从睡眠模式唤醒
    ^^^^^^^^^^^^^^^^^^^^^^

    如果使用触摸传感器中断将芯片从睡眠模式唤醒，可以选择配置一些触摸传感器，例如 SET1 或 SET1 和 SET2，触摸这些触摸传感器将触发中断并唤醒芯片。请调用 :cpp:func:`touch_pad_set_trigger_source` 实现上述操作。

    可以使用以下函数管理 'SET' 中触摸传感器所需的位模式配置：

    * :cpp:func:`touch_pad_set_group_mask` / :cpp:func:`touch_pad_get_group_mask`
    * :cpp:func:`touch_pad_clear_group_mask`

.. _touch_pad-api-examples:

应用示例
--------

.. only:: esp32

    - :example:`peripherals/touch_sensor/touch_sensor_v1/touch_pad_read` 演示了如何在 {IDF_TARGET_NAME} 上读取并打印电容触摸传感器的原始值或 IIR 滤波后的值，以及如何校准传感器。
    - :example:`peripherals/touch_sensor/touch_sensor_v1/touch_pad_interrupt` 演示了如何设置 {IDF_TARGET_NAME} 的电容触摸板，使其在被触摸时触发中断，以及如何使用软件监测来提高灵敏度，并比较硬件中断模式和软件轮询模式的性能。

.. only:: esp32s2 or esp32s3

    - :example:`peripherals/touch_sensor/touch_sensor_v2/touch_pad_read` 演示了如何在 {IDF_TARGET_NAME} 上读取并显示电容触摸传感器的原始值，包括如何校准传感器以及监测触摸动作。
    - :example:`peripherals/touch_sensor/touch_sensor_v2/touch_pad_interrupt` 演示了如何设置 {IDF_TARGET_NAME} 的电容触摸板外设，使其在被触摸时触发中断，以及在需要更高触摸监测灵敏度的传感器设计中，如何通过软件来监测触摸事件。


.. _touch_pad-api-reference:

API 参考
-------------

.. include-build-file:: inc/touch_sensor.inc
.. include-build-file:: inc/touch_sensor_common.inc

GPIO 宏查找表
^^^^^^^^^^^^^^^^^^

可以使用宏定义某一触摸传感器通道的 GPIO，或定义某一 GPIO 的通道。例如：

1. ``TOUCH_PAD_NUM5_GPIO_NUM`` 定义了通道 5 的 GPIO（即 GPIO 12）；
2. ``TOUCH_PAD_GPIO4_CHANNEL`` 定义了 GPIO 4 的通道（即通道 0）。

.. include-build-file:: inc/touch_sensor_channel.inc
.. include-build-file:: inc/touch_sensor_types.inc
