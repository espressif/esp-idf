=====================
ESP32-S2-DevKitM-1(U)
=====================

:link_to_translation:`en: [English]`

本指南介绍了乐鑫的小型开发板 ESP32-S2-DevKitM-1(U)。

ESP32-S2-DevKitM-1(U) 是一款基于 `ESP32-S2FH4 <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_cn.pdf>`__ 芯片（ESP32-S2 系列）的通用型开发板。该款开发板具有丰富的外设和优化的引脚布局，令产品开发更快捷。

ESP32-S2-DevKitM-1 搭载的是 `ESP32-S2-MINI-1 <https://www.espressif.com/sites/default/files/documentation/esp32-s2-mini-1_esp32-s2-mini-1u_datasheet_cn.pdf>`__ 模组 （PCB 板载天线），ESP32-S2-DevKitM-1U 搭载的是 `ESP32-S2-MINI-1U <https://www.espressif.com/sites/default/files/documentation/esp32-s2-mini-1_esp32-s2-mini-1u_datasheet_cn.pdf>`__ 模组 （外部天线连接器）。

+----------------------+-----------------------+
| |ESP32-S2-DevKitM-1| | |ESP32-S2-DevKitM-1U| | 
+----------------------+-----------------------+
|  ESP32-S2-DevKitM-1  |  ESP32-S2-DevKitM-1U  | 
+----------------------+-----------------------+

.. |ESP32-S2-DevKitM-1| image:: ../../../_static/esp32-s2-devkitm-1-v1-isometric.png

.. |ESP32-S2-DevKitM-1U| image:: ../../../_static/esp32-s2-devkitm-1u-v1-isometric.png

本指南包括如下内容：

- `入门指南`_: 简要介绍了 ESP32-S2-DevKitM-1(U) 和硬件、软件设置指南。
- `硬件参考`_: 详细介绍了 ESP32-S2-DevKitM-1(U) 的硬件。
- `硬件版本`_：介绍硬件历史版本和已知问题，并提供链接至历史版本开发板的入门指南（如有）。
- `相关文档`_: 列出了相关文档的链接。

入门指南
========

本节介绍了如何快速上手 ESP32-S2-DevKitM-1(U)。开头部分介绍了 ESP32-S2-DevKitM-1(U)，`开始开发应用`_ 小节介绍了怎样在 ESP32-S2-DevKitM-1(U) 上烧录固件及相关准备工作。

概述
----

ESP32-S2-DevKitM-1(U) 是乐鑫一款搭载 ESP32-S2-MINI-1 或 ESP32-S2-MINI-1U 模组的入门级开发板。板上模组大部分管脚均已引出至两侧排针，开发人员可根据实际需求，轻松通过跳线连接多种外围设备，同时也可将开发板插在面包板上使用。


内含组件和包装
--------------

零售订单
^^^^^^^^

如购买样品，每个 ESP32-S2-DevKitM-1(U) 底板将以防静电袋或零售商选择的其他方式包装。

零售订单请前往 https://www.espressif.com/zh-hans/company/contact/buy-a-sample。

批量订单
^^^^^^^^

如批量购买，ESP32-S2-DevKitM-1(U) 烧录底板将以大纸板箱包装。

批量订单请参考 `乐鑫产品订购信息 <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_cn.pdf>`__ (PDF)。

组件介绍
--------

.. _user-guide-devkitm-1-v1-board-front:

.. figure:: ../../../_static/esp32-s2-devkitm-1-v1-annotated-photo.png
    :align: center
    :alt: ESP32-S2-DevKitM-1 - 正面
    :figclass: align-center

    ESP32-S2-DevKitM-1 - 正面

.. figure:: ../../../_static/esp32-s2-devkitm-1u-v1-annotated-photo.png
    :align: center
    :alt: ESP32-S2-DevKitM-1U - 正面
    :figclass: align-center

    ESP32-S2-DevKitM-1U - 正面

