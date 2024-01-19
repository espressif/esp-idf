================
ESP32-S2-Saola-1
================

:link_to_translation:`en: [English]`

本指南介绍了乐鑫一款基于 `ESP32-S2 <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_cn.pdf>`_ 的小型开发板 ESP32-S2-Saola-1。

.. figure:: ../../../_static/esp32-s2-saola-1-v1.2-isometric.png
    :align: center
    :alt: ESP32-S2-Saola-1
    :figclass: align-center

    ESP32-S2-Saola-1

本指南包括如下内容：

- `入门指南`_：简要介绍了 ESP32-S2-Saola-1 和硬件、软件设置指南。
- `硬件参考`_：详细介绍了 ESP32-S2-Saola-1 的硬件。
- `硬件版本`_：介绍硬件历史版本和已知问题，并提供链接至历史版本开发板的入门指南（如有）。
- `相关文档`_：列出了相关文档的链接。


入门指南
========

本节介绍了如何快速上手 ESP32-S2-Saola-1。开头部分介绍了 ESP32-S2-Saola-1，`开始开发应用`_ 小节介绍了怎样在 ESP32-S2-Saola-1 上安装模组、设置及烧录固件。


概述
----

ESP32-S2-Saola-1 是乐鑫一款基于 ESP32-S2 的小型开发板。板上的绝大部分管脚均已引出，开发人员可根据实际需求，轻松通过跳线连接多种外围器件，或将开发板插在面包板上使用。

为了更好地满足不同用户需求，ESP32-S2-Saola-1 支持以下模组：

- `ESP32-S2-WROVER <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wrover_esp32-s2-wrover-i_datasheet_cn.pdf>`_
- `ESP32-S2-WROVER-I <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wrover_esp32-s2-wrover-i_datasheet_cn.pdf>`_
- `ESP32-S2-WROOM <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wroom_esp32-s2-wroom-i_datasheet_cn.pdf>`_
- `ESP32-S2-WROOM-I <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wroom_esp32-s2-wroom-i_datasheet_cn.pdf>`_

本指南以搭载 ESP32-S2-WROVER 模组的 ESP32-S2-Saola-1 为例。


内含组件和包装
--------------

零售订单
^^^^^^^^

如购买样品，每个 ESP32-S2-Saola-1 开发板将以防静电袋或零售商选择的其他方式包装。

零售订单请前往 https://www.espressif.com/zh-hans/company/contact/buy-a-sample。


批量订单
^^^^^^^^

如批量购买，ESP32-S2-Saola-1 开发板将以大纸板箱包装。

批量订单请前往 https://www.espressif.com/zh-hans/contact-us/sales-questions。


组件介绍
--------

.. _user-guide-saola-1-v1.2-board-front:

.. figure:: ../../../_static/esp32-s2-saola-1-v1.2-annotated-photo.png
    :align: center
    :alt: ESP32-S2-Saola-1 - 正面
    :figclass: align-center

    ESP32-S2-Saola-1 - 正面

以下按照顺时针的顺序依次介绍开发板上的主要组件。

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 主要组件
     - 介绍
   * - ESP32-S2-WROVER
     - ESP32-S2-WROVER 集成 ESP32-S2，是通用型 Wi-Fi MCU 模组，功能强大。该模组采用 PCB 板载天线，配置了 4 MB SPI flash 和 2 MB SPI PSRAM。
   * - Pin Headers（排针）
     - 所有可用 GPIO 管脚（除 Flash 和 PSRAM 的 SPI 总线）均已引出至开发板的排针。用户可对 ESP32-S2 芯片编程，使能 SPI、I2S、UART、I2C、触摸传感器、PWM 等多种功能。
   * - 3.3 V Power On LED（3.3 V 电源指示灯）
     - 开发板连接 USB 电源后，该指示灯亮起。
   * - USB-to-UART Bridge（USB 转 UART 桥接器）
     - 单芯片 USB 至 UART 桥接器，可提供高达 3 Mbps 的传输速率。
   * - Reset Button（Reset 键）
     - 复位按键。
   * - Micro-USB Port（Micro-USB 接口）
     - USB 接口。可用作开发板的供电电源或 PC 和 ESP32-S2 芯片的通信接口。
   * - Boot Button（Boot 键）
     - 下载按键。按住 **Boot** 键的同时按一下 **Reset** 键进入“固件下载”模式，通过串口下载固件。
   * - RGB LED
     - 可寻址 RGB 发光二极管 (WS2812)，由 GPIO18 驱动。


开始开发应用
------------

通电前，请确保 ESP32-S2-Saola-1 完好无损。


必备硬件
^^^^^^^^

- ESP32-S2-Saola-1
- USB 2.0 数据线（标准 A 型转 Micro-B 型）
- 电脑（Windows、Linux 或 macOS）

.. 注解::

  请确保使用适当的 USB 数据线。部分数据线仅可用于充电，无法用于数据传输和编程。


软件设置
^^^^^^^^

请前往 :doc:`../../get-started/index`，在 :ref:`get-started-step-by-step` 一节查看如何快速设置开发环境，将应用程序烧录至 ESP32-S2-Saola-1。

.. 注解::

    ESP32-S2 系列芯片仅支持 ESP-IDF master 分支或 v4.2 以上版本。


硬件参考
========

功能框图
--------

ESP32-S2-Saola-1 的主要组件和连接方式如下图所示。

.. figure:: ../../../_static/esp32-s2-saola-1-v1.2-block-diags.png
    :align: center
    :scale: 70%
    :alt: ESP32-S2-Saola-1 （点击放大）
    :figclass: align-center

    ESP32-S2-Saola-1 （点击放大）


