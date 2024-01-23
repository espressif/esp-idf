=====================
ESP-LyraP-TouchA v1.1
=====================

:link_to_translation:`en:[English]`

本用户指南可提供 ESP-LyraP-TouchA 扩展板的相关信息。

本扩展板通常仅与乐鑫其他开发板一起销售（即 *主板*，比如 ESP32-S2-Kaluga-1），不可单独购买。

目前，ESP-LyraP-TouchA v1.1 扩展板正在搭配以下套件销售：

* :doc:`user-guide-esp32-s2-kaluga-1-kit`
* :doc:`user-guide-esp32-s2-kaluga-1-kit-v1.2`

ESP-LyraP-TouchA 可为你的主板增加触摸按键功能。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-toucha-v1.1-3d.png
    :align: center
    :alt: ESP-LyraP-TouchA
    :figclass: align-center

    ESP-LyraP-TouchA

本指南包括如下内容：

- `概述`_：提供为了使用 ESP-LyraT-8311A 而必须了解的硬件和软件信息。
- `硬件参考`_：提供 ESP-LyraP-TouchA 的详细硬件信息。
- `硬件修订历史`_：提供该开发版的“修订历史”、“已知问题”以及此前版本开发板的用户指南链接。
- `相关文档`_：提供相关文档的链接。


概述
====

ESP-LyraP-TouchA 共有 6 个触摸按钮，主要用于音频应用，但也可以根据实际需要用作它用。


组件描述
--------

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-toucha-v1.1-layout-front.png
    :align: center
    :alt: ESP-LyraP-TouchA
    :figclass: align-center

    ESP-LyraP-TouchA

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 主要组件
     - 描述
   * - 主板触摸 FPC 连接器
     - 用于将子板连接到主板的触摸 FPC 连接器。
   * - 触摸板
     - 电容式触摸电极。
   * - 保护环
     - 连接至触摸传感器，可在开发板遇水时触发中断保护（遇水电路保护）。此时，传感器阵列也将遇水，绝大多数（或全部）触摸板将由于大量误触而无法使用。在接收到此中断后，用户可自行裁决是否通过软件禁用所有触摸传感器。


应用程序开发
------------

ESP-LyraP-TouchA 上电前，请首先确认开发板完好无损。


硬件准备
^^^^^^^^

- 带有触摸 FPC 扩展板连接器的主板（例如 ESP32-S2-Kaluga-1)
- ESP-LyraP-TouchA 扩展板
- FPC 线
- PC（Windows、Linux 或 macOS）


硬件设置
^^^^^^^^

使用 FPC 连接两个 FPC 连接器。


软件设置
^^^^^^^^

请前往 ESP32-S2-Kaluga-1 开发套件用户指南的 :ref:`user-guide-esp32-s2-kaluga-1-kit-software-setup` 章节。


硬件参考
========

功能框图
--------

ESP-LyraP-TouchA 的主要组件和连接方式如下图所示。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-toucha-v1.1-block-diagram.png
    :align: center
    :alt: ESP-LyraP-TouchA-v1.1 功能框图
    :figclass: align-center

    ESP-LyraP-TouchA-v1.1 功能框图


硬件修订历史
============

尚无版本升级历史。


相关文档
========

- `ESP-LyraP-TouchA 原理图 <https://dl.espressif.com/dl/schematics/ESP-LyraP-TouchA_V1.1_SCH_20200325A.pdf>`_ (PDF)
- `ESP-LyraP-TouchA PCB 布局图 <https://dl.espressif.com/dl/schematics/ESP-LyraP-TouchA_V1.1_PCB_20200325AA.pdf>`_ (PDF)

有关本开发板的更多设计文档，请联系我们的商务部门 sales@espressif.com。
