ESP32-DevKitC V2 入门指南
==================================

:link_to_translation:`en: [English]`

本指南介绍了如何开始使用 ESP32-DevKitC V2 开发板。


准备工作
--------

* :ref:`ESP32-DevKitC V2 开发板 <get-started-esp32-devkitc-v2-board-front>`
* USB A / micro USB B 数据线
* PC（Windows、Linux 或 Mac OS）

您可以跳过介绍部分，直接前往 `应用程序开发`_ 章节。


概述
----

ESP32-DevKitC V2 是 `乐鑫 <https://espressif.com>`_ 一款基于 ESP32 的小型开发板，板上模组的绝大部分管脚均已引出，开发人员可根据实际需求，轻松通过跳线连接多种外围器件，或将开发板插在面包板上使用。


功能说明
--------

ESP32-DevKitC V2 开发板的主要组件、接口及控制方式见下。

.. _get-started-esp32-devkitc-v2-board-front:

.. figure:: ../../_static/esp32-devkitc-v2-functional-overview.png
    :align: center
    :alt: ESP32-DevKitC V2 开发板
    :figclass: align-center



ESP32-DevKitC V2 开发板

.. list-table:: 
    :widths: 25 75
    :header-rows: 1

    * - 主要组件
      - 基本介绍
    * - ESP32-WROOM-32
      - 基于 ESP32 的模组。更多详情，请见 `《ESP32-WROOM-32 技术规格书》`_。
    * - EN
      - 复位按键。
    * - Boot
      - 下载按键。按下 **Boot** 键并保持，同时按一下 **EN** 键（此时不要松开 **Boot** 键）进入“固件下载”模式，通过串口下载固件。
    * - Micro USB 端口
      - USB 接口。可用作电路板的供电电源，或连接 PC 和 ESP32-WROOM-32 模组的通信接口。
    * - I/O
      - 板上模组的绝大部分管脚均已引出至开发板的排针。用户可以对 ESP32 进行编程，实现 PWM、ADC、DAC、I2C、I2S、SPI 等多种功能。

电源选项
--------

开发板可从以下三种供电方式中任选其一：

* Micro USB 供电（默认）
* 5V / GND 管脚供电
* 3V3 / GND 管脚供电

.. warning::

    上述供电模式 **不可同时连接**，否则可能会损坏开发板和/或电源。


应用程序开发
------------

ESP32-DevKitC V2 上电前，请首先确认开发板完好无损。

现在，请前往 :doc:`index` 中的 :ref:`get-started-step-by-step` 章节，查看如何设置开发环境，并尝试将示例项目烧录至您的开发板。


相关文档
--------

* `ESP32-DevKitC 原理图 <https://dl.espressif.com/dl/schematics/ESP32-Core-Board-V2_sch.pdf>`_ (PDF)
* `《ESP32 技术规格书》 <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_cn.pdf>`_ (PDF)
* `《ESP32-WROOM-32 技术规格书》 <https://espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_cn.pdf>`_ (PDF)