以下按照顺时针的顺序依次介绍开发板上的主要组件。 

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 主要组件
     - 介绍
   * - ESP32-S2-MINI-1 或 ESP32-S2-MINI-1U
     - ESP32-S2-MINI-1 和 ESP32-S2-MINI-1U 是集成 ESP32-S2FH4 的通用型 Wi-Fi MCU 模组，ESP32-S2-MINI-1 采用 PCB 板载天线，ESP32-S2-MINI-1U 采用外部天线连接器。两款模组均配置了 4 MB SPI flash。
   * - Pin Headers（排针）
     - 所有可用 GPIO 管脚（除 flash 的 SPI 总线）均已引出至开发板的排针。用户可对 ESP32-S2FH4 芯片编程，使能 SPI、I2S、UART、I2C、触摸传感器、PWM 等多种功能。请查看 :ref:`user-guide-devkitm-1-v1-header-blocks` 获取更多信息。
   * - 3.3 V Power On LED（3.3 V 电源指示灯）
     - 开发板连接 USB 电源后，该指示灯亮起。
   * - USB-to-UART Bridge（USB 转 UART 桥接器）
     - 单芯片 USB 至 UART 桥接器，可提供高达 3 Mbps 的传输速率。
   * - Reset Button（Reset 键）
     - 复位按键。
   * - Micro-USB（Micro-USB 接口）
     - USB 接口。可用作开发板的供电电源或 PC 和 ESP32-S2FH4 芯片的通信接口。     
   * - Boot Button（Boot 键）
     - 下载按键。按住 **Boot** 键的同时按一下 **Reset** 键进入“固件下载”模式，通过串口下载固件。
   * - RGB LED
     - 可寻址 RGB 发光二极管，由 GPIO18 驱动。
   * - 5 V to 3.3 V LDO（5 V 转 3.3 V LDO）
     - 电源转换器，输入 5 V，输出 3.3 V。
   * - External Antenna Connector（外部天线连接器）
     - 仅 **ESP32-S2-MINI-1U** 模组带有外部天线连接器。连接器尺寸，请参考 `《ESP32-S2-MINI-1 & ESP32-S2-MINI-1U 技术规格书》 <https://www.espressif.com/sites/default/files/documentation/esp32-s2-mini-1_esp32-s2-mini-1u_datasheet_cn.pdf>`_ 的外部天线连接器尺寸章节。

开始开发应用
------------

通电前，请确保 ESP32-S2-DevKitM-1(U) 完好无损。

必备硬件
^^^^^^^^

- ESP32-S2-DevKitM-1(U)

  + 如使用 ESP32-S2-DevKitM-1U，还需准备天线

- USB 2.0 数据线（标准 A 型转 Micro-B 型）
- 电脑 （Windows、Linux 或 macOS）

.. 注解::

  请确保使用适当的 USB 数据线。部分数据线仅可用于充电，无法用于数据传输和编程。

软件设置
^^^^^^^^

请前往 :doc:`../../get-started/index`，在 :ref:`get-started-step-by-step` 一节查看如何快速设置开发环境，将应用程序烧录至 ESP32-S2-DevKitM-1(U)。

.. 注解::
    
    ESP32-S2 系列芯片仅支持 ESP-IDF master 分支或 v4.2 以上版本。

硬件参考
========

功能框图
--------

ESP32-S2-DevKitM-1(U) 的主要组件和连接方式如下图所示。

.. figure:: ../../../_static/esp32-s2-devkitm-1-v1-block-diagram.png
    :align: center
    :scale: 70%
    :alt: ESP32-S2-DevKitM-1(U) （点击放大）
    :figclass: align-center

    ESP32-S2-DevKitM-1(U) （点击放大）

电源选项
^^^^^^^^

您可从以下三种供电方式中任选其一给 ESP32-S2-DevKitM-1(U) 供电：

- Micro-USB 接口供电（默认）
- 5V 和 GND 排针供电
- 3V3 和 GND 排针供电

建议选择第一种供电方式：Micro-USB 接口供电。

.. _user-guide-devkitm-1-v1-header-blocks:

排针
----

下表列出了开发板两侧排针（J1 和 J3）的 **名称** 和 **功能**，排针的名称如图 :ref:`user-guide-devkitm-1-v1-board-front` 所示，排针的序号与 `ESP32-S2-DevKitM-1(U) 原理图 <https://dl.espressif.com/dl/schematics/ESP32-S2-DevKitM-1_V1_Schematics.pdf>`_ (PDF)一致。

