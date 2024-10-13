===================
ESP32-S3-DevKitM-1
===================

:link_to_translation:`en: [English]`

本指南将帮助你快速上手 ESP32-S3-DevKitM-1，并提供该款开发板的详细信息。

ESP32-S3-DevKitM-1 是一款入门级开发板，搭载的是 Wi-Fi+蓝牙 LE 模组 ESP32-S3-MINI-1 或 ESP32-S3-MINI-1U，该款模组因小尺寸而得名。

板上模组的大部分管脚均已引出至开发板两侧排针，开发人员可根据实际需求，轻松通过跳线连接多种外围设备，也可将开发板插在面包板上使用。

.. figure:: ../../../_static/esp32-s3-devkitm-1-v1-isometric.png
    :align: center
    :scale: 70%
    :alt: ESP32-S3-DevKitM-1（板载 ESP32-S3-MINI-1 模组）
    :figclass: align-center

    ESP32-S3-DevKitM-1（板载 ESP32-S3-MINI-1 模组）


本指南包括如下内容：

- `入门指南`_：简要介绍了开发板和硬件、软件设置指南。
- `硬件参考`_：详细介绍了开发板的硬件。
- `相关文档`_：列出了相关文档的链接。


入门指南
========

本小节将简要介绍 ESP32-S3-DevKitM-1，说明如何在 ESP32-S3-DevKitM-1 上烧录固件及相关准备工作。


组件介绍
--------

.. _user-guide-s3-devkitm-1-v1-board-front:

.. figure:: ../../../_static/ESP32-S3-DevKitM-1_v1-annotated-photo.png
    :align: center
    :alt: ESP32-S3-DevKitM-1 - 正面
    :figclass: align-center

    ESP32-S3-DevKitM-1 - 正面

以下按照逆时针的顺序依次介绍开发板上的主要组件。

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 主要组件
     - 介绍
   * - ESP32-S3-MINI-1/1U
     - ESP32-S3-MINI-1 和 ESP32-S3-MINI-1U 是通用型 Wi-Fi + 低功耗蓝牙 MCU 模组，具有丰富的外设接口。ESP32-S3-MINI-1 采用 PCB 板载天线，ESP32-S3-MINI-1U 采用连接器连接外部天线。两款模组的核心是 ESP32-S3FN8 芯片。该芯片带有 8 MB flash，由于 flash 直接封装在芯片中，因此 ESP32-S3-MINI-1/1U 模组具有较小的封装尺寸。
   * - 5 V to 3.3 V LDO（5 V 转 3.3 V LDO）
     - 电源转换器，输入 5 V，输出 3.3 V。
   * - Pin Headers（排针）
     - 所有可用 GPIO 管脚（除 flash 的 SPI 总线）均已引出至开发板的排针。请查看 :ref:`user-guide-s3-DevKitM-1-header-blocks` 获取更多信息。
   * - USB-to-UART Port（USB 转 UART 接口）
     - Micro-USB 接口，可用作开发板的供电接口，可烧录固件至芯片，也可作为通信接口，通过板载 USB 转 UART 桥接器与芯片通信。
   * - Boot Button（Boot 键）
     - 下载按键。按住 **Boot** 键的同时按一下 **Reset** 键进入“固件下载”模式，通过串口下载固件。
   * - Reset Button（Reset 键）
     - 复位按键。
   * - ESP32-S3 USB Port（ESP32-S3 USB 接口）
     - ESP32-S3 USB OTG 接口，支持全速 USB 1.1 标准。ESP32-S3 USB 接口可用作开发板的供电接口，可烧录固件至芯片，可通过 USB 协议与芯片通信，也可用于 JTAG 调试。
   * - USB-to-UART Bridge（USB 转 UART 桥接器）
     - 单芯片 USB 至 UART 桥接器，可提供高达 3 Mbps 的传输速率。
   * - RGB LED
     - 可寻址 RGB 发光二极管，由 GPIO48 驱动。
   * - 3.3 V Power On LED（3.3 V 电源指示灯）
     - 开发板连接 USB 电源后，该指示灯亮起。


开始开发应用
-------------

通电前，请确保开发板完好无损。


必备硬件
^^^^^^^^

- ESP32-S3-DevKitM-1
- USB 2.0 数据线（标准 A 型转 Micro-B 型）
- 电脑（Windows、Linux 或 macOS）

