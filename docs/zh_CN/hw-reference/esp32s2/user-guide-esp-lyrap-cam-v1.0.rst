==================
ESP-LyraP-CAM v1.0
==================

:link_to_translation:`en:[English]`

本用户指南可提供 ESP-LyraP-CAM 扩展板的相关信息。

本扩展板通常仅与乐鑫其他开发板一起销售（即 *主板*，比如 ESP32-S2-Kaluga-1），不可单独购买。

目前，ESP-LyraP-CAM v1.0 扩展板正在搭配 :doc:`user-guide-esp32-s2-kaluga-1-kit-v1.2` 销售。

ESP-LyraP-CAM 可为你的主板增加摄像头功能。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-cam-v1.0-3d.png
    :align: center
    :alt: ESP-LyraP-CAM
    :figclass: align-center

    ESP-LyraP-CAM

本指南包括如下内容：

- `概述`_：提供为了使用 ESP-LyraP-CAM 而必须了解的硬件和软件信息。
- `硬件参考`_：提供 ESP-LyraP-CAM 的详细硬件信息。
- `硬件修订历史`_：提供该开发版的“修订历史”、“已知问题”以及此前版本开发板的用户指南链接。
- `相关文档`_：提供相关文档的链接。


概述
====

ESP-LyraP-CAM 扩展板可为你的主板增加一个摄像头。


组件描述
--------

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-cam-v1.0-layout-front.png
    :align: center
    :alt: ESP-LyraP-CAM - 正面和反面
    :figclass: align-center

    ESP-LyraP-CAM - 正面和反面

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 主要组件
     - 描述
   * - 主板摄像头排针
     - 连接至主板摄像头连接器
   * - 电源 LED 指示灯
     - 如果电源供电电压正常，则红色 LED 亮起
   * - 摄像头模块连接器
     - 硬件支持 OV2640 和 OV3660 摄像头模块；目前，ESP-LyraP-CAM 扩展板默认提供 OV2640 摄像头模块
   * - 电源调节器
     - LDO 调压器（3.3 V 至 2.8 V 和 1.5 V）


应用程序开发
------------

ESP-LyraP-CAM 上电前，请首先确认开发板完好无损。


硬件准备
^^^^^^^^

- 带有摄像头扩展板连接器（排母）的主板（例如 ESP32-S2-Kaluga-1)
- ESP-LyraP-CAM 扩展板
- PC（Windows、Linux 或 macOS）


硬件设置
^^^^^^^^

将 ESP-LyraP-CAM 扩展板插入主板的连接头排母中。


软件设置
^^^^^^^^

请前往 ESP32-S2-Kaluga-1 开发套件用户指南的 :ref:`user-guide-esp32-s2-kaluga-1-kit-v1.2-software-setup` 章节。


硬件参考
========

功能框图
--------

ESP-LyraP-CAM 的主要组件和连接方式如下图所示。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-cam-v1.0-block-diagram.png
    :align: center
    :alt: ESP-LyraP-CAM 功能框图
    :figclass: align-center

    ESP-LyraP-CAM 功能框图


硬件修订历史
============

尚无版本升级历史。


相关文档
========

- `ESP-LyraP-CAM 原理图 <https://dl.espressif.com/dl/schematics/ESP-LyraP-CAM_V1.0_SCH_20200302.pdf>`_ (PDF)
- `ESP-LyraP-CAM PCB 布局图 <https://dl.espressif.com/dl/schematics/ESP-LyraP-CAM_V1.0_PCB_20200302.pdf>`_ (PDF)

有关本开发板的更多设计文档，请联系我们的商务部门 sales@espressif.com。
