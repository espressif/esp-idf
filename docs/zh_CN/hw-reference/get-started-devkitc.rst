ESP32-DevKitC V4 入门指南
=============================================

:link_to_translation:`en: [English]`

本指南介绍了如何开始使用 ESP32-DevKitC V4 开发板。有关 ESP32-DevKitC 其他版本的介绍，请见：:doc:`../hw-reference/index`。


准备工作
-----------

* :ref:`ESP32-DevKitC V4 开发板 <get-started-esp32-devkitc-board-front>`
* USB A / micro USB B 数据线
* PC（Windows、Linux 或 Mac OS）

您可以跳过介绍部分，直接前往 `应用程序开发`_ 章节。


.. _DevKitC-Overview:

概述
----

ESP32-DevKitC V4 是 `乐鑫 <https://espressif.com>`_ 一款基于 ESP32 的小型开发板，板上模组的绝大部分管脚均已引出，开发人员可根据实际需求，轻松通过跳线连接多种外围器件，或将开发板插在面包板上使用。

为了更好地满足不同用户需求，ESP32-DevKitC V4 还支持以下不同配置：

- 可选多款 ESP32 模组

	- :ref:`esp-modules-and-boards-esp32-wroom-32`
	- :ref:`ESP32-WROOM-32D <esp-modules-and-boards-esp32-wroom-32d-and-u>`
	- :ref:`ESP32-WROOM-32U <esp-modules-and-boards-esp32-wroom-32d-and-u>`
	- :ref:`esp-modules-and-boards-esp32-solo-1`
	- :ref:`ESP32-WROVER <esp-modules-and-boards-esp32-wrover>`
	- :ref:`ESP32-WROVER-B <esp-modules-and-boards-esp32-wrover>`
	- :ref:`ESP32-WROVER-I <esp-modules-and-boards-esp32-wrover>`
	- :ref:`ESP32-WROVER-I (IPEX) <esp-modules-and-boards-esp32-wrover>`

- 可选排针或排母

详情请见 `《乐鑫产品订购信息》 <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_cn.pdf>`_。


功能说明
---------

ESP32-DevKitC V4 开发板的主要组件、接口及控制方式见下。

.. _get-started-esp32-devkitc-board-front:

.. figure:: ../../_static/esp32-devkitc-functional-overview.jpg
	:align: center
	:alt: ESP32-DevKitC V4（板载 ESP32-WROOM-32）
	:figclass: align-center

ESP32-DevKitC V4（板载 ESP32-WROOM-32）


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
    * - USB-to-UART 桥接器 | 单芯片 USB-UART 桥接器，可提供高达 3 Mbps 的传输速率。
      - Micro USB 端口
    * - USB 接口。
      - 可用作电路板的供电电源，或连接 PC 和 ESP32-WROOM-32 模组的通信接口。
    * - 5V Power On LED
      - 开发板通电后（USB 或外部 5 V），该指示灯将亮起。更多信息，请见 `相关文档`_ 中的原理图。
    * - I/O
      - 板上模组的绝大部分管脚均已引出至开发板的排针。用户可以对 ESP32 进行编程，实现 PWM、ADC、DAC、I2C、I2S、SPI 等多种功能。

.. note::

	管脚 D0、D1、D2、D3、CMD 和 CLK 用于 ESP32 芯片与 SPI flash 间的内部通信，集中分布在开发板两侧靠近 USB 端口的位置。通常而言，这些管脚最好不连，否则可能影响 SPI flash / SPI RAM 的工作。 

.. note::

	管脚 GPIO16 和 GPIO17 仅适用于板载 ESP32-WROOM 系列和 ESP32-SOLO-1 的开发板，保留内部使用。


电源选项
--------

开发板可从以下三种供电方式中任选其一：

* Micro USB 供电（默认）
* 5V / GND 管脚供电
* 3V3 / GND 管脚供电

.. warning::

	上述供电模式 **不可同时连接**，否则可能会损坏开发板和/或电源。


有关 C15 的提示
--------------------

较早版本 ESP32-DevKitC 开发板上的 C15 可能存在以下问题：

* 开发板上电后可能进入下载模式；
* 如果用户通过 GPIO0 输出时钟，C15 可能会影响信号。

用户如果认为 C15 可能影响开发板的使用，则可以将 C15 完全移除。C15 在开发板上的具体位置见下图黄色部分。

.. figure:: ../../_static/esp32-devkitc-c15-location.png
	:align: center
	:alt: C15（黄色）在 ESP32-DevKitC V4 开发板上的位置
	:figclass: align-center
	:width: 30%

C15（黄色）在 ESP32-DevKitC V4 开发板上的位置


应用程序开发
-----------------

ESP32-DevKitC V4 上电前，请首先确认开发板完好无损。

现在，请前往 :doc:`index` 中的 :ref:`get-started-step-by-step` 章节，查看如何设置开发环境，并尝试将示例项目烧录至您的开发板。

开发板尺寸
-------------

.. figure:: ../../_static/esp32-devkitc-dimensions-back.jpg
	:align: center
	:alt: ESP32-DevKitC 开发板尺寸 -- 仰视图
	:figclass: align-center

ESP32-DevKitC 开发板尺寸 -- 仰视图


相关文档
-----------

* `ESP32-DevKitC V4 原理图 <https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch.pdf>`_ (PDF)
* `《ESP32 技术规格书》 <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_cn.pdf>`_ (PDF)
* `《ESP32-WROOM-32 技术规格书》 <https://espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_cn.pdf>`_ (PDF)
* `《ESP32-WROOM-32D & ESP32-WROOM-32U 技术规格书》 <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_cn.pdf>`_ (PDF)
* `《ESP32-WROVER 技术规格书》 <https://espressif.com/sites/default/files/documentation/esp32_wrover_datasheet_cn.pdf>`_ (PDF)
* `《ESP32-WROVER-B 技术规格书》 <https://www.espressif.com/sites/default/files/documentation/esp32-wrover-b_datasheet_cn.pdf>`_ (PDF)
* `《乐鑫产品订购信息》 <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_cn.pdf>`_ (PDF)

.. toctree::
	:hidden:

	get-started-devkitc-v2