电源选项
^^^^^^^^

以下任一供电方式均可给 ESP32-S2-Saola-1 供电：

- Micro-USB 接口供电（默认）
- 5V 和 GND 排针供电
- 3V3 和 GND 排针供电

建议选择第一种供电方式：Micro-USB 接口供电。


排针
----

下表列出了开发板两侧排针（J2 和 J3）的 **名称** 和 **功能**，排针的名称如图 :ref:`user-guide-saola-1-v1.2-board-front` 所示，排针的序号与 `ESP32-S2-Saola-1 原理图`_ (PDF) 一致。


J2
^^^

==== ====  =========   ======================================
序号 名称  类型 [#]_    功能
==== ====  =========   ======================================
1    3V3    P          3.3 V 电源
2    IO0    I/O        GPIO0, 启动
3    IO1    I/O        GPIO1, ADC1_CH0, TOUCH_CH1
4    IO2    I/O        GPIO2, ADC1_CH1, TOUCH_CH2
5    IO3    I/O        GPIO3, ADC1_CH2, TOUCH_CH3
6    IO4    I/O        GPIO4, ADC1_CH3, TOUCH_CH4
7    IO5    I/O        GPIO5, ADC1_CH4, TOUCH_CH5
8    IO6    I/O        GPIO6, ADC1_CH5, TOUCH_CH6
9    IO7    I/O        GPIO7, ADC1_CH6, TOUCH_CH7
10   IO8    I/O        GPIO8, ADC1_CH7, TOUCH_CH8
11   IO9    I/O        GPIO9, ADC1_CH8, TOUCH_CH9
12   IO10   I/O        GPIO10, ADC1_CH9, TOUCH_CH10
13   IO11   I/O        GPIO11, ADC2_CH0, TOUCH_CH11
14   IO12   I/O        GPIO12, ADC2_CH1, TOUCH_CH12
15   IO13   I/O        GPIO13, ADC2_CH2, TOUCH_CH13
16   IO14   I/O        GPIO14, ADC2_CH3, TOUCH_CH14
17   IO15   I/O        GPIO15, ADC2_CH4, XTAL_32K_P
18   IO16   I/O        GPIO16, ADC2_CH5, XTAL_32K_N
19   IO17   I/O        GPIO17, ADC2_CH6, DAC_1
20   5V0    P          5 V 电源
21   GND    G          接地
==== ====  =========   ======================================


J3
^^^

====  ====  =====  ====================================
序号  名称   类型    功能
====  ====  =====  ====================================
1     GND   G      接地
2     RST   I      CHIP_PU, 复位
3     IO46  I      GPIO46
4     IO45  I/O    GPIO45
5     IO44  I/O    GPIO44, U0RXD
6     IO43  I/O    GPIO43, U0TXD
7     IO42  I/O    GPIO42, MTMS
8     IO41  I/O    GPIO41, MTDI
9     IO40  I/O    GPIO40, MTDO
10    IO39  I/O    GPIO39, MTCK
11    IO38  I/O    GPIO38
12    IO37  I/O    GPIO37
13    IO36  I/O    GPIO36
14    IO35  I/O    GPIO35
16    IO34  I/O    GPIO34
17    IO33  I/O    GPIO33
17    IO26  I/O    GPIO26
18    IO21  I/O    GPIO21
19    IO20  I/O    GPIO20, ADC2_CH9, USB_D+
20    IO19  I/O    GPIO19, ADC2_CH8, USB_D-
21    IO18  I/O    GPIO18, ADC2_CH7, DAC_2, RGB LED
====  ====  =====  ====================================

.. [#] P：电源；I：输入；O：输出；T：可设置为高阻。


管脚布局
^^^^^^^^

.. figure:: ../../../_static/esp32-s2_saola1-pinout.jpg
    :align: center
    :scale: 45%
    :alt: ESP32-S2-Saola-1 管脚布局（点击放大）
    :figclass: align-center

    ESP32-S2-Saola-1 管脚布局（点击放大）


硬件版本
==========

无历史版本。


相关文档
========

* `ESP32-S2-Saola-1 原理图`_ (PDF)
* `ESP32-S2-Saola-1 尺寸图`_ (PDF)
* `ESP32-S2 技术规格书`_ (PDF)
* `ESP32-S2-WROVER & ESP32-S2-WROVER-I 技术规格书`_ (PDF)
* `ESP32-S2-WROOM & ESP32-S2-WROOM-I 技术规格书`_ (PDF)
* `乐鑫产品选型工具`_

有关本开发板的更多设计文档，请联系我们的商务部门 `sales@espressif.com <sales@espressif.com>`_。

.. _ESP32-S2-Saola-1 原理图: https://dl.espressif.com/dl/schematics/ESP32-S2-SAOLA-1_V1.1_schematics.pdf
.. _ESP32-S2-Saola-1 尺寸图: https://dl.espressif.com/dl/schematics/ESP32-S2-Saola-1_V1.2_Dimensions.pdf
.. _ESP32-S2 技术规格书: https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_cn.pdf
.. _ESP32-S2-WROVER & ESP32-S2-WROVER-I 技术规格书: https://www.espressif.com/sites/default/files/documentation/esp32-s2-wrover_esp32-s2-wrover-i_datasheet_cn.pdf
.. _ESP32-S2-WROOM & ESP32-S2-WROOM-I 技术规格书: https://www.espressif.com/sites/default/files/documentation/esp32-s2-wroom_esp32-s2-wroom-i_datasheet_cn.pdf
.. _乐鑫产品选型工具: https://products.espressif.com/#/product-selector?names=