.. 注解::

  请确保使用适当的 USB 数据线。部分数据线仅可用于充电，无法用于数据传输和编程。


硬件设置
^^^^^^^^

通过 **USB 转 UART 接口** 或 **ESP32-S3 USB 接口** 连接开发板与电脑。在后续步骤中，默认使用 **USB 转 UART 接口**。


软件设置
^^^^^^^^

请前往 :doc:`../../get-started/index`，在 :ref:`get-started-step-by-step` 小节查看如何快速设置开发环境，将应用程序烧录至你的开发板。


内含组件和包装
---------------

零售订单
^^^^^^^^

如购买样品，每个开发板将以防静电袋或零售商选择的其他方式包装。

零售订单请前往 https://www.espressif.com/zh-hans/company/contact/buy-a-sample。


批量订单
^^^^^^^^

如批量购买，开发板将以大纸板箱包装。

批量订单请前往 https://www.espressif.com/zh-hans/contact-us/sales-questions。


硬件参考
========

功能框图
--------

ESP32-S3-DevKitM-1 的主要组件和连接方式如下图所示。

.. figure:: ../../../_static/ESP32-S3-DevKitM-1_v1_SystemBlock.png
    :align: center
    :scale: 70%
    :alt: ESP32-S3-DevKitM-1（点击放大）
    :figclass: align-center

    ESP32-S3-DevKitM-1（点击放大）


电源选项
^^^^^^^^

以下任一供电方式均可给开发板供电：

- USB 转 UART 接口供电或 ESP32-S3 USB 接口供电（选择其一或同时供电），默认供电方式（推荐）
- 5V 和 G (GND) 排针供电
- 3V3 和 G (GND) 排针供电


.. _user-guide-s3-DevKitM-1-header-blocks:

排针
----

下表列出了开发板两侧排针（J1 和 J3）的 **名称** 和 **功能**，排针的名称如图 :ref:`user-guide-s3-devkitm-1-v1-board-front` 所示，排针的序号与 `开发板原理图 <https://dl.espressif.com/dl/schematics/SCH_ESP32-S3-DEVKITM-1_V1_20210310A.pdf>`_ (PDF) 一致。


J1
^^^

