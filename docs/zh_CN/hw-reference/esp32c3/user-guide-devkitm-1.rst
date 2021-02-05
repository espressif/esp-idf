==================
ESP32-C3-DevKitM-1
==================

:link_to_translation:`en: [English]`

本指南将帮助您快速上手 ESP32-C3-DevKitM-1，并提供该款开发板的详细信息。

ESP32-C3-DevKitM-1 是一款入门级开发板，使用以尺寸小而得名的 `ESP32-C3-MINI-1 <https://www.espressif.com/zh-hans/products/modules>`_ 模组。该款开发板具备完整的 Wi-Fi 和低功耗蓝牙功能。

板上模组大部分管脚均已引出至两侧排针，开发人员可根据实际需求，轻松通过跳线连接多种外围设备，同时也可将开发板插在面包板上使用。

.. figure:: ../../../_static/esp32-c3-devkitm-1-v1-isometric.png
    :align: center
    :alt: ESP32-C3-DevKitM-1
    :figclass: align-center

    ESP32-C3-DevKitM-1

本指南包括如下内容：

- `入门指南`_: 简要介绍了 ESP32-C3-DevKitM-1 和硬件、软件设置指南。
- `硬件参考`_: 详细介绍了 ESP32-C3-DevKitM-1 的硬件。
- `硬件版本`_：介绍硬件历史版本和已知问题，并提供链接至历史版本开发板的入门指南（如有）。
- `相关文档`_: 列出了相关文档的链接。

入门指南
========

本小节将简要介绍 ESP32-C3-DevKitM-1，说明如何在 ESP32-C3-DevKitM-1 上烧录固件及相关准备工作。

组件介绍
--------

.. _user-guide-c3-devkitm-1-v1-board-front:

.. figure:: ../../../_static/esp32-c3-devkitm-1-v1-annotated-photo.png
    :align: center
    :alt: ESP32-C3-DevKitM-1 - 正面
    :figclass: align-center

    ESP32-C3-DevKitM-1 - 正面

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 主要组件
     - 介绍
   * - ESP32-C3-MINI-1
     - ESP32-C3-MINI-1 是一款通用型 Wi-Fi 和低功耗蓝牙双模模组，采用 PCB 板载天线。该款模组集成配置 4 MB 嵌入式 flash 的 `ESP32-C3FN4 <https://www.espressif.com/zh-hans/products/socs/esp32-c3>`_ 芯片。由于 flash 直接封装在芯片中，ESP32-C3-MINI-1 模组具有更小的封装尺寸。
   * - 5 V 转 3.3 V LDO
     - 电源转换器，输入 5 V，输出 3.3 V。
   * - 5 V 电源指示灯
     - 开发板连接 USB 电源后，该指示灯亮起。
   * - I/O 连接器 
     - 所有可用 GPIO 管脚（除 flash 的 SPI 总线）均已引出至开发板的排针。请查看 :ref:`user-guide-c3-devkitm-1-v1-header-blocks` 获取更多信息。
   * - Boot 键
     - 下载按键。按住 **Boot** 键的同时按一下 **Reset** 键进入“固件下载”模式，通过串口下载固件。
   * - Micro-USB 接口
     - USB 接口。可用作开发板的供电电源或 PC 和 ESP32-C3FN4 芯片的通信接口。     
   * - Reset 键
     - 复位按键。 
   * - USB 至 UART 桥接器
     - 单芯片 USB 至 UART 桥接器，可提供高达 3 Mbps 的传输速率。
   * - RGB LED
     - 可寻址 RGB 发光二极管 (WS2812)，由 GPIO8 驱动。

开始开发应用
------------

通电前，请确保 ESP32-C3-DevKitM-1 完好无损。

必备硬件
^^^^^^^^

- ESP32-C3-DevKitM-1
- USB 2.0 数据线（标准 A 型转 Micro-B型）
- 电脑 （Windows、Linux 或 macOS）

软件设置
^^^^^^^^

请前往 :doc:`../../get-started/index`，在 :ref:`get-started-step-by-step` 小节查看如何快速设置开发环境，将应用程序烧录至 ESP32-C3-DevKitM-1。

内含组件和包装
--------------

零售订单
^^^^^^^^

如购买样品，每个 ESP32-C3-DevKitM-1 底板将以防静电袋或零售商选择的其他方式包装。

零售订单请前往 https://www.espressif.com/zh-hans/company/contact/buy-a-sample。

批量订单
^^^^^^^^

如批量购买，ESP32-C3-DevKitM-1 烧录底板将以大纸板箱包装。

批量订单请参考 `乐鑫产品订购信息 <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_cn.pdf>`__ (PDF)。


