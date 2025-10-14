模数转换器 (ADC)
==================

:link_to_translation:`en:[English]`

概述
----

本指南介绍了 {IDF_TARGET_NAME} 的模数转换器 (ADC)。首先介绍 ADC 核心概念，如 ADC 转换、原始数据分辨率、参考电压和衰减配置，随后详细讲解两种支持的驱动模式：单次转换模式和连续转换模式，以及用于提升精度的 ADC 校准功能。

{IDF_TARGET_NAME} 集成了 {IDF_TARGET_SOC_ADC_PERIPH_NUM} 个 ADC，用于测量多路输入通道的模拟信号。具体测量通道数（支持模拟功能的引脚）、电压范围和 ADC 特性，请参阅 `技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`__。

ADC 转换
----------

ADC 转换是将输入的模拟电压转换为数字值的过程。ADC 驱动 API 提供的原始转换结果是以数字形式反映模拟输入信号。

默认情况下，ADC 原始转换结果的位宽为 12 位，这意味着输入电压范围被划分为 4096 (2\ :sup:`12`) 个离散电平，由此定义了输入信号的最小可检测变化量。

根据原始值 ``data`` 计算对应电压值 ``Vdata`` 的公式如下：

.. math::

    V_{data} = \frac{data}{2^{bitwidth} - 1} \times V_{ref}

其中：

- ``data`` 是 ADC 原始数据
- ``bitwidth`` 是 ADC 原始数据的分辨率（如 12 位）
- ``Vref`` 是 ADC 参考电压


设计上，``Vref`` 出厂设定为 1100 mV，但由于制造差异，实际值可能在 1000 mV 到 1200 mV 之间波动。

请参阅 :doc:`adc_calibration` 了解如何利用 ADC 校准驱动以得到校准后的准确电压值。


ADC 衰减
----------

ADC 可以测量 0 V 到 ``Vref`` 范围内的模拟电压。如需测量更高电压，可以在输入 ADC 之前对输入电压进行衰减。

支持的衰减等级包括：

- 0 dB (k≈100%)
- 2.5 dB (k≈75%)
- 6 dB (k≈50%)
- 12 dB (k≈25%)

衰减等级越高，ADC 可测量的输入电压范围越大。应用衰减后的电压 ``Vdata`` 计算公式为：

.. math::

    V_{data} = \frac{V_{ref}}{k}\times{\frac{data}{2^{bitwidth} - 1}}

其中：

- ``k``：衰减参数对应的比例值
- 其他变量定义同上

.. only:: not esp32

    各衰减配置对应的可测量输入电压范围，请参考 `技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`__ > 电气特性 > ADC 特性。

.. only:: esp32

    各衰减配置对应的可测量输入电压范围，请参考 `技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`__ > 模拟外设 > 模/数转换器 (ADC)。

驱动使用
------------

.. list::

    - 每个 ADC 单元支持 **单次转换模式**。该模式适用于单次采样：ADC 一次对一个通道进行采样。
    :SOC_ADC_DMA_SUPPORTED: - 每个 ADC 单元支持 **连续转换模式**。该模式适用于连续采样：ADC 依次对一组通道进行采样，或者连续对单个通道进行采样。

详见以下指南：

.. toctree::
    :maxdepth: 2

    adc_oneshot
    :SOC_ADC_DMA_SUPPORTED: adc_continuous


ADC 校准功能
-----------------

ADC 校准驱动通过软件方式，修正偏差，获取更准确的输出结果。

详见以下指南：

.. toctree::
    :maxdepth: 2

    adc_calibration

API 参考
-------------

.. include-build-file:: inc/adc_channel.inc
.. include-build-file:: inc/adc_types.inc