====  ====  ==========  =====================================================================
序号  名称   类型 [#]_    功能
====  ====  ==========  =====================================================================
1     3V3   P           3.3 V 电源
2     0     I/O/T       RTC_GPIO0, GPIO0
3     1     I/O/T       RTC_GPIO1, GPIO1, TOUCH1, ADC1_CH0
4     2     I/O/T       RTC_GPIO2, GPIO2, TOUCH2, ADC1_CH1
5     3     I/O/T       RTC_GPIO3, GPIO3, TOUCH3, ADC1_CH2
6     4     I/O/T       RTC_GPIO4, GPIO4, TOUCH4, ADC1_CH3
7     5     I/O/T       RTC_GPIO5, GPIO5, TOUCH5, ADC1_CH4
8     6     I/O/T       RTC_GPIO6, GPIO6, TOUCH6, ADC1_CH5
9     7     I/O/T       RTC_GPIO7, GPIO7, TOUCH7, ADC1_CH6
10    8     I/O/T       RTC_GPIO8, GPIO8, TOUCH8, ADC1_CH7, SUBSPICS1
11    9     I/O/T       RTC_GPIO9, GPIO9, TOUCH9, ADC1_CH8, FSPIHD, SUBSPIHD
12    10    I/O/T       RTC_GPIO10, GPIO10, TOUCH10, ADC1_CH9, FSPICS0, FSPIIO4, SUBSPICS0
13    11    I/O/T       RTC_GPIO11, GPIO11, TOUCH11, ADC2_CH0, FSPID, FSPIIO5, SUBSPID
14    12    I/O/T       RTC_GPIO12, GPIO12, TOUCH12, ADC2_CH1, FSPICLK, FSPIIO6, SUBSPICLK
15    13    I/O/T       RTC_GPIO13, GPIO13, TOUCH13, ADC2_CH2, FSPIQ, FSPIIO7, SUBSPIQ
16    14    I/O/T       RTC_GPIO14, GPIO14, TOUCH14, ADC2_CH3, FSPIWP, FSPIDQS, SUBSPIWP
17    15    I/O/T       RTC_GPIO15, GPIO15, U0RTS, ADC2_CH4, XTAL_32K_P
18    16    I/O/T       RTC_GPIO16, GPIO16, U0CTS, ADC2_CH5, XTAL_32K_N
19    17    I/O/T       RTC_GPIO17, GPIO17, U1TXD, ADC2_CH6
20    18    I/O/T       RTC_GPIO18, GPIO18, U1RXD, ADC2_CH7, CLK_OUT3
21    5V    P           5 V 电源
22    G     G           接地
====  ====  ==========  =====================================================================


J3
^^^

====  ====  ==========  =====================================================================
序号  名称   类型          功能
====  ====  ==========  =====================================================================
1     G     G           接地
2     RST   I           EN
3     46    I/O/T       GPIO46
4     45    I/O/T       GPIO45
5     RX    I/O/T       U0RXD, GPIO44, CLK_OUT2
6     TX    I/O/T       U0TXD, GPIO43, CLK_OUT1
7     42    I/O/T       MTMS, GPIO42
8     41    I/O/T       MTDI, GPIO41, CLK_OUT1
9     40    I/O/T       MTDO, GPIO40, CLK_OUT2
10    39    I/O/T       MTCK, GPIO39, CLK_OUT3, SUBSPICS1
11    38    I/O/T       GPIO38, FSPIWP, SUBSPIWP
12    37    I/O/T       SPIDQS, GPIO37, FSPIQ, SUBSPIQ
13    36    I/O/T       SPIIO7, GPIO36, FSPICLK, SUBSPICLK
14    35    I/O/T       SPIIO6, GPIO35, FSPID, SUBSPID
15    34    I/O/T       SPIIO5, GPIO34, FSPICS0, SUBSPICS0
16    33    I/O/T       SPIIO4, GPIO33, FSPIHD, SUBSPIHD
17    26    I/O/T       SPICS1, GPIO26
18    21    I/O/T       RTC_GPIO21, GPIO21
19    20    I/O/T       RTC_GPIO20, GPIO20, U1CTS, ADC2_CH9, CLK_OUT1, USB_D+
20    19    I/O/T       RTC_GPIO19, GPIO19, U1RTS, ADC2_CH8, CLK_OUT2, USB_D-
21    48    I/O/T       SPICLK_N, GPIO48, SUBSPICLK_N_DIFF, RGB LED
22    47    I/O/T       SPICLK_P, GPIO47, SUBSPICLK_P_DIFF
====  ====  ==========  =====================================================================


.. [#] P：电源；I：输入；O：输出；T：可设置为高阻。



有关管脚功能名称的解释，请参考 `ESP32-S3 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_cn.pdf>`_ (PDF)。


管脚布局
^^^^^^^^^^^

.. figure:: ../../../_static/ESP32-S3_DevKitM-1_pinlayout.jpg
    :align: center
    :scale: 50%
    :alt: ESP32-S3-DevKitM-1（点击放大）
    :figclass: align-center

    ESP32-S3-DevKitM-1 管脚布局（点击放大）


硬件版本
============

无历史版本。


相关文档
========

- `ESP32-S3 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_cn.pdf>`_ (PDF)
- `ESP32-S3-MINI-1 & ESP32-S3-MINI-1U 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s3-mini-1_mini-1u_datasheet_cn.pdf>`_ (PDF)
- `ESP32-S3-DevKitM-1 原理图 <https://dl.espressif.com/dl/schematics/SCH_ESP32-S3-DEVKITM-1_V1_20210310A.pdf>`_ (PDF)
- `ESP32-S3-DevKitM-1 PCB 布局图 <https://dl.espressif.com/dl/schematics/PCB_ESP32-S3-DevKitM-1_V1_20210310AC.pdf>`_ (PDF)
- `ESP32-S3-DevKitM-1 尺寸图 <https://dl.espressif.com/dl/schematics/DXF_ESP32-S3-DevKitM-1_V1_20210310AC.pdf>`_ (PDF)
- `ESP32-S3-DevKitM-1 尺寸图源文件 <https://dl.espressif.com/dl/schematics/DXF_ESP32-S3-DevKitM-1_V1_20210310AC.dxf>`_ (DXF) - 可使用 `Autodesk Viewer <https://viewer.autodesk.com/>`_ 查看

有关本开发板的更多设计文档，请联系我们的商务部门 `sales@espressif.com <sales@espressif.com>`_。
