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

- `入门指南`_: 简要介绍了 ESP32-S2-Saola-1 和硬件、软件设置指南。
- `硬件参考`_: 详细介绍了 ESP32-S2-Saola-1 的硬件。
- `相关文档`_: 列出了相关文档的链接。

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

如购买样品，每个 ESP32-S2-Saola-1 底板将以防静电袋或零售商选择的其他方式包装。

零售订单请前往 https://www.espressif.com/zh-hans/company/contact/buy-a-sample。

批量订单
^^^^^^^^

如批量购买，ESP32-S2-Saola-1 烧录底板将以大纸板箱包装。

批量订单请参考 `乐鑫产品订购信息 <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_cn.pdf>`__ (PDF)。

组件介绍
--------

.. figure:: ../../../_static/esp32-s2-saola-1-v1.2-annotated-photo.png
    :align: center
    :alt: ESP32-S2-Saola-1 - 正面
    :figclass: align-center

    ESP32-S2-Saola-1 - 正面

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 主要组件
     - 介绍
   * - ESP32-S2-WROVER
     - ESP32-S2-WROVER 集成 ESP32-S2，是通用型 Wi-Fi MCU 模组，功能强大。该模组采用 PCB 板载天线，配置了 4 MB SPI flash 和 2 MB SPI PSRAM。
   * - I/O 连接器
     - 所有可用 GPIO 管脚（除 Flash 和 PSRAM 的 SPI 总线）均已引出至开发板的排针。用户可对 ESP32-S2 芯片编程，使能 SPI、I2S、UART、I2C、触摸传感器、PWM 等多种功能。
   * - USB 至 UART 桥接器
     - 单芯片 USB 至 UART 桥接器，可提供高达 3 Mbps 的传输速率。
   * - Boot 键。
     - 下载按键。按住 Boot 键的同时按一下 EN 键进入“固件下载”模式，通过串口下载固件。
   * - Reset 键。
     - 复位按键。
   * - 3.3 V 电源指示灯
     - 开发板连接 USB 电源后，该指示灯亮起。
   * - Micro-USB 接口
     - USB 接口。可用作开发板的供电电源或 PC 和 ESP32-S2 芯片的通信接口。
   * - RGB LED
     - 可寻址 RGB 发光二极管 (WS2812)，由 GPIO18 驱动。

开始开发应用
------------

通电前，请确保 ESP32-S2-Saola-1 完好无损。

必备硬件
^^^^^^^^

- ESP32-S2-Saola-1
- USB 2.0 数据线（标准 A 型转 Micro-B型）
- 电脑 （Windows、Linux 或 macOS）

软件设置
^^^^^^^^

请前往 :doc:`../../get-started/index`，在 :ref:`get-started-step-by-step` 一节查看如何快速设置开发环境，将应用程序烧录至 ESP32-S2-Saola-1。

.. 注解::
    
    ESP32-S2 仅支持 ESP-IDF master 分支或 v4.2 以上版本。

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

您可从以下三种供电方式中任选其一给 ESP32-S2-Saola-1 供电：

- Micro USB 端口供电（默认）
- 5V 和 GND 管脚供电
- 3V3 和 GND 管脚供电

建议选择第一种供电方式：Micro USB 端口供电。

相关文档
========
* `ESP32-S2-Saola-1 原理图 <https://dl.espressif.com/dl/schematics/ESP32-S2-SAOLA-1_V1.1_schematics.pdf>`_ (PDF)
* `ESP32-S2 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_cn.pdf>`_ (PDF)
* `ESP32-S2-WROVER & ESP32-S2-WROVER-I 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wrover_esp32-s2-wrover-i_datasheet_cn.pdf>`_ (PDF)
* `ESP32-S2-WROOM & ESP32-S2-WROOM-I 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wroom_esp32-s2-wroom-i_datasheet_cn.pdf>`_ (PDF)
* `乐鑫产品订购信息 <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_cn.pdf>`_ (PDF) 
