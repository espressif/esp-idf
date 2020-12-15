====================
ESP-LyraT-8311A v1.2
====================

:link_to_translation:`en:[English]`

本用户指南可提供 ESP-LyraT-8311A 扩展板的相关信息。

本扩展板通常仅与乐鑫其他开发板一起销售（即 *主板*，比如 ESP32-S2-Kaluga-1），不可单独购买。

目前，ESP-LyraT-8311A v1.2 扩展板正在搭配 :doc:`user-guide-esp32-s2-kaluga-1-kit-v1.2` 销售。

ESP-LyraT-8311A 扩展板可为您的主板增加音频处理功能。

- 音频播放/录音
- 音频信号处理
- 支持可编程按钮，可实现轻松控制

ESP-LyraT-8311A 扩展板有多种使用方式。该应用程序包括语音用户界面、语音控制、语音授权、录音和播放等功能。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrat-8311a-v1.2-3d.png
    :align: center
    :width: 2545px
    :height: 1786px
    :scale: 30%
    :alt: ESP-LyraT-8311A
    :figclass: align-center

    ESP-LyraT-8311A（点击放大）

本指南包括如下内容：

- `概述`_：提供为了使用 ESP-LyraT-8311A 而必须了解的硬件和软件信息。
- `硬件参考`_：提供 ESP-LyraT-8311A 的详细硬件信息。
- `硬件修订历史`_：提供该开发版的“修订历史”、“已知问题”以及此前版本开发板的用户指南链接。
- `相关文档`_：提供相关文档的链接。


概述
====

ESP-LyraT-8311A 主要用于音频应用，但也可根据实际需求用作它用。

组件描述
--------

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrat-8311a-v1.2-layout-front.png
    :align: center
    :width: 934px
    :height: 565px
    :scale: 70%
    :alt: ESP-LyraT-8311A - 正面
    :figclass: align-center

    ESP-LyraT-8311A - 正面（点击放大）

下表将从图片右上角开始，以顺时针顺序介绍上图中的主要组件。

**保留** 表示该功能可用，但当前版本的套件并未启用该功能。


.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 主要组件
     - 描述
   * - 扩展板排针
     - 反面的排针用于于主板上的排母相连；排母用于连接其他带有排针的主板
   * - 功能按钮
     - 带有 6 个可编程按钮
   * - 麦克风
     - 支持驻极体和 MEMS 麦克风；此扩展板默认带有驻极体麦克风
   * - 耳机接口
     - 6.3 mm (1/8") 立体声耳机接口
   * - 扬声器连接器
     - 2 针连接器，用于连接外部扬声器
   * - PA
     - 3 W 音频信号放大器，配合外部扬声器使用
   * - 外部麦克风矩阵连接器
     - （保留）FPC 连接器，用于连接外部麦克风矩阵（麦克风开发板）
   * - ADC
     - （保留）高性能 ADC/ES7243，包括 1 个麦克风通道、1 个声学回声消除 (AEC) 功能通道
   * - 单声道音频编解器
     - ES8311 音频 ADC 和 DAC，可转换麦克风拾音的模拟信号；或转换数字信号，使其可通过扬声器或耳机进行播放



应用程序开发
------------

ESP-LyraT-8311A 上电前，请首先确认开发板完好无损。


硬件准备
^^^^^^^^

- 带有连接器（排母）的主板（例如 ESP32-S2-Kaluga-1)
- ESP-LyraT-8311A 扩展板
- 4 x 螺栓，用于保证安装稳定
- PC（Windows、Linux 或 macOS）


硬件设置
^^^^^^^^

请按照以下步骤将 ESP-LyraT-8311A 安装到带有排母的主板上：

1. 先将 4 个螺栓固定到主板的相应位置上
2. 对齐 ESP-LyraT-8311A 与主板和螺栓的位置，并小心插入


软件设置
^^^^^^^^

请根据您的具体应用，参考以下部分：

* ESP-ADF（乐鑫音频开发框架）的用户，请前往 `ESP-ADF 入门指南 <https://docs.espressif.com/projects/esp-adf/en/latest/get-started/index.html#about-esp-adf>`_。
* ESP32-IDF（乐鑫 IoT 开发框架）的用户，请前往 ESP32-S2-Kaluga-1 开发套件用户指南 :ref:`user-guide-esp32-s2-kaluga-1-kit-v1.2-software-setup` 章节。


硬件参考
========


功能框图
--------

ESP-LyraT-8311A 的主要组件和连接方式如下图所示。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrat-8311a-v1.2-block-diagram.png
    :align: center
    :alt: ESP-LyraT-8311A 功能框图
    :figclass: align-center

    ESP-LyraT-8311A 功能框图


硬件修订历史
============

尚无版本升级历史。


相关文档
========

- `ESP-LyraT-8311A 原理图 <https://dl.espressif.com/dl/schematics/ESP-LyraT-8311A_V1_2_SCH_20200421A.pdf>`_ (PDF)
- `ESP-LyraT-8311A PCB 布局图 <https://dl.espressif.com/dl/schematics/ESP-LyraT-8311A_V1_2_PCB_20200324AA.pdf>`_ (PDF)

有关本开发板的更多设计文档，请联系我们的商务部门 sales@espressif.com。