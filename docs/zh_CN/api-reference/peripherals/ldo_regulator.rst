低压差线性稳压器 (LDO)
======================

:link_to_translation:`en:[English]`

{IDF_TARGET_LDO_CHANNELS:default="unknown", esp32p4="4"}

简介
----

{IDF_TARGET_NAME} 芯片内部集成了 {IDF_TARGET_LDO_CHANNELS} 路低压差线性稳压器 (LDO)，每路的电压都是可编程调节的。在硬件的参考设计中，我们通常会将其中一些的 LDO 输出作为内部 Flash 和 PSRAM 的电源，剩余的一些 LDO 可以用于给外部设备供电。

.. note::

    使用前请阅读手册，确保你需要的电流不会超过芯片的规格。

功能概述
--------

下文将分节介绍 LDO 驱动的功能：

- :ref:`ldo-channel-acquisition` - 介绍LDO通道的种类以及如何申请 LDO 通道资源
- :ref:`ldo-adjust-voltage` - 介绍如何调节 LDO 通道的输出电压

.. _ldo-channel-acquisition:

LDO 通道申请
^^^^^^^^^^^^

LDO 通道可以分为两种，一种是电压固定的，另一种是电压可调的。对于同一个输出通道，如果电压固定，那么可以允许有多个用户同时使用（软件允许一个变量拥有多个不可变引用）。如果电压可调，那么只能允许一个用户使用（软件上不允许一个变量有多个可变引用或者可变和不可变引用同时存在）。

LDO 通道在驱动软件中由 :cpp:type:`esp_ldo_channel_handle_t` 句柄表示。申请 LDO 通道资源的函数是 :cpp:func:`esp_ldo_acquire_channel`。申请成功后，会返回一个 LDO 通道的句柄，这个句柄可以用于后续的电压调节操作。在申请通道的时候，我们需要通过 :cpp:type:`esp_ldo_channel_config_t` 结构体来指定 LDO 通道的基本信息，包括通道 ID，期望的输出电压，以及电压是否可以动态调节。

- :cpp:member:`esp_ldo_channel_config_t::chan_id` - LDO 通道的唯一标记，用于区分不同的 LDO 通道。注意，这需要你根据电路原理图和芯片数据手册来确定。比如，一个标记着 "LDO_VO3" 的 LDO 通道，对应的 ID 是 3。
- :cpp:member:`esp_ldo_channel_config_t::voltage_mv` - 期望的输出电压，单位是毫伏。
- :cpp:member:`esp_ldo_channel_config_t::ldo_extra_flags::adjustable` - 是否允许调节输出电压。只有设置为 `true`，才允许使用 :cpp:func:`esp_ldo_channel_adjust_voltage` 函数来动态地调节输出电压。

由于允许多个用户同时使用固定电压的 LDO 通道，所以驱动内部会维持一个引用计数器。当最后一个用户释放 LDO 通道资源时，LDO 通道会被自动关闭。释放 LDO 通道资源的函数是 :cpp:func:`esp_ldo_release_channel`。另外还需要注意，申请和释放 LDO 通道在使用的时候需要成对出现。

.. _ldo-adjust-voltage:

LDO 通道电压调节
^^^^^^^^^^^^^^^^

:cpp:func:`esp_ldo_channel_adjust_voltage` 函数用来在运行时调整 LDO 通道的输出电压。但是，这个函数只能用于可调节电压的 LDO 通道，否则会返回错误。

注意，由于硬件限制，LDO 通道电压的精度可能会 50~100mV 左右的偏差，请勿依赖于 LDO 通道的输出电压来进行精确的模拟量控制。

应用示例
--------

.. list::

    :SOC_MIPI_DSI_SUPPORTED: * Use the internal LDO channel to power up the MIPI DPHY: :example:`peripherals/lcd/mipi_dsi`

API 参考
--------

.. include-build-file:: inc/esp_ldo_regulator.inc
