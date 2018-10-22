ESP32-DevKitC V4 入门指南
======================================
:link_to_translation:`en:[English]`

本指南介绍了如何开始使用 ESP32-DevKitC V4 开发板。有关 ESP32-DevKitC 其他版本的介绍，请见：`../hw-reference/index`。

准备工作
-------------

* 1 × :ref:`ESP32-DevKitC V4 开发板 <get-started-esp32-devkitc-board-front>`
* 1 × USB A / micro USB B 电缆 
* 1 × PC（Windows、Linux 或 Mac OS）


简介
--------

ESP32-DevKitC V4 是一款来自 `乐鑫 <https://espressif.com>`_ 的迷你开发板，板上模组的绝大部分管脚均已引出，可根据用户需求，轻松连接多种外围器件。此外，本开发板还采用了标准排针，可便利面包板的使用。

本开发板可支持多款 ESP32 模组，包括 :ref:`esp-modules-and-boards-esp32-wroom-32`，:ref:`ESP32-WROOM-32U <esp-modules-and-boards-esp32-wroom-32d-and-u>`，:ref:`ESP32-WROOM-32D <esp-modules-and-boards-esp32-wroom-32d-and-u>`，:ref:`esp-modules-and-boards-esp32-solo-1` 及 :ref:`esp-modules-and-boards-esp32-wrover` 系列。

功能说明
----------------------
ESP32-DevKitC V4 开发板的主要组件、接口及控制方式如下文所示。

ESP32-WROOM-32
    ESP32-DevKitC V4 开发板上焊接的标准 :ref:`esp-modules-and-boards-esp32-wroom-32` 模组。

额外空间
    本开发板的还预留了部分额外空间，用于焊接其他 ESP32-WROOM-32 之外的较长模组，比如 :ref:`esp-modules-and-boards-esp32-wrover` 模组。

USB-UART 桥接器
    单芯片 USB-UART 桥接器，可提供高达 3 Mbps 的传输速率。

Boot 按键
    按下 **Boot** 键并保持，同时按一下 **EN** 键（此时不要松开 **Boot** 键）进入固件下载模式，通过串口下载固件。

EN 按键
    复位键，可重置系统。

Micro USB 端口
    USB 接口，可用作电路板的供电电源，或连接 PC 端的通信接口。

5V LED 电源指示灯
    更多信息，请见 `相关文档`_ 中的原理图。

I/O 连接器
    ESP32-DevKitC V4 迷你开发板，板上模组的绝大部分管脚均已引出。用户可以对 ESP32 进行编程，实现 PWM、ADC、DAC、I2C、I2S、SPI 等多种功能。
    
    .. note::

        引脚 CLK、D0、D1、D2、D3 和 CMD (GPIO6 - GPIO11) 用于 ESP32-WROOM-32、ESP32-WROOM-32D/U 和 ESP32-SOLO-1 模组的内部 SPI 通信，集中分布在 USB 接口一侧。通常而言，这些引脚最好不连，否则可能影响 SPI flash 内存 / SPI RAM 的工作。 

.. _get-started-esp32-devkitc-board-front:

.. figure:: ../../_static/esp32-devkitc-functional-overview.jpg
    :align: center
    :alt: ESP32-DevKitC V4 with ESP32-WROOM-32 module soldered
    :figclass: align-center

    图为 ESP32-DevKitC V4（贴 ESP32-WROOM-32）


电源选项
--------------------

ESP32-DevKitC V4 支持以下几种供电模式：

1. Micro USB 接口供电（默认）
2. 5V / GND 管脚供电
3. 3V3 / GND 管脚供电

.. warning::

    上述供电模式不可同时连接，否则可能会损坏电路板和/或电源。

C15 相关说明
--------------

较早版本 ESP32-DevKitC 上的 C15 可能带来两个问题：

1. 开发板上电后可能进入下载模式；
2. 如果用户通过 GPIO0 输出时钟，C15 可能会影响时钟输出。

用户如果认为 C15 可能影响开发板的使用，则可以将 C15 完全移除（C15 在开发板上的具体位置见下图黄色部分）。否则，则无需处理 C15。

.. figure:: ../../_static/esp32-devkitc-c15-location.png
    :align: center
    :alt: Location of C15 (colored yellow) on ESP32-DevKitC V4 board
    :figclass: align-center
    :width: 30%

    C15（黄色部分）在 ESP32-DevKitC V4 开发板上的具体位置


应用程序开发
------------------------------

ESP32-DevKitC 上电前，请首先确认电路板完好无损。

有关应用程序开发的具体步骤，请见章节 :doc:`index`：

* :ref:`设置 Toolchain <get-started-setup-toolchain>`，以便用 C 语言开发应用
* :ref:`连接 <get-started-connect>` 模组至 PC，并确认访问状态
* :ref:`构建并向 ESP32 烧录示例 <get-started-build-flash>`
* :ref:`即刻监测 <get-started-build-monitor>` 应用程序的动作


开发板尺寸
----------------

.. figure:: ../../_static/esp32-devkitc-dimensions-back.jpg
    :align: center
    :alt: ESP32-DevKitC board dimensions - back
    :figclass: align-center

    ESP32-DevKitC 开发板尺寸 - 背面


相关文档
-----------------

* `ESP32-DevKitC V4 原理图 <https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch-20180607a.pdf>`_ (PDF)
* `ESP32 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_cn.pdf>`_ (PDF)
* `ESP32-WROOM-32 技术规格书 <https://espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_cn.pdf>`_ (PDF)
* `ESP32-WROOM-32D/U 技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_cn.pdf>`_ (PDF)


.. toctree::
    :hidden:

    get-started-devkitc-v2