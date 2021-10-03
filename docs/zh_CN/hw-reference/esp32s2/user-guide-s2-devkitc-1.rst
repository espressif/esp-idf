===================
ESP32-S2-DevKitC-1
===================

:link_to_translation:`en: [English]`

本指南将帮助您快速上手 ESP32-S2-DevKitC-1，并提供该款开发板的详细信息。

ESP32-S2-DevKitC-1 是一款入门级开发板，使用带有 4 MB SPI flash 的 ESP32-S2-SOLO（板载 PCB 天线）或 ESP32-S2-SOLO-U（外部天线连接器）模组。该款开发板具备完整的 Wi-Fi 功能。

板上模组大部分管脚均已引出至两侧排针，开发人员可根据实际需求，轻松通过跳线连接多种外围设备，同时也可将开发板插在面包板上使用。

.. figure:: ../../../_static/esp32-s2-devkitc-1-v1-isometric.png
    :align: center
    :alt: ESP32-S2-DevKitC-1（板载 ESP32-S2-SOLO 模组）
    :figclass: align-center

    ESP32-S2-DevKitC-1（板载 ESP32-S2-SOLO 模组）

本指南包括如下内容：

- `入门指南`_：简要介绍了 ESP32-S2-DevKitC-1 和硬件、软件设置指南。
- `硬件参考`_：详细介绍了 ESP32-S2-DevKitC-1 的硬件。
- `硬件版本`_：介绍硬件历史版本和已知问题，并提供链接至历史版本开发板的入门指南（如有）。
- `相关文档`_：列出了相关文档的链接。

入门指南
========

本小节将简要介绍 ESP32-S2-DevKitC-1，说明如何在 ESP32-S2-DevKitC-1 上烧录固件及相关准备工作。

组件介绍
--------

.. _user-guide-s2-devkitc-1-v1-board-front:

.. figure:: ../../../_static/esp32-s2-devkitc-1-v1-annotated-photo.png
    :align: center
    :alt: ESP32-S2-DevKitC-1 - 正面
    :figclass: align-center

    ESP32-S2-DevKitC-1 - 正面

以下按照顺时针的顺序依次介绍开发板上的主要组件。 

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 主要组件
     - 介绍
   * - ESP32-S2-SOLO 或 ESP32-S2-SOLO-U
     - ESP32-S2-SOLO 和 ESP32-S2-SOLO-U 是两款通用型 Wi-Fi 模组。ESP32-S2-SOLO 采用 PCB 板载天线，ESP32-S2-SOLO-U 采用连接器连接外部天线。开发板上的 ESP32-S2-SOLO 或 ESP32-S2-SOLO-U 模组可配置 4 MB flash，也可配置 4 MB flash 加 2 MB PSRAM（芯片内置）。
   * - 3.3 V Power On LED（3.3 V 电源指示灯）
     - 开发板连接 USB 电源后，该指示灯亮起。
   * - USB-to-UART Bridge（USB 转 UART 桥接器）
     - 单芯片 USB 转 UART 桥接器，可提供高达 3 Mbps 的传输速率。
   * - Pin Headers（排针）
     - 所有可用 GPIO 管脚（除 flash 的 SPI 总线）均已引出至开发板的排针。请查看 :ref:`user-guide-s2-devkitc-1-v1-header-blocks` 获取更多信息。
   * - ESP32-S2 USB Port（ESP32-S2 USB 接口）
     - ESP32-S2 USB OTG 接口，支持全速 USB 1.1 标准。该接口可用作开发板的供电接口，可烧录固件至芯片，也可通过 USB 协议与芯片通信。
   * - Reset Button（Reset 键）
     - 复位按键。
   * - Boot Button（Boot 键）
     - 下载按键。按住 **Boot** 键的同时按一下 **Reset** 键进入“固件下载”模式，通过串口下载固件。
   * - USB-to-UART Port（USB 转 UART 接口）
     - Micro-USB 接口，可用作开发板的供电接口，可烧录固件至芯片，也可作为通信接口，通过板载 USB 转 UART 桥接器与 ESP32-S2 芯片通信。
   * - RGB LED
     - 可寻址 RGB 发光二极管，由 GPIO18 驱动。
   * - 5 V to 3.3 V LDO（5 V 转 3.3 V LDO）
     - 电源转换器，输入 5 V，输出 3.3 V。


开始开发应用
------------