J1
^^^
==== ====  =========   =========================================================================
序号 名称  类型 [#]_    功能
==== ====  =========   =========================================================================
1    3V3    P          3.3 V 电源
2    0      I/O/T      RTC_GPIO0, GPIO0
3    1      I/O/T      RTC_GPIO1, GPIO1, TOUCH1, ADC1_CH0
4    2      I/O/T      RTC_GPIO2, GPIO2, TOUCH2, ADC1_CH1
5    3      I/O/T      RTC_GPIO3, GPIO3, TOUCH3, ADC1_CH2
6    4      I/O/T      RTC_GPIO4, GPIO4, TOUCH4, ADC1_CH3
7    5      I/O/T      RTC_GPIO5, GPIO5, TOUCH5, ADC1_CH4
8    6      I/O/T      RTC_GPIO6, GPIO6, TOUCH6, ADC1_CH5
9    7      I/O/T      RTC_GPIO7, GPIO7, TOUCH7, ADC1_CH6
10   8      I/O/T      RTC_GPIO8, GPIO8, TOUCH8, ADC1_CH7
11   9      I/O/T      RTC_GPIO9, GPIO9, TOUCH9, ADC1_CH8, FSPIHD
12   10     I/O/T      RTC_GPIO10, GPIO10, TOUCH10, ADC1_CH9, FSPICS0, FSPIIO4
13   11     I/O/T      RTC_GPIO11, GPIO11, TOUCH11, ADC2_CH0, FSPID, FSPIIO5
14   12     I/O/T      RTC_GPIO12, GPIO12, TOUCH12, ADC2_CH1, FSPICLK, FSPIIO6
15   13     I/O/T      RTC_GPIO13, GPIO13, TOUCH13, ADC2_CH2, FSPIQ, FSPIIO7
16   14     I/O/T      RTC_GPIO14, GPIO14, TOUCH14, ADC2_CH3, FSPIWP, FSPIDQS
17   15     I/O/T      RTC_GPIO15, GPIO15, U0RTS, ADC2_CH4, XTAL_32K_P
18   16     I/O/T      RTC_GPIO16, GPIO16, U0CTS, ADC2_CH5, XTAL_32K_N
19   17     I/O/T      RTC_GPIO17, GPIO17, U1TXD, ADC2_CH6, DAC_1
20   5V     P          5 V 电源
21   G      G          接地
==== ====  =========   =========================================================================

J3
^^^
====  ====  =====  ========================================================
序号  名称   类型    功能
====  ====  =====  ========================================================
1     G     G      接地
2     RST   I      CHIP_PU
3     46    I      GPIO46
4     45    I/O/T  GPIO45
5     RX    I/O/T  U0RXD, GPIO44, CLK_OUT2
6     TX    I/O/T  U0TXD, GPIO43, CLK_OUT1
7     42    I/O/T  MTMS, GPIO42
8     41    I/O/T  MTDI, GPIO41, CLK_OUT1
9     40    I/O/T  MTDO, GPIO40, CLK_OUT2
10    39    I/O/T  MTCK, GPIO39, CLK_OUT3
11    38    I/O/T  GPIO38, FSPIWP
12    37    I/O/T  SPIDQS, GPIO37, FSPIQ
13    36    I/O/T  SPIIO7, GPIO36, FSPICLK
14    35    I/O/T  SPIIO6, GPIO35, FSPID
15    34    I/O/T  SPIIO5, GPIO34, FSPICS0
16    33    I/O/T  SPIIO4, GPIO33, FSPIHD
17    26    I/O/T  SPICS1, GPIO26
18    21    I/O/T  RTC_GPIO21, GPIO21
19    20    I/O/T  RTC_GPIO20, GPIO20, U1CTS, ADC2_CH9, CLK_OUT1, USB_D+
20    19    I/O/T  RTC_GPIO19, GPIO19, U1RTS, ADC2_CH8, CLK_OUT2, USB_D-
21    18    I/O/T  RTC_GPIO18, GPIO18, U1RXD, ADC2_CH7, DAC_2, CLK_OUT3, RGB LED
====  ====  =====  ========================================================

.. [#] P：电源；I：输入；O：输出；T：可设置为高阻。 

管脚布局
^^^^^^^^
.. figure:: ../../../_static/esp32-s2-devkitm-1-v1-pin-layout.png
    :align: center
    :scale: 15%
    :alt: ESP32-S2-DevKitM-1(U) 管脚布局（点击放大）
    :figclass: align-center

    ESP32-S2-DevKitM-1(U) 管脚布局（点击放大）

硬件版本
==========

无历史版本。

相关文档
========
* `ESP32-S2-DevKitM-1(U) 原理图 <https://dl.espressif.com/dl/schematics/ESP32-S2-DevKitM-1_V1_Schematics.pdf>`_ (PDF)
* `ESP32-S2-DevKitM-1(U) PCB 布局 <https://dl.espressif.com/dl/schematics/ESP32-S2-DevKitM-1_V1_PCB_Layout.pdf>`_ (PDF)
* `ESP32-S2-DevKitM-1(U) 尺寸图 <https://dl.espressif.com/dl/schematics/ESP32-S2-DevKitM-1_V1_Dimensions.pdf>`_ (PDF)
* `ESP32-S2 系列技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_cn.pdf>`_ (PDF)
* `ESP32-S2-MINI-1 & ESP32-S2-MINI-1U 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s2-mini-1_esp32-s2-mini-1u_datasheet_cn.pdf>`_ (PDF)
* `乐鑫产品订购信息 <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_cn.pdf>`_ (PDF) 

有关本开发板的更多设计文档，请联系我们的商务部门 `sales@espressif.com <sales@espressif.com>`_。
