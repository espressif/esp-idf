====================
ESP-LyraP-LCD32 v1.1
====================

:link_to_translation:`en:[English]`

本用户指南可提供 ESP-LyraP-LCD32 扩展板的相关信息。

本扩展板通常仅与乐鑫其他开发板一起销售（即 *主板*，比如 ESP32-S2-Kaluga-1），不可单独购买。

目前，ESP-LyraP-CAM v1.1 扩展板正在搭配 :doc:`user-guide-esp32-s2-kaluga-1-kit-v1.2` 销售。

ESP-LyraP-LCD32 可为你的主板增加 LCD 图像显示功能。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-lcd32-v1.1-3d.png
    :align: center
    :width: 2243px
    :height: 1534px
    :scale: 30%
    :alt: ESP-LyraP-LCD32
    :figclass: align-center

    ESP-LyraP-LCD32（点击放大）

本指南包括如下内容：

- `概述`_：提供为了使用 ESP-LyraP-LCD32 而必须了解的硬件和软件信息。
- `硬件参考`_：提供 ESP-LyraP-LCD32 的详细硬件信息。
- `硬件修订历史`_：提供该开发版的“修订历史”、“已知问题”以及此前版本开发板的用户指南链接。
- `相关文档`_：提供相关文档的链接。


概述
====

ESP-LyraP-LCD32 可为你的主板增加一块 3.2" LCD 图形显示屏（320 x 240 分辨率）。该显示屏通过 SPI 总线连接到 ESP32-S2。


组件描述
--------

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-lcd32-v1.1-layout-front.png
    :align: center
    :width: 934px
    :height: 600px
    :scale: 70%
    :alt: ESP-LyraP-LCD32 - 正面
    :figclass: align-center

    ESP-LyraP-LCD32 - 正面（点击放大）


在下面的组件描述中，**保留** 表示该功能可用，但当前版本的套件并未启用该功能。


.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 主要组件
     - 描述
   * - 扩展板排针
     - 连接器排针，用于插入主板上的排母
   * - LCD 显示屏
     - 本版本支持 3.2" 的 SPI LCD 显示模块（320 x 240 分辨率）；显示器驱动（控制器）为 Sitronix ST7789V
   * - 触摸屏开关
     - 暂不支持触摸屏，因此请注意保持关闭，确保相关管脚复用不受影响。
   * - 主板 3.2" LCD FPC 连接器
     - （保留）连接到主板的 3.2" LCD FPC 连接器
   * - 控制开关
     - 打开将 Reset/Backlight_control/CS 设置为默认高电平或低电平；关闭允许释放这些管脚用作它用。


应用程序开发
------------

ESP-LyraP-LCD32 上电前，请首先确认开发板完好无损。


硬件准备
^^^^^^^^

- 带有摄像头扩展板连接器（排母）的主板（例如 ESP32-S2-Kaluga-1、ESP-LyraT-8311A)
- ESP-LyraP-LCD32 扩展板
- 4 x 螺栓，用于保证安装稳定
- PC（Windows、Linux 或 macOS）


硬件设置
^^^^^^^^

请按照以下步骤将 ESP-LyraP-LCD32 安装到带有排母的主板上：

1. 先将 4 个螺栓固定到主板的相应位置上
2. 对齐 ESP-LyraP-LCD32 与主板和螺栓的位置，并小心插入


软件设置
^^^^^^^^

请前往 ESP32-S2-Kaluga-1 开发套件用户指南的 :ref:`user-guide-esp32-s2-kaluga-1-kit-v1.2-software-setup` 章节。


硬件参考
========

功能框图
--------

ESP-LyraP-LCD32 的主要组件和连接方式如下图所示。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-lcd32-v1.1-block-diagram.png
    :align: center
    :alt: ESP-LyraP-LCD32 功能框图
    :figclass: align-center

    ESP-LyraP-LCD32 功能框图


硬件修订历史
============

尚无版本升级历史。


相关文档
========

- `ESP-LyraP-LCD32 原理图 <https://dl.espressif.com/dl/schematics/ESP-LyraP-LCD32_V1_1_SCH_20200324A.pdf>`_ (PDF)
- `ESP-LyraP-LCD32 PCB 布局图 <https://dl.espressif.com/dl/schematics/ESP-LyraP-LCD32_V1_1_PCB_20200324AC.pdf>`_ (PDF)

有关本开发板的更多设计文档，请联系我们的商务部门 sales@espressif.com。