通电前，请确保 ESP32-S2-DevKitC-1 完好无损。

必备硬件
^^^^^^^^

- ESP32-S2-DevKitC-1
- USB 2.0 数据线（标准 A 型转 Micro-B 型）
- 电脑 （Windows、Linux 或 macOS）

.. 注解::

  请确保使用适当的 USB 数据线。部分数据线仅可用于充电，无法用于数据传输和编程。

硬件设置
^^^^^^^^

通过 **USB 转 UART 接口** 连接开发板与电脑。目前有关 **ESP32-S2 USB 接口** 连接的文档尚不完善。在后续步骤中，默认使用 **USB 转 UART 接口**。

软件设置
^^^^^^^^

请前往 `ESP-IDF 快速入门 <https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32s2/get-started/index.html>`_，在 `详细安装步骤 <https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32s2/get-started/index.html#get-started-step-by-step>`_ 小节查看如何快速设置开发环境，将应用程序烧录至 ESP32-S2-DevKitC-1。

内含组件和包装
--------------

零售订单
^^^^^^^^

如购买样品，每个 ESP32-S2-DevKitC-1 将以防静电袋或零售商选择的其他方式包装。

零售订单请前往 https://www.espressif.com/zh-hans/company/contact/buy-a-sample。

批量订单
^^^^^^^^

如批量购买，ESP32-S2-DevKitC-1 将以大纸板箱包装。

批量订单请前往 https://www.espressif.com/zh-hans/contact-us/sales-questions。

硬件参考
========

功能框图
--------

ESP32-S2-DevKitC-1 的主要组件和连接方式如下图所示。

.. figure:: ../../../_static/esp32-s2-devkitc-1-v1-block-diags.png
    :align: center
    :scale: 70%
    :alt: ESP32-S2-DevKitC-1（点击放大）
    :figclass: align-center

    ESP32-S2-DevKitC-1（点击放大）

电源选项
^^^^^^^^

您可从以下三种供电方式中任选其一给 ESP32-S2-DevKitC-1 供电：

- USB 转 UART 接口供电或 ESP32-S2 USB 接口供电（选择其一或同时供电），默认供电方式（推荐）
- 5V 和 G (GND) 排针供电
- 3V3 和 G (GND) 排针供电

.. _user-guide-s2-devkitc-1-v1-header-blocks:

排针 
---- 
 
下表列出了开发板两侧排针（J1 和 J3）的 **名称** 和 **功能**，排针的名称如图 :ref:`user-guide-s2-devkitc-1-v1-board-front` 所示，排针的序号与 `ESP32-S2-DevKitC-1 原理图 <https://dl.espressif.com/dl/schematics/SCH_ESP32-S2-DEVKITC-1_V1_20210508.pdf>`_ (PDF) 一致。 