硬件参考
========

功能框图
--------

ESP32-C3-DevKitM-1 的主要组件和连接方式如下图所示。

.. figure:: ../../../_static/esp32-c3-devkitm-1-v1-block-diagram.png
    :align: center
    :scale: 70%
    :alt: ESP32-C3-DevKitM-1 （点击放大）
    :figclass: align-center

    ESP32-C3-DevKitM-1 （点击放大）

电源选项
^^^^^^^^

您可从以下三种供电方式中任选其一给 ESP32-C3-DevKitM-1 供电：

- Micro USB 端口供电（默认）
- 5V 和 GND 管脚供电
- 3V3 和 GND 管脚供电

建议选择第一种供电方式：Micro USB 端口供电。

.. _user-guide-c3-devkitm-1-v1-header-blocks:

排针
----

下表列出了开发板两侧排针的 **名称** 和 **功能**，开发板排针图可前往 :ref:`user-guide-c3-devkitm-1-v1-board-front` 查看。表格中的序号和名称与 `ESP32-C3-DevKitM-1 原理图 <https://dl.espressif.com/dl/schematics/SCH_ESP32-C3-DEVKITM-1_V1_20200915A.pdf>`_ (PDF)一致。

J1
^^^
====  ====  =====  ================================
序号  名称   类型   功能
====  ====  =====  ================================
1     GND   G      接地
2     3V3   P      3.3 V 电源
3     3V3   P      3.3 V 电源
4     IO2   I/O/T  GPIO2, ADC1_CH2, FSPIQ
5     IO3   I/O/T  GPIO3, ADC1_CH3
6     GND   G      接地
7     RST   I      CHIP_PU
8     GND   G      接地
9     IO1   I/O/T  GPIO1, ADC1_CH1, XTAL_32K_N
11    IO10  I/O/T  GPIO10, FSPICS0
12    GND   G      接地
13    5V    P      5 V 电源
14    5V    P      5 V 电源
15    GND   G      接地
====  ====  =====  ================================

J3
^^^
====  ====  =====  ================================
序号  名称   类型   功能
====  ====  =====  ================================
1     GND   G      接地
2     TX    I/O/T  GPIO21, U0TXD
3     RX    I/O/T  GPIO20, U0RXD
4     GND   G      接地
5     IO9   I/O/T  GPIO9
6     IO8   I/O/T  GPIO8
7     GND   G      接地
8     IO7   I/O/T  GPIO7, FSPID, MTDO
9     IO6   I/O/T  GPIO6, FSPICLK, MTCK
10    IO5   I/O/T  GPIO5, ADC2_CH0, FSPIWP, MTDI
11    IO4   I/O/T  GPIO4, ADC1_CH4, FSPIHD, MTMS
12    GND   G      接地
13    IO18  I/O/T  GPIO18
14    IO19  I/O/T  GPIO19
15    GND   G      接地
====  ====  =====  ================================


    P：电源；
    I：输入：
    O：输出；
    T：可设置为高阻。

管脚布局
^^^^^^^^
.. figure:: ../../../_static/esp32-c3-devkitm-1-v1-pinout.png
    :align: center
    :alt: ESP32-C3-DevKitM-1 管脚布局
    :figclass: align-center

    ESP32-C3-DevKitM-1 管脚布局

硬件版本
==========

该开发板为最新硬件，尚未有历史版本。

相关文档
========
* `ESP32-C3 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_cn.pdf>`_ (PDF) 
* `ESP32-C3-MINI-1 规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-c3-mini-1_datasheet_cn.pdf>`_ (PDF)
* `ESP32-C3-DevKitM-1 原理图 <https://dl.espressif.com/dl/schematics/SCH_ESP32-C3-DEVKITM-1_V1_20200915A.pdf>`_ (PDF)
* `ESP32-C3-DevKitM-1 PCB 布局图 <https://dl.espressif.com/dl/schematics/PCB_ESP32-C3-DEVKITM-1_V1_20200915AA.pdf>`_ (PDF)
* `ESP32-C3-DevKitM-1 尺寸图 <https://dl.espressif.com/dl/schematics/DIMENSION_ESP32-C3-DEVKITM-1_V1_20200915AA.dxf>`_ (PDF)
* `ESP32-C3-DevKitM-1 尺寸图源文件 <https://dl.espressif.com/dl/schematics/DIMENSION_ESP32-C3-DEVKITM-1_V1_20200915AA.dxf>`_ (DXF) - 可使用 `Autodesk Viewer <https://viewer.autodesk.com/>`_ 查看

有关本开发板的更多设计文档，请联系我们的商务部门 `sales@espressif.com <sales@espressif.com>`_。