J1 
^^^ 
==== ====  =========   =========================================================================
序号 名称  类型 [#]_    功能
==== ====  =========   =========================================================================
1    3V3    P          3.3 V 电源
2    3V3    P          3.3 V 电源
3    RST    I          CHIP_PU
4    4      I/O/T      RTC_GPIO4, GPIO4, TOUCH4, ADC1_CH3
5    5      I/O/T      RTC_GPIO5, GPIO5, TOUCH5, ADC1_CH4
6    6      I/O/T      RTC_GPIO6, GPIO6, TOUCH6, ADC1_CH5
7    7      I/O/T      RTC_GPIO7, GPIO7, TOUCH7, ADC1_CH6
8    15     I/O/T      RTC_GPIO15, GPIO15, U0RTS, ADC2_CH4, XTAL_32K_P
9    16     I/O/T      RTC_GPIO16, GPIO16, U0CTS, ADC2_CH5, XTAL_32K_N
10   17     I/O/T      RTC_GPIO17, GPIO17, U1TXD, ADC2_CH6, DAC_1
11   18     I/O/T      RTC_GPIO18, GPIO18, U1RXD, ADC2_CH7, DAC_2, CLK_OUT3, RGB LED
12   8      I/O/T      RTC_GPIO8, GPIO8, TOUCH8, ADC1_CH7
13   3      I/O/T      RTC_GPIO3, GPIO3, TOUCH3, ADC1_CH2
14   46     I          GPIO46
15   9      I/O/T      RTC_GPIO9, GPIO9, TOUCH9, ADC1_CH8, FSPIHD
16   10     I/O/T      RTC_GPIO10, GPIO10, TOUCH10, ADC1_CH9, FSPICS0, FSPIIO4
17   11     I/O/T      RTC_GPIO11, GPIO11, TOUCH11, ADC2_CH0, FSPID, FSPIIO5
18   12     I/O/T      RTC_GPIO12, GPIO12, TOUCH12, ADC2_CH1, FSPICLK, FSPIIO6
19   13     I/O/T      RTC_GPIO13, GPIO13, TOUCH13, ADC2_CH2, FSPIQ, FSPIIO7
20   14     I/O/T      RTC_GPIO14, GPIO14, TOUCH14, ADC2_CH3, FSPIWP, FSPIDQS
21   5V     P          5 V 电源
22   G      G          接地
==== ====  =========   =========================================================================
 
J3 
^^^ 

====  ====  =====  ================================================================ 
序号  名称   类型   功能
====  ====  =====  ================================================================
1     G     G      接地
2     TX    I/O/T  U0TXD, GPIO43, CLK_OUT1
3     RX    I/O/T  U0RXD, GPIO44, CLK_OUT2
4     1     I/O/T  RTC_GPIO1, GPIO1, TOUCH1, ADC1_CH0
5     2     I/O/T  RTC_GPIO2, GPIO2, TOUCH2, ADC1_CH1
6     42    I/O/T  MTMS, GPIO42
7     41    I/O/T  MTDI, GPIO41, CLK_OUT1
8     40    I/O/T  MTDO, GPIO40, CLK_OUT2
9     39    I/O/T  MTCK, GPIO39, CLK_OUT3
10    38    I/O/T  GPIO38, FSPIWP
11    37    I/O/T  SPIDQS, GPIO37, FSPIQ
12    36    I/O/T  SPIIO7, GPIO36, FSPICLK
13    35    I/O/T  SPIIO6, GPIO35, FSPID
14    0     I/O/T  RTC_GPIO0, GPIO0
15    45    I/O/T  GPIO45
16    34    I/O/T  SPIIO5, GPIO34, FSPICS0
17    33    I/O/T  SPIIO4, GPIO33, FSPIHD
18    21    I/O/T  RTC_GPIO21, GPIO21
19    20    I/O/T  RTC_GPIO20, GPIO20, U1CTS, ADC2_CH9, CLK_OUT1, USB_D+
20    19    I/O/T  RTC_GPIO19, GPIO19, U1RTS, ADC2_CH8, CLK_OUT2, USB_D-
21    G     G      接地
22    G     G      接地
====  ====  =====  ================================================================ 

.. [#] P：电源；I：输入；O：输出；T：可设置为高阻。 

管脚布局
^^^^^^^^
.. figure:: ../../../_static/esp32-s2-devkitc-1-v1-pinout.png
    :align: center
    :scale: 15%
    :alt: ESP32-S2-DevKitC-1 管脚布局（点击放大）
    :figclass: align-center

    ESP32-S2-DevKitC-1 管脚布局（点击放大）

硬件版本
==========

无历史版本。

相关文档
========
* `ESP32-S2 系列芯片规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_cn.pdf>`_ (PDF)
* `ESP32-S2-SOLO & ESP32-S2-SOLO-U 模组技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s2-solo_esp32-s2-solo-u_datasheet_cn.pdf>`_ (PDF)
* `ESP32-S2-DevKitC-1 原理图 <https://dl.espressif.com/dl/schematics/SCH_ESP32-S2-DEVKITC-1_V1_20210508.pdf>`_ (PDF)
* `ESP32-S2-DevKitC-1 PCB 布局图 <https://dl.espressif.com/dl/schematics/PCB_ESP32-S2-DevKitC-1_V1_20210508.pdf>`_ (PDF)
* `ESP32-S2-DevKitC-1 尺寸图 <https://dl.espressif.com/dl/schematics/DXF_ESP32-S2-DevKitC-1_V1_20210511.pdf>`_ (PDF)
* `ESP32-S2-DevKitC-1 尺寸图源文件 <https://dl.espressif.com/dl/schematics/DXF_ESP32-S2-DevKitC-1_V1_20210511.dxf>`_ (DXF) - 可使用 `Autodesk Viewer <https://viewer.autodesk.com/>`_ 查看

有关本开发板的更多设计文档，请联系我们的商务部门 `sales@espressif.com <sales@espressif.com>`_。
